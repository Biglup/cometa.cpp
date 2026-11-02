/**
 * @file CborReaderState.h
 *
 * @brief CBOR reader state.
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
/** @brief CBOR reader state. */
enum class CborReaderState : std::uint32_t
{
  /** @brief Indicates the undefined state. */
  Undefined = 0U,

  /** @brief Indicates that the next CBOR data item is an unsigned integer (major type 0). */
  UnsignedInteger = 1U,

  /** @brief Indicates that the next CBOR data item is a negative integer (major type 1). */
  NegativeInteger = 2U,

  /** @brief Indicates that the next CBOR data item is a byte string (major type 2). */
  ByteString = 3U,

  /** @brief Indicates the start of an indefinite-length byte string (major type 2). */
  StartIndefiniteLengthByteString = 4U,

  /** @brief Indicates the end of an indefinite-length byte string (major type 2). */
  EndIndefiniteLengthByteString = 5U,

  /** @brief Indicates that the next CBOR data item is a UTF-8 string (major type 3). */
  TextString = 6U,

  /** @brief Indicates the start of an indefinite-length UTF-8 text string (major type 3). */
  StartIndefiniteLengthTextString = 7U,

  /** @brief Indicates the end of an indefinite-length UTF-8 text string (major type 3). */
  EndIndefiniteLengthTextString = 8U,

  /** @brief Indicates the start of an array (major type 4). */
  StartArray = 9U,

  /** @brief Indicates the end of an array (major type 4). */
  EndArray = 10U,

  /** @brief Indicates the start of a map (major type 5). */
  StartMap = 11U,

  /** @brief Indicates the end of a map (major type 5). */
  EndMap = 12U,

  /** @brief Indicates that the next CBOR data item is a semantic reader_state (major type 6). */
  Tag = 13U,

  /** @brief Indicates that the next CBOR data item is a simple value (major type 7). */
  SimpleValue = 14U,

  /** @brief Indicates an IEEE 754 Half-Precision float (major type 7). */
  HalfPrecisionFloat = 15U,

  /** @brief Indicates an IEEE 754 Single-Precision float (major type 7). */
  SinglePrecisionFloat = 16U,

  /** @brief Indicates an IEEE 754 Double-Precision float (major type 7). */
  DoublePrecisionFloat = 17U,

  /** @brief Indicates a null literal (major type 7). */
  Null = 18U,

  /** @brief Indicates a bool value (major type 7). */
  Boolean = 19U,

  /** @brief Indicates the completion of reading a full CBOR document. */
  Finished = 20U,
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Gets a descriptive name.
 * @param value The value to describe.
 * @return Its name, or an unknown-value description.
 */
std::string toString(CborReaderState value);
} // namespace Cometa
