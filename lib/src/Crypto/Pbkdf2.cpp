/**
 * @file Pbkdf2.cpp
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

#include <Cometa/Crypto/Pbkdf2.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/pbkdf2.h>

#include <limits>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::vector<std::uint8_t>
pbkdf2HmacSha512(std::string_view password, const std::vector<std::uint8_t>& salt, std::uint32_t iterations, std::size_t size)
{
  if (salt.empty() || iterations == 0 || size == 0)
  {
    throw std::invalid_argument("PBKDF2 requires a salt, positive iterations, and positive output size");
  }

  if (static_cast<std::uint64_t>(size) > 64ULL * std::numeric_limits<std::uint32_t>::max())
  {
    throw std::length_error("PBKDF2 output exceeds the block limit");
  }

  std::vector<std::uint8_t> output(size);

  checkCardanoError(cardano_crypto_pbkdf2_hmac_sha512(reinterpret_cast<const std::uint8_t*>(password.empty() ? "" : password.data()), password.size(), salt.data(), salt.size(), iterations, output.data(), output.size()), "pbkdf2HmacSha512");

  return output;
}
} // namespace Cometa
