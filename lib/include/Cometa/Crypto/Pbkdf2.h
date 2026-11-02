/**
 * @file Pbkdf2.h
 *
 * @brief PBKDF2 HMAC-SHA512 key derivation.
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

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <vector>

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Derives bytes with PBKDF2 HMAC-SHA512.
 *
 * @param password The password bytes; may be empty. No text normalization is performed.
 * @param salt The nonempty salt.
 * @param iterations The positive iteration count.
 * @param size The positive output byte count, at most 64 times UINT32_MAX.
 *
 * @return The derived bytes in an ordinary byte vector.
 *
 * @throws std::invalid_argument The salt, iteration count, or output size is zero.
 * @throws std::length_error The output exceeds the PBKDF2 block limit.
 * @throws Error Derivation fails.
 */
std::vector<std::uint8_t> pbkdf2HmacSha512(std::string_view password, const std::vector<std::uint8_t>& salt, std::uint32_t iterations, std::size_t size);
} // namespace Cometa
