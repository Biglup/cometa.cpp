/**
 * @file Error.h
 *
 * @brief Declaration of the native operation exception.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <stdexcept>
#include <system_error>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Reports a failed native operation through std::system_error.
 *
 * The inherited code() retains the native status and the cardano-c category.
 * Native allocation-error codes are translated to std::bad_alloc instead.
 * Invalid C++ arguments throw std::invalid_argument.
 */
class Error : public std::system_error
{
  public:

    /**
     * @brief Initializes an exception with the native status and operation name.
     *
     * @param code      The native error code.
     * @param operation The operation name included in what(); must not be null.
     */
    Error(int code, const char* operation);
};
} // namespace Cometa
