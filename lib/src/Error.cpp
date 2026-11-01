/**
 * @file Error.cpp
 *
 * @brief Implementation of the Error value type.
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

/* INCLUDES ******************************************************************/

#include <cometa/Error.h>

#include <cardano/error.h>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Provides messages for the native cardano-c error category.
 */
class NativeCategory final : public std::error_category
{
  public:

    const char*
    name() const noexcept override
    {
      return "cardano-c";
    }

    std::string
    message(int value) const override
    {
      return cardano_error_to_string(static_cast<cardano_error_t>(value));
    }
};

const NativeCategory NATIVE_CATEGORY;
} // namespace

/* CLASS IMPLEMENTATION ******************************************************/

Error::Error(int code, const char* operation)
: std::system_error(code, NATIVE_CATEGORY, operation)
{
}
} // namespace Cometa
