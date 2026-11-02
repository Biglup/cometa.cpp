/**
 * @file Bip39.h
 *
 * @brief English BIP39 mnemonic conversion.
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
 * @brief Converts entropy to an English BIP39 mnemonic.
 *
 * @param entropy Exactly 16, 20, 24, 28, or 32 bytes.
 *
 * @return The 12, 15, 18, 21, or 24 mnemonic words.
 *
 * @throws Error The entropy size is invalid.
 */
std::vector<std::string> entropyToMnemonic(const std::vector<std::uint8_t>& entropy);

/**
 * @brief Validates an English BIP39 mnemonic and extracts its entropy.
 *
 * @param words Exactly 12, 15, 18, 21, or 24 lowercase English words.
 *
 * @return The entropy bytes.
 *
 * @throws std::invalid_argument A word contains an embedded null character.
 * @throws Error The word count, a word, or the checksum is invalid.
 */
std::vector<std::uint8_t> mnemonicToEntropy(const std::vector<std::string>& words);
} // namespace Cometa
