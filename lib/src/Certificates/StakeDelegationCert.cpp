/**
 * @file StakeDelegationCert.cpp
 *
 * @brief Stake delegation certificates.
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

#include <Cometa/Certificates/StakeDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/stake_delegation_cert.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
StakeDelegationCert::StakeDelegationCert(const Credential& credential, const Blake2bHash& poolKeyHash)
: m_value(nullptr)
{
  if (poolKeyHash.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Credential                                                                    copy(credential);
  const Blake2bHash                                                                   poolCopy(poolKeyHash);
  cardano_stake_delegation_cert_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_stake_delegation_cert_new(copy.checked(), poolCopy.checked(), &value);
  CardanoObject<cardano_stake_delegation_cert_t, cardano_stake_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeDelegationCert");
  m_value = owner.release();
}

StakeDelegationCert::StakeDelegationCert(const StakeDelegationCert& other)
: m_value(nullptr)
{
  cardano_stake_delegation_cert_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_stake_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_stake_delegation_cert_t, cardano_stake_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeDelegationCert copy");
  m_value = owner.release();
}

StakeDelegationCert::StakeDelegationCert(StakeDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

StakeDelegationCert::~StakeDelegationCert()
{
  cardano_stake_delegation_cert_unref(&m_value);
}

StakeDelegationCert
StakeDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return StakeDelegationCert(decodeCbor<cardano_stake_delegation_cert_t, cardano_stake_delegation_cert_unref>(
    cbor, cardano_stake_delegation_cert_from_cbor));
}

StakeDelegationCert
StakeDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
StakeDelegationCert::credential() const
{
  cardano_credential_t* value = cardano_stake_delegation_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Stake registration certificate has no credential");
  }

  const Credential result(value);
  return Credential(result);
}

void
StakeDelegationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_stake_delegation_cert_set_credential(checked(), copy.checked()), "StakeDelegationCert setCredential");
}

Blake2bHash
StakeDelegationCert::poolKeyHash() const
{
  cardano_blake2b_hash_t* value = cardano_stake_delegation_cert_get_pool_key_hash(checked());
  if (!value)
  {
    throw std::logic_error("Stake delegation certificate has no pool key hash");
  }

  const Blake2bHash result(value);
  return Blake2bHash(result);
}

void
StakeDelegationCert::setPoolKeyHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Blake2bHash copy(value);
  checkCardanoError(cardano_stake_delegation_cert_set_pool_key_hash(checked(), copy.checked()), "StakeDelegationCert setPoolKeyHash");
}

std::vector<std::uint8_t>
StakeDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_stake_delegation_cert_to_cbor);
}

std::string
StakeDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_stake_delegation_cert_to_cip116_json);
}

void
StakeDelegationCert::swap(StakeDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

StakeDelegationCert&
StakeDelegationCert::operator=(const StakeDelegationCert& other)
{
  if (this != &other)
  {
    StakeDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

StakeDelegationCert&
StakeDelegationCert::operator=(StakeDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_stake_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
StakeDelegationCert::operator==(const StakeDelegationCert& other) const
{
  return credential() == other.credential() && poolKeyHash() == other.poolKeyHash();
}

bool
StakeDelegationCert::operator!=(const StakeDelegationCert& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

StakeDelegationCert::StakeDelegationCert(cardano_stake_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_stake_delegation_cert_t*
StakeDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from StakeDelegationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(StakeDelegationCert& lhs, StakeDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
