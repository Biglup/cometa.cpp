/**
 * @file Bech32.h
 *
 * @brief Checksummed Bech32 encoding for Cardano addresses and keys.
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

#include <Cometa/Encoding/Bech32Data.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Encodes a binary payload as Bech32, supporting Cardano-length strings.
 *
 * @param prefix The human-readable prefix: 1 to 83 printable ASCII characters. Letters are normalized to lowercase.
 * @param bytes The binary payload; may be empty.
 *
 * @return The lowercase Bech32 text including its checksum.
 *
 * @throws std::invalid_argument The prefix is invalid.
 * @throws std::length_error The payload exceeds the supported buffer capacity.
 * @throws Error Encoding fails.
 */
std::string encodeBech32(const std::string_view& prefix, const std::vector<std::uint8_t>& bytes);

/**
 * @brief Decodes Bech32 text and validates its checksum and bit padding.
 *
 * @param text The encoded text. Uppercase or lowercase is accepted, but mixed case is not.
 *
 * @return The lowercase prefix and decoded payload; both are independent values.
 *
 * @throws std::invalid_argument The prefix, separator, length, or character range is invalid.
 * @throws std::length_error The text exceeds the supported buffer capacity.
 * @throws Error The checksum, case, payload, or padding is invalid.
 */
Bech32Data decodeBech32(std::string_view text);
} // namespace Cometa
