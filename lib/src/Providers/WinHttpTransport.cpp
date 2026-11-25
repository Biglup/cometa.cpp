/**
 * @file WinHttpTransport.cpp
 *
 * @brief Windows HTTPS transport.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
 *
 * Copyright 2026 Biglup Labs
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* INCLUDES ******************************************************************/

#include <Cometa/Providers/WinHttpTransport.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <winhttp.h>

#include <array>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <limits>
#include <mutex>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Maximum accepted response size. */
constexpr std::size_t MAX_RESPONSE_BYTES = 8U * 1024U * 1024U;

/** @brief Owns one WinHTTP handle. */
class WinHttpHandle
{
  public:

    /** @brief Takes ownership of a handle. @param value The handle. */
    explicit WinHttpHandle(HINTERNET value) noexcept
    : m_value(value)
    {
    }

    /** @brief Closes the handle. */
    ~WinHttpHandle()
    {
      if (m_value != nullptr)
      {
        WinHttpCloseHandle(m_value);
      }
    }

  public:

    /** @brief Gets the borrowed handle. @return The handle. */
    HINTERNET
    get() const noexcept
    {
      return m_value;
    }

  public:

    /** @brief Handles cannot be copied. */
    WinHttpHandle(const WinHttpHandle&) = delete;

    /** @brief Handles cannot be copy assigned. */
    WinHttpHandle& operator=(const WinHttpHandle&) = delete;

  private:

    /** @brief Owned handle. */
    HINTERNET m_value;
};

/** @brief Throws an operation error without including credentials. @param operation The failed operation. */
void
throwWinHttpError(const char* operation)
{
  const DWORD error = GetLastError();
  if (error == ERROR_WINHTTP_TIMEOUT)
  {
    throw std::system_error(std::make_error_code(std::errc::timed_out), operation);
  }
  throw std::runtime_error(std::string(operation) + " failed with Windows error " + std::to_string(error));
}

/** @brief Owns an asynchronous request and its callback state through handle closure. */
class AsyncRequest
{
  public:

    /** @brief Adopts a request handle. @param handle Owned request. */
    explicit AsyncRequest(HINTERNET handle)
    : m_handle(handle)
    {
      if (m_handle == nullptr)
      {
        throwWinHttpError("WinHttpOpenRequest");
      }
      DWORD_PTR context = reinterpret_cast<DWORD_PTR>(this);
      if (!WinHttpSetOption(m_handle, WINHTTP_OPTION_CONTEXT_VALUE, &context, sizeof(context)))
      {
        WinHttpCloseHandle(m_handle);
        throwWinHttpError("WinHttpSetOption");
      }
      if (WinHttpSetStatusCallback(m_handle, callback, WINHTTP_CALLBACK_FLAG_ALL_COMPLETIONS | WINHTTP_CALLBACK_FLAG_HANDLES, 0) == WINHTTP_INVALID_STATUS_CALLBACK)
      {
        WinHttpCloseHandle(m_handle);
        throwWinHttpError("WinHttpSetStatusCallback");
      }
    }

    /** @brief Cancels pending I/O and retains callback state until HANDLE_CLOSING. */
    ~AsyncRequest()
    {
      WinHttpCloseHandle(m_handle);
      std::unique_lock<std::mutex> lock(m_mutex);
      while (!m_closed)
      {
        m_changed.wait(lock);
      }
    }

    /** @brief Prevents shared ownership. */
    AsyncRequest(const AsyncRequest&) = delete;
    /** @brief Prevents shared ownership. */
    AsyncRequest& operator=(const AsyncRequest&) = delete;

    /** @brief Gets the request handle. @return Borrowed request. */
    HINTERNET
    get() const noexcept
    {
      return m_handle;
    }

    /** @brief Gets storage kept alive until pending reads are cancelled. @return Read buffer. */
    std::array<std::uint8_t, 8192>&
    buffer() noexcept
    {
      return m_buffer;
    }

    /** @brief Waits for one completion within the entire request deadline. @param deadline Absolute deadline. @return Bytes read, if applicable. */
    DWORD
    wait(std::chrono::steady_clock::time_point deadline)
    {
      std::unique_lock<std::mutex> lock(m_mutex);
      if (!m_changed.wait_until(lock, deadline, std::bind(&AsyncRequest::completed, this)))
      {
        throw std::system_error(std::make_error_code(std::errc::timed_out));
      }
      m_completed = false;
      if (m_error == ERROR_WINHTTP_TIMEOUT)
      {
        throw std::system_error(std::make_error_code(std::errc::timed_out));
      }
      if (m_error != ERROR_SUCCESS)
      {
        throw std::runtime_error("WinHTTP request failed");
      }
      return m_received;
    }

