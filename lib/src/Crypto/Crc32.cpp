/**
 * @file Crc32.cpp
 *
 * @brief BIP32 derivation indices.
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

/* INCLUDES ******************************************************************/

#include <Cometa/Crypto/Crc32.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/crc32.h>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::uint32_t
crc32(const std::vector<std::uint8_t>& bytes) noexcept
{
  return cardano_checksum_crc32(byteData(bytes), bytes.size());
}
} // namespace Cometa
