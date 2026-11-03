/**
 * @file CurlHttpTransport.cpp
 *
 * @brief libcurl HTTP transport.
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

#include <Cometa/Providers/CurlHttpTransport.h>

#include <curl/curl.h>

#include <limits>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Maximum accepted response size. */
constexpr std::size_t MAX_RESPONSE_BYTES = 16U * 1024U * 1024U;

/** @brief Appends a bounded libcurl response chunk. @param contents The bytes. @param size Element size. @param count Element count. @param context Response vector. @return Consumed bytes. */
std::size_t
appendResponse(char* contents, std::size_t size, std::size_t count, void* context)
{
  std::vector<std::uint8_t>* response = static_cast<std::vector<std::uint8_t>*>(context);

  if (size != 0 && count > std::numeric_limits<std::size_t>::max() / size)
  {
    return 0;
  }

  const std::size_t length = size * count;

  if (length > MAX_RESPONSE_BYTES - response->size())
  {
    return 0;
  }

  response->insert(response->end(), contents, contents + length);
  return length;
}

/** @brief Deleter for a curl easy handle. */
struct CurlDeleter
{
    /** @brief Releases the handle. @param handle The handle. */
    void
    operator()(CURL* handle) const
    {
      curl_easy_cleanup(handle);
    }
};

/** @brief Deleter for a curl header list. */
struct HeaderDeleter
{
    /** @brief Releases the list. @param headers The list. */
    void
    operator()(curl_slist* headers) const
    {
      curl_slist_free_all(headers);
    }
};

/** @brief Initializes libcurl once for the process. */
void
initializeCurl()
{
  static std::once_flag flag;
  std::call_once(flag, []()
                 {
                   if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
                   {
                     throw std::runtime_error("Could not initialize libcurl");
                   } });
}
} // namespace

/* IMPLEMENTATION ************************************************************/

HttpResponse
CurlHttpTransport::execute(const HttpRequest& request) const
{
  if (request.method != "GET" && request.method != "POST")
  {
    throw std::invalid_argument("Unsupported HTTP method");
  }

  if (request.url.compare(0, 8, "https://") != 0)
  {
    throw std::invalid_argument("HTTPS is required");
  }

  initializeCurl();

  const std::unique_ptr<CURL, CurlDeleter> handle(curl_easy_init());

  if (!handle)
  {
    throw std::runtime_error("Could not create a libcurl request");
  }

  std::unique_ptr<curl_slist, HeaderDeleter> headers;

  for (const std::pair<std::string, std::string>& header: request.headers)
  {
    const std::string line = header.first + ": " + header.second;
    curl_slist*       next = curl_slist_append(headers.get(), line.c_str());

    if (next == nullptr)
    {
      throw std::bad_alloc();
    }

    headers.release();
    headers.reset(next);
  }

  HttpResponse response { 0, {} };
  curl_easy_setopt(handle.get(), CURLOPT_URL, request.url.c_str());
  curl_easy_setopt(handle.get(), CURLOPT_HTTPHEADER, headers.get());
  curl_easy_setopt(handle.get(), CURLOPT_WRITEFUNCTION, &appendResponse);
  curl_easy_setopt(handle.get(), CURLOPT_WRITEDATA, &response.body);
  curl_easy_setopt(handle.get(), CURLOPT_CONNECTTIMEOUT_MS, 10000L);
  curl_easy_setopt(handle.get(), CURLOPT_TIMEOUT_MS, 30000L);
  curl_easy_setopt(handle.get(), CURLOPT_NOSIGNAL, 1L);
  curl_easy_setopt(handle.get(), CURLOPT_FOLLOWLOCATION, 0L);
  curl_easy_setopt(handle.get(), CURLOPT_SSL_VERIFYPEER, 1L);
  curl_easy_setopt(handle.get(), CURLOPT_SSL_VERIFYHOST, 2L);

  if (request.method == "POST")
  {
    if (request.body.size() > static_cast<std::size_t>(std::numeric_limits<curl_off_t>::max()))
    {
      throw std::out_of_range("HTTP request body is too large");
    }

    curl_easy_setopt(handle.get(), CURLOPT_POST, 1L);
    const char* body = request.body.empty() ? "" : reinterpret_cast<const char*>(request.body.data());
    curl_easy_setopt(handle.get(), CURLOPT_POSTFIELDS, body);
    curl_easy_setopt(handle.get(), CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(request.body.size()));
  }

  const CURLcode status = curl_easy_perform(handle.get());

  if (status != CURLE_OK)
  {
    throw std::runtime_error(std::string("Blockfrost request failed: ") + curl_easy_strerror(status));
  }

  curl_easy_getinfo(handle.get(), CURLINFO_RESPONSE_CODE, &response.status);
  return response;
}