  private:

    /** @brief Tests completion while the caller holds m_mutex. @return Whether an operation completed. */
    bool
    completed() const noexcept
    {
      return m_completed;
    }

    /** @brief Records completion without issuing another request or throwing.
     * @param context Owned request state.
     * @param status Completion notification.
     * @param information Status-specific borrowed data.
     * @param length Bytes read or information size.
     * @return No return value.
     */
    static void CALLBACK
    callback(HINTERNET, DWORD_PTR context, DWORD status, LPVOID information, DWORD length)
    {
      if (context == 0)
      {
        return;
      }
      AsyncRequest*               state = reinterpret_cast<AsyncRequest*>(context);
      std::lock_guard<std::mutex> lock(state->m_mutex);
      switch (status)
      {
        case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
          state->m_closed = true;
          break;
        case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
          state->m_error     = static_cast<WINHTTP_ASYNC_RESULT*>(information)->dwError;
          state->m_completed = true;
          break;
        case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
          state->m_received  = length;
          state->m_completed = true;
          break;
        case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
          state->m_completed = true;
          break;
        default:
          break;
      }
      // Notify while holding the lock: no state access follows destruction's wait.
      state->m_changed.notify_all();
    }

    /** @brief Owned request, closed before callback state is destroyed. */
    HINTERNET m_handle;
    /** @brief Callback synchronization. */
    std::mutex m_mutex;
    /** @brief Completion notification. */
    std::condition_variable m_changed;
    /** @brief Read storage retained through cancellation. */
    std::array<std::uint8_t, 8192> m_buffer {};
    /** @brief Last asynchronous error. */
    DWORD m_error = ERROR_SUCCESS;
    /** @brief Last read length. */
    DWORD m_received = 0;
    /** @brief Completion not yet consumed by the caller. */
    bool m_completed = false;
    /** @brief Final callback received. */
    bool m_closed = false;
};

/** @brief Converts UTF-8 request text to UTF-16. @param text UTF-8 text. @return UTF-16 text. */
std::wstring
toWide(const std::string& text)
{
  if (text.find('\0') != std::string::npos || text.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
  {
    throw std::invalid_argument("Invalid HTTP text");
  }

  if (text.empty())
  {
    return {};
  }

  const int length = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), nullptr, 0);

  if (length == 0)
  {
    throw std::invalid_argument("HTTP text is not valid UTF-8");
  }

  std::wstring result(static_cast<std::size_t>(length), L'\0');

  if (MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, text.data(), static_cast<int>(text.size()), &result[0], length) != length)
  {
    throwWinHttpError("UTF-8 conversion");
  }

  return result;
}

/** @brief Rejects header line breaks. @param text The header name or value. */
void
validateHeaderText(const std::string& text)
{
  if (text.find('\r') != std::string::npos || text.find('\n') != std::string::npos)
  {
    throw std::invalid_argument("HTTP headers cannot contain line breaks");
  }
}
} // namespace

/* IMPLEMENTATION ************************************************************/

