/**
 * @file BootstrapWitness.cpp
 *
 * @brief Bootstrap transaction witness.
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

#include <Cometa/WitnessSet/BootstrapWitness.h>

#include "../CardanoInterop.h"

#include <cardano/buffer.h>
#include <cardano/crypto/ed25519_public_key.h>
#include <cardano/crypto/ed25519_signature.h>
#include <cardano/witness_set/bootstrap_witness.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
BootstrapWitness::BootstrapWitness(const Ed25519PublicKey& publicKey, const Ed25519Signature& signature, const std::vector<std::uint8_t>& chainCode, const std::vector<std::uint8_t>& attributes)
: m_value(nullptr)
{
  const Ed25519PublicKey keyCopy(publicKey);
  const Ed25519Signature signatureCopy(signature);
  if (chainCode.size() != 32U)
  {
    throw std::invalid_argument("BootstrapWitness chain code must be 32 bytes");
  }

  cardano_buffer_t*                                     chain = cardano_buffer_new_from(byteData(chainCode), chainCode.size());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> chainOwner(chain);
  if (!chain)
  {
    throw std::bad_alloc();
  }

  cardano_buffer_t*                                     attrs = cardano_buffer_new(attributes.empty() ? 1U : attributes.size());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> attrsOwner(attrs);
  if (!attrs)
  {
    throw std::bad_alloc();
  }

  if (!attributes.empty())
  {
    checkCardanoError(cardano_buffer_write(attrs, attributes.data(), attributes.size()), "BootstrapWitness attribute bytes");
  }

  cardano_bootstrap_witness_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_bootstrap_witness_new(keyCopy.checked(), signatureCopy.checked(), chain, attrs, &value);
  CardanoObject<cardano_bootstrap_witness_t, cardano_bootstrap_witness_unref> owner(value);

  checkCardanoError(status, "BootstrapWitness");
  m_value = owner.release();
}

BootstrapWitness::BootstrapWitness(const BootstrapWitness& other)
: m_value(nullptr)
{
  cardano_bootstrap_witness_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_bootstrap_witness_deep_clone(other.checked(), &value);
  CardanoObject<cardano_bootstrap_witness_t, cardano_bootstrap_witness_unref> owner(value);

  checkCardanoError(status, "BootstrapWitness copy");
  m_value = owner.release();
}

BootstrapWitness::BootstrapWitness(BootstrapWitness&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

BootstrapWitness::~BootstrapWitness()
{
  cardano_bootstrap_witness_unref(&m_value);
}

BootstrapWitness
BootstrapWitness::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return BootstrapWitness(decodeCbor<cardano_bootstrap_witness_t, cardano_bootstrap_witness_unref>(cbor, cardano_bootstrap_witness_from_cbor));
}

BootstrapWitness
BootstrapWitness::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Ed25519PublicKey
BootstrapWitness::publicKey() const
{
  cardano_ed25519_public_key_t*                                                 value = cardano_bootstrap_witness_get_vkey(checked());
  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("BootstrapWitness has no verification key");
  }

  const Ed25519PublicKey found(*owner.release());
  return Ed25519PublicKey(found);
}

void
BootstrapWitness::setPublicKey(const Ed25519PublicKey& value)
{
  const Ed25519PublicKey copy(value);
  checkCardanoError(cardano_bootstrap_witness_set_vkey(checked(), copy.checked()), "BootstrapWitness setPublicKey");
}

Ed25519Signature
BootstrapWitness::signature() const
{
  cardano_ed25519_signature_t*                                                value = cardano_bootstrap_witness_get_signature(checked());
  CardanoObject<cardano_ed25519_signature_t, cardano_ed25519_signature_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("BootstrapWitness has no signature");
  }

  const Ed25519Signature found(*owner.release());
  return Ed25519Signature(found);
}

void
BootstrapWitness::setSignature(const Ed25519Signature& value)
{
  const Ed25519Signature copy(value);
  checkCardanoError(cardano_bootstrap_witness_set_signature(checked(), copy.checked()), "BootstrapWitness setSignature");
}

std::vector<std::uint8_t>
BootstrapWitness::chainCode() const
{
  cardano_buffer_t*                                     value = cardano_bootstrap_witness_get_chain_code(checked());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);
  if (!value)
  {
    throw std::logic_error("BootstrapWitness has no chain code");
  }
  const std::uint8_t* bytes = cardano_buffer_get_data(value);
  return std::vector<std::uint8_t>(bytes, bytes + cardano_buffer_get_size(value));
}

void
BootstrapWitness::setChainCode(const std::vector<std::uint8_t>& value)
{
  if (value.size() != 32U)
  {
    throw std::invalid_argument("BootstrapWitness chain code must be 32 bytes");
  }

  cardano_buffer_t*                                     buffer = cardano_buffer_new(value.size());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(buffer);
  if (!buffer)
  {
    throw std::bad_alloc();
  }
  checkCardanoError(cardano_buffer_write(buffer, value.data(), value.size()), "BootstrapWitness chain code bytes");
  checkCardanoError(cardano_bootstrap_witness_set_chain_code(checked(), buffer), "BootstrapWitness setChainCode");
}

std::vector<std::uint8_t>
BootstrapWitness::attributes() const
{
  cardano_buffer_t*                                     value = cardano_bootstrap_witness_get_attributes(checked());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);
  if (!value)
  {
    throw std::logic_error("BootstrapWitness has no attributes");
  }
  const std::uint8_t* bytes = cardano_buffer_get_data(value);
  return std::vector<std::uint8_t>(bytes, bytes + cardano_buffer_get_size(value));
}

void
BootstrapWitness::setAttributes(const std::vector<std::uint8_t>& value)
{
  cardano_buffer_t*                                     buffer = cardano_buffer_new(value.empty() ? 1U : value.size());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(buffer);
  if (!buffer)
  {
    throw std::bad_alloc();
  }
  if (!value.empty())
  {
    checkCardanoError(cardano_buffer_write(buffer, value.data(), value.size()), "BootstrapWitness attribute bytes");
  }
  checkCardanoError(cardano_bootstrap_witness_set_attributes(checked(), buffer), "BootstrapWitness setAttributes");
}

std::vector<std::uint8_t>
BootstrapWitness::toCbor() const
{
  return encodeCbor(checked(), cardano_bootstrap_witness_to_cbor);
}

std::string
BootstrapWitness::toJson() const
{
  return encodeJson(checked(), cardano_bootstrap_witness_to_cip116_json);
}

void
BootstrapWitness::swap(BootstrapWitness& other) noexcept
{
  std::swap(m_value, other.m_value);
}

BootstrapWitness&
BootstrapWitness::operator=(const BootstrapWitness& other)
{
  if (this != &other)
  {
    BootstrapWitness copy(other);
    swap(copy);
  }
  return *this;
}

BootstrapWitness&
BootstrapWitness::operator=(BootstrapWitness&& other) noexcept
{
  if (this != &other)
  {
    cardano_bootstrap_witness_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
BootstrapWitness::operator==(const BootstrapWitness& other) const
{
  return toCbor() == other.toCbor();
}

bool
BootstrapWitness::operator!=(const BootstrapWitness& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

BootstrapWitness::BootstrapWitness(cardano_bootstrap_witness_t* value) noexcept
: m_value(value)
{
}

cardano_bootstrap_witness_t*
BootstrapWitness::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from BootstrapWitness");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(BootstrapWitness& lhs, BootstrapWitness& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
