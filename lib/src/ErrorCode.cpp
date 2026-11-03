/**
 * @file ErrorCode.cpp
 *
 * @brief Error category and status conversion.
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

#include <Cometa/ErrorCode.h>

#include <cardano/error.h>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/** @brief Supplies the category name and messages for Cometa errors. */
class CometaCategory final : public std::error_category
{
  public:

    /**
     * @brief Gets the error category name.
     *
     * @return The string Cometa.
     */
    const char*
    name() const noexcept override
    {
      return "Cometa";
    }

    /**
     * @brief Gets the description of an error code.
     *
     * @param value The numeric error code.
     *
     * @return The error description.
     */
    std::string
    message(int value) const override
    {
      return cardano_error_to_string(static_cast<cardano_error_t>(value));
    }
};

} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

std::error_code
makeErrorCode(ErrorCode code) noexcept
{
  static const CometaCategory CATEGORY;

  return { static_cast<int>(code), CATEGORY };
}

std::error_code
make_error_code(ErrorCode code) noexcept
{
  return makeErrorCode(code);
}
} // namespace Cometa
