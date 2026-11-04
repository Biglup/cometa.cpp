/**
 * @file VkeyWitness.cpp
 *
 * @brief Verification-key transaction witness.
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

#include <Cometa/WitnessSet/VkeyWitness.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/ed25519_public_key.h>
#include <cardano/crypto/ed25519_signature.h>
#include <cardano/witness_set/vkey_witness.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VkeyWitness::VkeyWitness(const Ed25519PublicKey& publicKey, const Ed25519Signature& signature)
: m_value(nullptr)
{
  const Ed25519PublicKey                                            keyCopy(publicKey);
  const Ed25519Signature                                            signatureCopy(signature);
  cardano_vkey_witness_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_vkey_witness_new(checkedNativeHandle(keyCopy), checkedNativeHandle(signatureCopy), &value);
  CardanoObject<cardano_vkey_witness_t, cardano_vkey_witness_unref> owner(value);

  checkCardanoError(status, "VkeyWitness");
  m_value = owner.release();
}

VkeyWitness::VkeyWitness(const VkeyWitness& other)
: m_value(nullptr)
{
  cardano_vkey_witness_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_vkey_witness_deep_clone(other.checked(), &value);
  CardanoObject<cardano_vkey_witness_t, cardano_vkey_witness_unref> owner(value);

  checkCardanoError(status, "VkeyWitness copy");
  m_value = owner.release();
}

VkeyWitness::VkeyWitness(VkeyWitness&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

VkeyWitness::~VkeyWitness()
{
  cardano_vkey_witness_unref(&m_value);
}

VkeyWitness
VkeyWitness::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VkeyWitness(decodeCbor<cardano_vkey_witness_t, cardano_vkey_witness_unref>(cbor, cardano_vkey_witness_from_cbor));
}

VkeyWitness
VkeyWitness::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Ed25519PublicKey
VkeyWitness::publicKey() const
{
  cardano_ed25519_public_key_t*                                                 value = cardano_vkey_witness_get_vkey(checked());
  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("VkeyWitness has no verification key");
  }

  const Ed25519PublicKey found = Ed25519PublicKey::adopt(owner.release());
  return Ed25519PublicKey(found);
}

void
VkeyWitness::setPublicKey(const Ed25519PublicKey& value)
{
  const Ed25519PublicKey copy(value);
  checkCardanoError(cardano_vkey_witness_set_vkey(checked(), checkedNativeHandle(copy)), "VkeyWitness setPublicKey");
}

bool
VkeyWitness::hasPublicKey(const Ed25519PublicKey& value) const
{
  return cardano_vkey_witness_has_public_key(checked(), checkedNativeHandle(value));
}

Ed25519Signature
VkeyWitness::signature() const
{
  cardano_ed25519_signature_t*                                                value = cardano_vkey_witness_get_signature(checked());
  CardanoObject<cardano_ed25519_signature_t, cardano_ed25519_signature_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("VkeyWitness has no signature");
  }

  const Ed25519Signature found = Ed25519Signature::adopt(owner.release());
  return Ed25519Signature(found);
}

void
VkeyWitness::setSignature(const Ed25519Signature& value)
{
  const Ed25519Signature copy(value);
  checkCardanoError(cardano_vkey_witness_set_signature(checked(), checkedNativeHandle(copy)), "VkeyWitness setSignature");
}

std::vector<std::uint8_t>
VkeyWitness::toCbor() const
{
  return encodeCbor(checked(), cardano_vkey_witness_to_cbor);
}

std::string
VkeyWitness::toJson() const
{
  return encodeJson(checked(), cardano_vkey_witness_to_cip116_json);
}

void
VkeyWitness::swap(VkeyWitness& other) noexcept
{
  std::swap(m_value, other.m_value);
}

VkeyWitness&
VkeyWitness::operator=(const VkeyWitness& other)
{
  if (this != &other)
  {
    VkeyWitness copy(other);
    swap(copy);
  }
  return *this;
}

VkeyWitness&
VkeyWitness::operator=(VkeyWitness&& other) noexcept
{
  if (this != &other)
  {
    cardano_vkey_witness_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
VkeyWitness::operator==(const VkeyWitness& other) const
{
  return toCbor() == other.toCbor();
}

bool
VkeyWitness::operator!=(const VkeyWitness& other) const
{
  return !(*this == other);
}

VkeyWitness
VkeyWitness::adopt(cardano_vkey_witness_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null VkeyWitness");
  }

  return VkeyWitness(value);
}

cardano_vkey_witness_t*
VkeyWitness::nativeHandle() noexcept
{
  return m_value;
}

const cardano_vkey_witness_t*
VkeyWitness::nativeHandle() const noexcept
{
  return m_value;
}

cardano_vkey_witness_t*
VkeyWitness::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

VkeyWitness::VkeyWitness(cardano_vkey_witness_t* value) noexcept
: m_value(value)
{
}

cardano_vkey_witness_t*
VkeyWitness::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VkeyWitness");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VkeyWitness& lhs, VkeyWitness& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
