/**
 * @file CborTag.h
 *
 * @brief CBOR tag.
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
/** @brief CBOR tag. */
enum class CborTag : std::uint32_t
{
  /** @brief Tag value for RFC3339 date/time strings. */
  DateTimeString = 0U,

  /** @brief Tag value for Epoch-based date/time in seconds. */
  UnixTimeSeconds = 1U,

  /** @brief Tag value for unsigned bignum encodings. */
  UnsignedBigNum = 2U,

  /** @brief Tag value for negative bignum encodings. */
  NegativeBigNum = 3U,

  /** @brief Tag value for decimal fraction encodings. */
  DecimalFraction = 4U,

  /** @brief Tag value for big float encodings. */
  BigFloat = 5U,

  /** @brief Tag value for byte strings containing embedded CBOR data item encodings. */
  Set = 258U,

  /** @brief Tag value for the Self-Describe CBOR header (0xd9d9f7). */
  SelfDescribeCbor = 55799U,
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Gets a descriptive name.
 * @param value The value to describe.
 * @return Its name, or an unknown-value description.
 */
std::string toString(CborTag value);
} // namespace Cometa
