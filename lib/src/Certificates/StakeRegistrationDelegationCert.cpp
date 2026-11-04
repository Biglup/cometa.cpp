/**
 * @file StakeRegistrationDelegationCert.cpp
 *
 * @brief Stake registration and delegation certificates.
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

#include <Cometa/Certificates/StakeRegistrationDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/stake_registration_delegation_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
StakeRegistrationDelegationCert::StakeRegistrationDelegationCert(const Credential& credential, const Blake2bHash& poolKeyHash, std::uint64_t deposit)
: m_value(nullptr)
{
  if (poolKeyHash.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Credential                                                                                              copy(credential);
  const Blake2bHash                                                                                             poolCopy(poolKeyHash);
  cardano_stake_registration_delegation_cert_t*                                                                 value  = nullptr;
  const cardano_error_t                                                                                         status = cardano_stake_registration_delegation_cert_new(checkedNativeHandle(copy), checkedNativeHandle(poolCopy), deposit, &value);
  CardanoObject<cardano_stake_registration_delegation_cert_t, cardano_stake_registration_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeRegistrationDelegationCert");
  m_value = owner.release();
}

StakeRegistrationDelegationCert::StakeRegistrationDelegationCert(const StakeRegistrationDelegationCert& other)
: m_value(nullptr)
{
  cardano_stake_registration_delegation_cert_t*                                                                 value  = nullptr;
  const cardano_error_t                                                                                         status = cardano_stake_registration_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_stake_registration_delegation_cert_t, cardano_stake_registration_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeRegistrationDelegationCert copy");
  m_value = owner.release();
}

StakeRegistrationDelegationCert::StakeRegistrationDelegationCert(StakeRegistrationDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

StakeRegistrationDelegationCert::~StakeRegistrationDelegationCert()
{
  cardano_stake_registration_delegation_cert_unref(&m_value);
}

StakeRegistrationDelegationCert
StakeRegistrationDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return StakeRegistrationDelegationCert(decodeCbor<cardano_stake_registration_delegation_cert_t, cardano_stake_registration_delegation_cert_unref>(
    cbor, cardano_stake_registration_delegation_cert_from_cbor));
}

StakeRegistrationDelegationCert
StakeRegistrationDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
StakeRegistrationDelegationCert::credential() const
{
  cardano_credential_t* value = cardano_stake_registration_delegation_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Stake registration certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
StakeRegistrationDelegationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_stake_registration_delegation_cert_set_credential(checked(), checkedNativeHandle(copy)), "StakeRegistrationDelegationCert setCredential");
}

Blake2bHash
StakeRegistrationDelegationCert::poolKeyHash() const
{
  cardano_blake2b_hash_t* value = cardano_stake_registration_delegation_cert_get_pool_key_hash(checked());
  if (!value)
  {
    throw std::logic_error("Stake registration and delegation certificate has no pool key hash");
  }

  const Blake2bHash result = Blake2bHash::adopt(value);
  return Blake2bHash(result);
}

void
StakeRegistrationDelegationCert::setPoolKeyHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Blake2bHash copy(value);
  checkCardanoError(cardano_stake_registration_delegation_cert_set_pool_key_hash(checked(), checkedNativeHandle(copy)), "StakeRegistrationDelegationCert setPoolKeyHash");
}

std::uint64_t
StakeRegistrationDelegationCert::deposit() const
{
  return cardano_stake_registration_delegation_cert_get_deposit(checked());
}

void
StakeRegistrationDelegationCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_stake_registration_delegation_cert_set_deposit(checked(), value), "StakeRegistrationDelegationCert setDeposit");
}

std::vector<std::uint8_t>
StakeRegistrationDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_stake_registration_delegation_cert_to_cbor);
}

std::string
StakeRegistrationDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_stake_registration_delegation_cert_to_cip116_json);
}

void
StakeRegistrationDelegationCert::swap(StakeRegistrationDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

StakeRegistrationDelegationCert&
StakeRegistrationDelegationCert::operator=(const StakeRegistrationDelegationCert& other)
{
  if (this != &other)
  {
    StakeRegistrationDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

StakeRegistrationDelegationCert&
StakeRegistrationDelegationCert::operator=(StakeRegistrationDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_stake_registration_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
StakeRegistrationDelegationCert::operator==(const StakeRegistrationDelegationCert& other) const
{
  return credential() == other.credential() && poolKeyHash() == other.poolKeyHash() && deposit() == other.deposit();
}

bool
StakeRegistrationDelegationCert::operator!=(const StakeRegistrationDelegationCert& other) const
{
  return !(*this == other);
}

StakeRegistrationDelegationCert
StakeRegistrationDelegationCert::adopt(cardano_stake_registration_delegation_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null StakeRegistrationDelegationCert");
  }

  return StakeRegistrationDelegationCert(value);
}

cardano_stake_registration_delegation_cert_t*
StakeRegistrationDelegationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_stake_registration_delegation_cert_t*
StakeRegistrationDelegationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_stake_registration_delegation_cert_t*
StakeRegistrationDelegationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

StakeRegistrationDelegationCert::StakeRegistrationDelegationCert(cardano_stake_registration_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_stake_registration_delegation_cert_t*
StakeRegistrationDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from StakeRegistrationDelegationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(StakeRegistrationDelegationCert& lhs, StakeRegistrationDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
