/**
 * \file error.hpp
 *
 * \author angel.castillo
 * \date   Nov 01, 2026
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
#include <stdexcept>
#include <system_error>

namespace Biglup::Cometa
{
/** Native operation failure. code() preserves the cardano-c error code and category.
 * Native allocation-error codes map to std::bad_alloc; invalid C++ arguments throw
 * std::invalid_argument. No native ownership escapes through an exception.
 */
class Error : public std::system_error
{
  public:

    /** Construct an error with its operation name and native status code. */
    Error(int code, const char* operation);
};
} // namespace Biglup::Cometa
