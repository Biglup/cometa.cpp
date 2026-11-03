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

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Maximum accepted response size. */
constexpr std::size_t MAX_RESPONSE_BYTES = 16U * 1024U * 1024U;

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
  throw std::runtime_error(std::string(operation) + " failed with Windows error " + std::to_string(GetLastError()));
}

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

  if (request.url.compare(0, 8, "https://") != 0)
  {
    throw std::invalid_argument("HTTPS is required");
  }

  if (request.body.size() > static_cast<std::size_t>(std::numeric_limits<DWORD>::max()))
  {
    throw std::out_of_range("HTTP request body is too large");
  }

  const std::wstring url = toWide(request.url);
  URL_COMPONENTS     components {};
  components.dwStructSize      = sizeof(components);
  components.dwSchemeLength    = static_cast<DWORD>(-1);
  components.dwHostNameLength  = static_cast<DWORD>(-1);
  components.dwUrlPathLength   = static_cast<DWORD>(-1);
  components.dwExtraInfoLength = static_cast<DWORD>(-1);

  if (!WinHttpCrackUrl(url.c_str(), 0, 0, &components) || components.nScheme != INTERNET_SCHEME_HTTPS || components.dwHostNameLength == 0)
  {
    throw std::invalid_argument("Invalid HTTPS URL");
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
    L"Cometa.cpp", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0));

  if (session.get() == nullptr)
  {
    throwWinHttpError("WinHttpOpen");
  }

  if (!WinHttpSetTimeouts(session.get(), 10000, 10000, 30000, 30000))
  {
    throwWinHttpError("WinHttpSetTimeouts");
  }

  const WinHttpHandle connection(WinHttpConnect(session.get(), host.c_str(), components.nPort, 0));

  if (connection.get() == nullptr)
  {
    throwWinHttpError("WinHttpConnect");
  }

  const std::wstring  method = toWide(request.method);
  const WinHttpHandle handle(WinHttpOpenRequest(
    connection.get(), method.c_str(), path.c_str(), nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE));

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

  if (!WinHttpSendRequest(handle.get(), WINHTTP_NO_ADDITIONAL_HEADERS, 0, body, bodySize, bodySize, 0))
  {
    throwWinHttpError("WinHttpSendRequest");
  }

  if (!WinHttpReceiveResponse(handle.get(), nullptr))
  {
    throwWinHttpError("WinHttpReceiveResponse");
  }

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
    DWORD available = 0;

    if (!WinHttpQueryDataAvailable(handle.get(), &available))
    {
      throwWinHttpError("WinHttpQueryDataAvailable");
    }

    if (available == 0)
    {
      break;
    }

    if (available > MAX_RESPONSE_BYTES - response.body.size())
    {
      throw std::runtime_error("HTTP response exceeds 16 MiB");
    }

    const std::size_t start = response.body.size();
    response.body.resize(start + available);

    DWORD received = 0;

    if (!WinHttpReadData(handle.get(), response.body.data() + start, available, &received))
    {
      throwWinHttpError("WinHttpReadData");
    }

    response.body.resize(start + received);

    if (received == 0)
    {
      break;
    }
  }

  return response;
}