HttpResponse
WinHttpTransport::execute(const HttpRequest& request) const
{
  if (request.method != "GET" && request.method != "POST")
  {
    throw std::invalid_argument("Unsupported HTTP method");
  }

  if (request.url.compare(0, 8, "https://") != 0 && request.url.compare(0, 7, "http://") != 0)
  {
    throw std::invalid_argument("HTTP or HTTPS is required");
  }

  if (request.body.size() > static_cast<std::size_t>(std::numeric_limits<DWORD>::max()))
  {
    throw std::out_of_range("HTTP request body is too large");
  }

  if (request.timeout.count() < 1 || request.timeout.count() > 60000)
  {
    throw std::invalid_argument("HTTP timeout must be between 1 and 60000 milliseconds");
  }
  const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + request.timeout;
  const std::wstring                          url      = toWide(request.url);
  URL_COMPONENTS                              components {};
  components.dwStructSize      = sizeof(components);
  components.dwSchemeLength    = static_cast<DWORD>(-1);
  components.dwHostNameLength  = static_cast<DWORD>(-1);
  components.dwUrlPathLength   = static_cast<DWORD>(-1);
  components.dwExtraInfoLength = static_cast<DWORD>(-1);

  if (!WinHttpCrackUrl(url.c_str(), 0, 0, &components) || (components.nScheme != INTERNET_SCHEME_HTTPS && components.nScheme != INTERNET_SCHEME_HTTP) || components.dwHostNameLength == 0)
  {
    throw std::invalid_argument("Invalid HTTP URL");
  }

  const std::wstring host(components.lpszHostName, components.dwHostNameLength);
  std::wstring       path = components.dwUrlPathLength == 0
          ? L"/"
          : std::wstring(components.lpszUrlPath, components.dwUrlPathLength);

  if (components.dwExtraInfoLength != 0)
  {
    path.append(components.lpszExtraInfo, components.dwExtraInfoLength);
  }

  const WinHttpHandle session(WinHttpOpen(
    L"Cometa.cpp", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC));

  if (session.get() == nullptr)
  {
    throwWinHttpError("WinHttpOpen");
  }

  if (!WinHttpSetTimeouts(session.get(), static_cast<int>(request.timeout.count()), static_cast<int>(request.timeout.count()), static_cast<int>(request.timeout.count()), static_cast<int>(request.timeout.count())))
  {
    throwWinHttpError("WinHttpSetTimeouts");
  }

  const WinHttpHandle connection(WinHttpConnect(session.get(), host.c_str(), components.nPort, 0));

  if (connection.get() == nullptr)
  {
    throwWinHttpError("WinHttpConnect");
  }

  const std::wstring method = toWide(request.method);
  AsyncRequest       handle(WinHttpOpenRequest(
    connection.get(), method.c_str(), path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, components.nScheme == INTERNET_SCHEME_HTTPS ? WINHTTP_FLAG_SECURE : 0));

  if (handle.get() == nullptr)
  {
    throwWinHttpError("WinHttpOpenRequest");
  }

  DWORD redirectPolicy = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;

  if (!WinHttpSetOption(handle.get(), WINHTTP_OPTION_REDIRECT_POLICY, &redirectPolicy, sizeof(redirectPolicy)))
  {
    throwWinHttpError("WinHttpSetOption");
  }

  for (const std::pair<std::string, std::string>& header: request.headers)
  {
    validateHeaderText(header.first);
    validateHeaderText(header.second);

    const std::wstring line = toWide(header.first + ": " + header.second);

    if (!WinHttpAddRequestHeaders(
          handle.get(), line.c_str(), static_cast<DWORD>(line.size()), WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE))
    {
      throwWinHttpError("WinHttpAddRequestHeaders");
    }
  }

  const DWORD bodySize = static_cast<DWORD>(request.body.size());
  void* const body     = request.body.empty()
        ? WINHTTP_NO_REQUEST_DATA
        : const_cast<std::uint8_t*>(request.body.data());

  if (!WinHttpSendRequest(handle.get(), WINHTTP_NO_ADDITIONAL_HEADERS, 0, body, bodySize, bodySize, reinterpret_cast<DWORD_PTR>(&handle)))
  {
    throwWinHttpError("WinHttpSendRequest");
  }

  handle.wait(deadline);

  if (!WinHttpReceiveResponse(handle.get(), nullptr))
  {
    throwWinHttpError("WinHttpReceiveResponse");
  }

  handle.wait(deadline);

  DWORD status     = 0;
  DWORD statusSize = sizeof(status);

  if (!WinHttpQueryHeaders(
        handle.get(), WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, WINHTTP_HEADER_NAME_BY_INDEX, &status, &statusSize, WINHTTP_NO_HEADER_INDEX))
  {
    throwWinHttpError("WinHttpQueryHeaders");
  }

  HttpResponse response { static_cast<long>(status), {} };

  while (true)
  {
    if (!WinHttpReadData(handle.get(), handle.buffer().data(), static_cast<DWORD>(handle.buffer().size()), nullptr))
    {
      throwWinHttpError("WinHttpReadData");
    }
    const DWORD received = handle.wait(deadline);
    if (received == 0)
    {
      break;
    }
    if (received > MAX_RESPONSE_BYTES - response.body.size())
    {
      throw std::runtime_error("HTTP response exceeds 8 MiB");
    }
    response.body.insert(response.body.end(), handle.buffer().begin(), handle.buffer().begin() + received);
  }

  return response;
}
