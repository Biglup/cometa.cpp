/**
 * @file HttpTransport.h
 *
 * @brief HTTP requests used by network providers.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/** @brief An HTTP request with binary body support. */
struct HttpRequest
{
    /** @brief HTTP method, such as GET or POST. */
    std::string method;

    /** @brief Absolute request URL. */
    std::string url;

    /** @brief Request header names and values. */
    std::vector<std::pair<std::string, std::string>> headers;

    /** @brief Request bytes. */
    std::vector<std::uint8_t> body;
};

/** @brief An HTTP response with binary body support. */
struct HttpResponse
{
    /** @brief HTTP status code. */
    long status;

    /** @brief Response bytes. */
    std::vector<std::uint8_t> body;
};
} // namespace Cometa

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Executes HTTP requests for a network provider. */
class HttpTransport
{
  public:

    /** @brief Destroys the transport. */
    virtual ~HttpTransport() = default;

  public:

    /** @brief Executes a request. @param request The request. @return The response. */
    virtual HttpResponse execute(const HttpRequest& request) const = 0;
};
} // namespace Cometa
