/**
 * @file Error.h
 *
 * @brief Declaration of the operation exception.
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

#include <Cometa/ErrorCode.h>

#include <system_error>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Reports an operation failure.
 *
 * Compare code() with makeErrorCode(ErrorCode) to identify the failure.
 */
class Error : public std::system_error
{
  public:

    /**
     * @brief Initializes an exception with the error code and operation name.
     *
     * @param code      The operation error code.
     * @param operation The operation name included in what(); must not be null.
     */
    Error(ErrorCode code, const char* operation);
};
} // namespace Cometa
