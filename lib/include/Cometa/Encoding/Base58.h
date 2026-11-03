/**
 * @file Base58.h
 *
 * @brief Base58 conversion for binary payloads such as legacy address encodings.
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
#include <string_view>
#include <vector>

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Encodes bytes with the Bitcoin Base58 alphabet.
 *
 * @param bytes The input bytes; empty input produces an empty string.
 *
 * @return The encoded text, preserving leading zero bytes as leading ones.
 *
 * @throws std::length_error The input exceeds the supported buffer capacity.
 * @throws Error Encoding fails.
 */
std::string encodeBase58(const std::vector<std::uint8_t>& bytes);

/**
 * @brief Decodes bytes from the Bitcoin Base58 alphabet.
 *
 * @param text The encoded text; empty input produces an empty vector. Whitespace is not accepted.
 *
 * @return The decoded bytes, preserving leading ones as leading zero bytes.
 *
 * @throws std::invalid_argument The text contains an invalid character.
 * @throws Error Decoding fails.
 */
std::vector<std::uint8_t> decodeBase58(std::string_view text);
} // namespace Cometa
