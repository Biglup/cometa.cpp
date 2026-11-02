/**
 * @file BlsKey.cpp
 *
 * @brief A BLS public key and proof of possession.
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

#include <Cometa/PoolParams/BlsKey.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/bls_key.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
BlsKey::BlsKey(const std::vector<std::uint8_t>& publicKey, const std::vector<std::uint8_t>& possessionProof)
: m_value(nullptr)
{
  cardano_bls_key_t*    value  = nullptr;
  const cardano_error_t result = cardano_bls_key_new(byteData(publicKey), publicKey.size(), byteData(possessionProof), possessionProof.size(), &value);

  CardanoObject<cardano_bls_key_t, cardano_bls_key_unref> owner(value);

  checkCardanoError(result, "BlsKey");

  m_value = owner.release();
}

BlsKey::BlsKey(const BlsKey& other)
: m_value(nullptr)
{
  cardano_bls_key_t*    value  = nullptr;
  const cardano_error_t result = cardano_bls_key_deep_clone(other.checked(), &value);

  CardanoObject<cardano_bls_key_t, cardano_bls_key_unref> owner(value);

  checkCardanoError(result, "BlsKey");

  m_value = owner.release();
}

BlsKey::BlsKey(BlsKey&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

BlsKey::~BlsKey()
{
  cardano_bls_key_unref(&m_value);
}

BlsKey
BlsKey::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return BlsKey(*decodeCbor<cardano_bls_key_t, cardano_bls_key_unref>(cbor, cardano_bls_key_from_cbor));
}

BlsKey
BlsKey::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

BlsKey
BlsKey::fromHex(std::string_view publicKeyHex, std::string_view possessionProofHex)
{
  return BlsKey(decodeHex(publicKeyHex), decodeHex(possessionProofHex));
}

std::vector<std::uint8_t>
BlsKey::publicKey() const
{
  const cardano_bls_key_t* value = checked();
  const std::uint8_t*      bytes = cardano_bls_key_get_public_key_bytes(value);

  return std::vector<std::uint8_t>(bytes, bytes + cardano_bls_key_get_public_key_size(value));
}

std::vector<std::uint8_t>
BlsKey::possessionProof() const
{
  const cardano_bls_key_t* value = checked();
  const std::uint8_t*      bytes = cardano_bls_key_get_possession_proof_bytes(value);

  return std::vector<std::uint8_t>(bytes, bytes + cardano_bls_key_get_possession_proof_size(value));
}

std::vector<std::uint8_t>
BlsKey::toCbor() const
{
  return encodeCbor(checked(), cardano_bls_key_to_cbor);
}

std::string
BlsKey::toJson() const
{
  return encodeJson(checked(), cardano_bls_key_to_cip116_json);
}

void
BlsKey::swap(BlsKey& other) noexcept
{
  std::swap(m_value, other.m_value);
}

BlsKey&
BlsKey::operator=(const BlsKey& other)
{
  if (this != &other)
  {
    BlsKey copy(other);

    swap(copy);
  }

  return *this;
}

BlsKey&
BlsKey::operator=(BlsKey&& other) noexcept
{
  if (this != &other)
  {
    cardano_bls_key_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
BlsKey::operator==(const BlsKey& other) const
{
  return publicKey() == other.publicKey() && possessionProof() == other.possessionProof();
}

bool
BlsKey::operator!=(const BlsKey& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

BlsKey::BlsKey(cardano_bls_key_t& value) noexcept
: m_value(&value)
{
}

cardano_bls_key_t*
BlsKey::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from BlsKey");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(BlsKey& lhs, BlsKey& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
