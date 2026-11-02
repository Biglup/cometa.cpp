/**
 * @file GenesisKeyDelegationCert.cpp
 *
 * @brief Historical genesis key delegation certificates.
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

#include <Cometa/Certificates/GenesisKeyDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/genesis_key_delegation_cert.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
GenesisKeyDelegationCert::GenesisKeyDelegationCert(const Blake2bHash& genesisHash, const Blake2bHash& delegateHash, const Blake2bHash& vrfKeyHash)
: m_value(nullptr)
{
  validateHash(genesisHash, 28);
  validateHash(delegateHash, 28);
  validateHash(vrfKeyHash, 32);

  const Blake2bHash                                                                               genesisCopy(genesisHash);
  const Blake2bHash                                                                               delegateCopy(delegateHash);
  const Blake2bHash                                                                               vrfCopy(vrfKeyHash);
  cardano_genesis_key_delegation_cert_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_genesis_key_delegation_cert_new(genesisCopy.checked(), delegateCopy.checked(), vrfCopy.checked(), &value);
  CardanoObject<cardano_genesis_key_delegation_cert_t, cardano_genesis_key_delegation_cert_unref> owner(value);

  checkCardanoError(status, "GenesisKeyDelegationCert");
  m_value = owner.release();
}

GenesisKeyDelegationCert::GenesisKeyDelegationCert(const GenesisKeyDelegationCert& other)
: m_value(nullptr)
{
  cardano_genesis_key_delegation_cert_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_genesis_key_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_genesis_key_delegation_cert_t, cardano_genesis_key_delegation_cert_unref> owner(value);

  checkCardanoError(status, "GenesisKeyDelegationCert copy");
  m_value = owner.release();
}

GenesisKeyDelegationCert::GenesisKeyDelegationCert(GenesisKeyDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

GenesisKeyDelegationCert::~GenesisKeyDelegationCert()
{
  cardano_genesis_key_delegation_cert_unref(&m_value);
}

GenesisKeyDelegationCert
GenesisKeyDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  GenesisKeyDelegationCert result(decodeCbor<cardano_genesis_key_delegation_cert_t, cardano_genesis_key_delegation_cert_unref>(
    cbor, cardano_genesis_key_delegation_cert_from_cbor));

  validateHash(result.genesisHash(), 28);
  validateHash(result.delegateHash(), 28);
  validateHash(result.vrfKeyHash(), 32);

  return result;
}

GenesisKeyDelegationCert
GenesisKeyDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Blake2bHash
GenesisKeyDelegationCert::genesisHash() const
{
  cardano_blake2b_hash_t* value = cardano_genesis_key_delegation_cert_get_genesis_hash(checked());
  if (!value)
  {
    throw std::logic_error("Genesis key delegation certificate has no genesis key hash");
  }

  const Blake2bHash result(value);
  return Blake2bHash(result);
}

void
GenesisKeyDelegationCert::setGenesisHash(const Blake2bHash& value)
{
  validateHash(value, 28);
  const Blake2bHash copy(value);
  checkCardanoError(cardano_genesis_key_delegation_cert_set_genesis_hash(checked(), copy.checked()), "GenesisKeyDelegationCert setGenesisHash");
}

Blake2bHash
GenesisKeyDelegationCert::delegateHash() const
{
  cardano_blake2b_hash_t* value = cardano_genesis_key_delegation_cert_get_genesis_delegate_hash(checked());
  if (!value)
  {
    throw std::logic_error("Genesis key delegation certificate has no delegate key hash");
  }

  const Blake2bHash result(value);
  return Blake2bHash(result);
}

void
GenesisKeyDelegationCert::setDelegateHash(const Blake2bHash& value)
{
  validateHash(value, 28);
  const Blake2bHash copy(value);
  checkCardanoError(cardano_genesis_key_delegation_cert_set_genesis_delegate_hash(checked(), copy.checked()), "GenesisKeyDelegationCert setDelegateHash");
}

Blake2bHash
GenesisKeyDelegationCert::vrfKeyHash() const
{
  cardano_blake2b_hash_t* value = cardano_genesis_key_delegation_cert_get_vrf_key_hash(checked());
  if (!value)
  {
    throw std::logic_error("Genesis key delegation certificate has no VRF key hash");
  }

  const Blake2bHash result(value);
  return Blake2bHash(result);
}

void
GenesisKeyDelegationCert::setVrfKeyHash(const Blake2bHash& value)
{
  validateHash(value, 32);
  const Blake2bHash copy(value);
  checkCardanoError(cardano_genesis_key_delegation_cert_set_vrf_key_hash(checked(), copy.checked()), "GenesisKeyDelegationCert setVrfKeyHash");
}

std::vector<std::uint8_t>
GenesisKeyDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_genesis_key_delegation_cert_to_cbor);
}

std::string
GenesisKeyDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_genesis_key_delegation_cert_to_cip116_json);
}

void
GenesisKeyDelegationCert::swap(GenesisKeyDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

GenesisKeyDelegationCert&
GenesisKeyDelegationCert::operator=(const GenesisKeyDelegationCert& other)
{
  if (this != &other)
  {
    GenesisKeyDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

GenesisKeyDelegationCert&
GenesisKeyDelegationCert::operator=(GenesisKeyDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_genesis_key_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
GenesisKeyDelegationCert::operator==(const GenesisKeyDelegationCert& other) const
{
  return genesisHash() == other.genesisHash() && delegateHash() == other.delegateHash() && vrfKeyHash() == other.vrfKeyHash();
}

bool
GenesisKeyDelegationCert::operator!=(const GenesisKeyDelegationCert& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

GenesisKeyDelegationCert::GenesisKeyDelegationCert(cardano_genesis_key_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_genesis_key_delegation_cert_t*
GenesisKeyDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from GenesisKeyDelegationCert");
  }

  return m_value;
}

void
GenesisKeyDelegationCert::validateHash(const Blake2bHash& value, std::size_t size)
{
  if (value.toBytes().size() != size)
  {
    throw std::invalid_argument("Invalid genesis key delegation hash length");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(GenesisKeyDelegationCert& lhs, GenesisKeyDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
