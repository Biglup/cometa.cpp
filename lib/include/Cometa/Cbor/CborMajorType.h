/**
 * @file CborMajorType.h
 *
 * @brief CBOR major type.
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
/** @brief CBOR major type. */
enum class CborMajorType : std::uint32_t
{
  /** @brief An unsigned integer. */
  UnsignedInteger = 0U,

  /** @brief A negative integer. */
  NegativeInteger = 1U,

  /** @brief A byte string. */
  ByteString = 2U,

  /** @brief A text string encoded as UTF-8. */
  Utf8String = 3U,

  /** @brief An array of data items. */
  Array = 4U,

  /** @brief A map of pairs of data items. */
  Map = 5U,

  /** @brief A tagged data item ("tag"). */
  Tag = 6U,

  /** @brief Simple values, floating-point numbers, and the "break" stop code. */
  Simple = 7U,

  /** @brief Undefined major type. */
  Undefined = 4294967295U,
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Gets a descriptive name.
 * @param value The value to describe.
 * @return Its name, or an unknown-value description.
 */
std::string toString(CborMajorType value);
} // namespace Cometa
