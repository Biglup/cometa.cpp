/**
 * @file Cip8.cpp
 *
 * @brief CIP-8 message signing.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

#include <Cometa/MessageSigning/Cip8.h>

#include "../CardanoInterop.h"

#include <cardano/address/address.h>
#include <cardano/buffer.h>
#include <cardano/cardano.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/crypto/ed25519_private_key.h>
#include <cardano/message_signing/cip8.h>

#include <stdexcept>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Creates a native signing key and clears the temporary key bytes.
 * @param key The signing key.
 * @return The native key.
 */
CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref>
nativeKey(const Ed25519PrivateKey& key)
{
  std::vector<std::uint8_t>      bytes  = key.toBytes();
  cardano_ed25519_private_key_t* value  = nullptr;
  cardano_error_t                status = CARDANO_ERROR_INVALID_ARGUMENT;

  if (bytes.size() == 32)
  {
    status = cardano_ed25519_private_key_from_normal_bytes(bytes.data(), bytes.size(), &value);
  }
  else if (bytes.size() == 64)
  {
    status = cardano_ed25519_private_key_from_extended_bytes(bytes.data(), bytes.size(), &value);
  }

  cardano_memzero(bytes.data(), bytes.size());
  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> owner(value);
  checkCardanoError(status, "signCip8 key");
  return owner;
}

/**
 * @brief Copies the COSE outputs into independent byte vectors.
 * @param sign1 The signed message.
 * @param key The verification key.
 * @return The signature pair.
 */
Cip8Signature
copySignature(const cardano_buffer_t* sign1, const cardano_buffer_t* key)
{
  const std::uint8_t* sign1Bytes = cardano_buffer_get_data(sign1);
  const std::uint8_t* keyBytes   = cardano_buffer_get_data(key);

  Cip8Signature result;
  result.coseSign1.assign(sign1Bytes, sign1Bytes + cardano_buffer_get_size(sign1));
  result.coseKey.assign(keyBytes, keyBytes + cardano_buffer_get_size(key));
  return result;
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

Cip8Signature
signCip8(const std::vector<std::uint8_t>& message, const Address& address, const Ed25519PrivateKey& key)
{
  const std::vector<std::uint8_t>                         addressBytes  = address.toBytes();
  cardano_address_t*                                      addressValue  = nullptr;
  const cardano_error_t                                   addressStatus = cardano_address_from_bytes(byteData(addressBytes), addressBytes.size(), &addressValue);
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(addressValue);
  checkCardanoError(addressStatus, "signCip8 address");

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> signingKey = nativeKey(key);
  cardano_buffer_t*                                                               sign1      = nullptr;
  cardano_buffer_t*                                                               coseKey    = nullptr;
  const cardano_error_t                                                           status     = cardano_cip8_sign(byteData(message), message.size(), nativeAddress.get(), signingKey.get(), &sign1, &coseKey);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref>                           sign1Owner(sign1);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref>                           keyOwner(coseKey);
  checkCardanoError(status, "signCip8 address");
  return copySignature(sign1Owner.get(), keyOwner.get());
}

Cip8Signature
signCip8(const std::vector<std::uint8_t>& message, const Blake2bHash& keyHash, const Ed25519PrivateKey& key)
{
  const std::vector<std::uint8_t>                                   hashBytes  = keyHash.toBytes();
  cardano_blake2b_hash_t*                                           hashValue  = nullptr;
  const cardano_error_t                                             hashStatus = cardano_blake2b_hash_from_bytes(byteData(hashBytes), hashBytes.size(), &hashValue);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> nativeHash(hashValue);
  checkCardanoError(hashStatus, "signCip8 keyHash");

  CardanoObject<cardano_ed25519_private_key_t, cardano_ed25519_private_key_unref> signingKey = nativeKey(key);
  cardano_buffer_t*                                                               sign1      = nullptr;
  cardano_buffer_t*                                                               coseKey    = nullptr;
  const cardano_error_t                                                           status     = cardano_cip8_sign_ex(byteData(message), message.size(), nativeHash.get(), signingKey.get(), &sign1, &coseKey);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref>                           sign1Owner(sign1);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref>                           keyOwner(coseKey);
  checkCardanoError(status, "signCip8 keyHash");
  return copySignature(sign1Owner.get(), keyOwner.get());
}
} // namespace Cometa
