/**
 * @file CborSimpleValue.h
 *
 * @brief CBOR simple value.
 *
 * @author angel.castillo
 * @date   Nov 02, 2026
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

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Identifies CBOR boolean, null, and undefined simple values.
 *
 * These values belong to CBOR major type 7 and are distinct from integers with the same numeric
 * arguments.
 */
enum class CborSimpleValue : std::uint32_t
{
  /** @brief Represents the value 'false'. */
  False = 20U,

  /** @brief Represents the value 'true'. */
  True = 21U,

  /** @brief Represents the value 'null'. */
  Null = 22U,

  /** @brief Represents an undefined value. */
  Undefined = 23U,
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Gets a descriptive name.
 * @param value The value to describe.
 * @return Its name, or an unknown-value description.
 */
std::string toString(CborSimpleValue value);
} // namespace Cometa
