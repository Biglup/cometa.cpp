/**
 * @file Emip3.h
 *
 * @brief EMIP-003 password encryption.
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
 * @brief Encrypts bytes in the EMIP-003 format with a random salt and nonce.
 *
 * @param bytes The plaintext; may be empty.
 * @param password The password bytes; may be empty. No text normalization is performed.
 *
 * @return Salt, nonce, authentication tag, and ciphertext in one byte vector.
 *
 * @throws std::length_error The plaintext size exceeds the supported capacity.
 * @throws Error Encryption fails.
 */
std::vector<std::uint8_t> encryptEmip3(const std::vector<std::uint8_t>& bytes, std::string_view password);

/**
 * @brief Authenticates and decrypts an EMIP-003 byte sequence.
 *
 * @param encrypted The encoded salt, nonce, authentication tag, and ciphertext.
 * @param password The password bytes; may be empty. No text normalization is performed.
 *
 * @return The plaintext in an ordinary byte vector.
 *
 * @throws Error The input is truncated, authentication fails, or decryption fails.
 */
std::vector<std::uint8_t> decryptEmip3(const std::vector<std::uint8_t>& encrypted, std::string_view password);
} // namespace Cometa
