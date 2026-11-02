/**
 * @file Emip3.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/Crypto/Emip3.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/emip3.h>

#include <limits>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Erases a temporary plaintext buffer before releasing it.
 *
 * @param buffer The owned buffer reference to erase and release.
 */
void
releasePlaintext(cardano_buffer_t** buffer)
{
  if (*buffer != nullptr)
  {
    cardano_buffer_memzero(*buffer);
  }

  cardano_buffer_unref(buffer);
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

std::vector<std::uint8_t>
encryptEmip3(const std::vector<std::uint8_t>& bytes, std::string_view password)
{
  if (bytes.size() > std::numeric_limits<std::size_t>::max() - 60)
  {
    throw std::length_error("EMIP-003 plaintext exceeds the supported capacity");
  }

  cardano_buffer_t*     buffer = nullptr;
  const cardano_error_t result = cardano_crypto_emip3_encrypt(byteData(bytes), bytes.size(), reinterpret_cast<const std::uint8_t*>(password.empty() ? "" : password.data()), password.size(), &buffer);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(buffer);

  checkCardanoError(result, "encryptEmip3");

  const std::uint8_t* data = cardano_buffer_get_data(owner.get());

  return std::vector<std::uint8_t>(data, data + cardano_buffer_get_size(owner.get()));
}

std::vector<std::uint8_t>
decryptEmip3(const std::vector<std::uint8_t>& encrypted, std::string_view password)
{
  cardano_buffer_t*     buffer = nullptr;
  const cardano_error_t result = cardano_crypto_emip3_decrypt(byteData(encrypted), encrypted.size(), reinterpret_cast<const std::uint8_t*>(password.empty() ? "" : password.data()), password.size(), &buffer);

  CardanoObject<cardano_buffer_t, releasePlaintext> owner(buffer);

  checkCardanoError(result, "decryptEmip3");

  const std::uint8_t* data = cardano_buffer_get_data(owner.get());

  return std::vector<std::uint8_t>(data, data + cardano_buffer_get_size(owner.get()));
}
} // namespace Cometa
