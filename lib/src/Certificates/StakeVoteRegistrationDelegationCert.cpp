/**
 * @file StakeVoteRegistrationDelegationCert.cpp
 *
 * @brief Stake and vote registration delegation certificates.
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

#include <Cometa/Certificates/StakeVoteRegistrationDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/stake_vote_registration_delegation_cert.h>
#include <cardano/common/drep.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
StakeVoteRegistrationDelegationCert::StakeVoteRegistrationDelegationCert(const Credential& credential, const Blake2bHash& poolKeyHash, const DRep& drep, std::uint64_t deposit)
: m_value(nullptr)
{
  if (poolKeyHash.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Credential                                                                                                        copy(credential);
  const Blake2bHash                                                                                                       poolCopy(poolKeyHash);
  const DRep                                                                                                              drepCopy(drep);
  cardano_stake_vote_registration_delegation_cert_t*                                                                      value  = nullptr;
  const cardano_error_t                                                                                                   status = cardano_stake_vote_registration_delegation_cert_new(checkedNativeHandle(copy), deposit, checkedNativeHandle(drepCopy), checkedNativeHandle(poolCopy), &value);
  CardanoObject<cardano_stake_vote_registration_delegation_cert_t, cardano_stake_vote_registration_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeVoteRegistrationDelegationCert");
  m_value = owner.release();
}

StakeVoteRegistrationDelegationCert::StakeVoteRegistrationDelegationCert(const StakeVoteRegistrationDelegationCert& other)
: m_value(nullptr)
{
  cardano_stake_vote_registration_delegation_cert_t*                                                                      value  = nullptr;
  const cardano_error_t                                                                                                   status = cardano_stake_vote_registration_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_stake_vote_registration_delegation_cert_t, cardano_stake_vote_registration_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeVoteRegistrationDelegationCert copy");
  m_value = owner.release();
}

StakeVoteRegistrationDelegationCert::StakeVoteRegistrationDelegationCert(StakeVoteRegistrationDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

StakeVoteRegistrationDelegationCert::~StakeVoteRegistrationDelegationCert()
{
  cardano_stake_vote_registration_delegation_cert_unref(&m_value);
}

StakeVoteRegistrationDelegationCert
StakeVoteRegistrationDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return StakeVoteRegistrationDelegationCert(decodeCbor<cardano_stake_vote_registration_delegation_cert_t, cardano_stake_vote_registration_delegation_cert_unref>(
    cbor, cardano_stake_vote_registration_delegation_cert_from_cbor));
}

StakeVoteRegistrationDelegationCert
StakeVoteRegistrationDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
StakeVoteRegistrationDelegationCert::credential() const
{
  cardano_credential_t* value = cardano_stake_vote_registration_delegation_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Stake and vote delegation certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
StakeVoteRegistrationDelegationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_stake_vote_registration_delegation_cert_set_credential(checked(), checkedNativeHandle(copy)), "StakeVoteRegistrationDelegationCert setCredential");
}

Blake2bHash
StakeVoteRegistrationDelegationCert::poolKeyHash() const
{
  cardano_blake2b_hash_t* value = cardano_stake_vote_registration_delegation_cert_get_pool_key_hash(checked());
  if (!value)
  {
    throw std::logic_error("Stake and vote delegation certificate has no pool key hash");
  }

  const Blake2bHash result = Blake2bHash::adopt(value);
  return Blake2bHash(result);
}

void
StakeVoteRegistrationDelegationCert::setPoolKeyHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Blake2bHash copy(value);
  checkCardanoError(cardano_stake_vote_registration_delegation_cert_set_pool_key_hash(checked(), checkedNativeHandle(copy)), "StakeVoteRegistrationDelegationCert setPoolKeyHash");
}

DRep
StakeVoteRegistrationDelegationCert::drep() const
{
  cardano_drep_t* value = cardano_stake_vote_registration_delegation_cert_get_drep(checked());
  if (!value)
  {
    throw std::logic_error("Stake and vote delegation certificate has no DRep");
  }

  const DRep result = DRep::adopt(value);
  return DRep(result);
}

void
StakeVoteRegistrationDelegationCert::setDRep(const DRep& value)
{
  const DRep copy(value);
  checkCardanoError(cardano_stake_vote_registration_delegation_cert_set_drep(checked(), checkedNativeHandle(copy)), "StakeVoteRegistrationDelegationCert setDRep");
}

std::uint64_t
StakeVoteRegistrationDelegationCert::deposit() const
{
  return cardano_stake_vote_registration_delegation_cert_get_deposit(checked());
}

void
StakeVoteRegistrationDelegationCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_stake_vote_registration_delegation_cert_set_deposit(checked(), value), "StakeVoteRegistrationDelegationCert setDeposit");
}

std::vector<std::uint8_t>
StakeVoteRegistrationDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_stake_vote_registration_delegation_cert_to_cbor);
}

std::string
StakeVoteRegistrationDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_stake_vote_registration_delegation_cert_to_cip116_json);
}

void
StakeVoteRegistrationDelegationCert::swap(StakeVoteRegistrationDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

StakeVoteRegistrationDelegationCert&
StakeVoteRegistrationDelegationCert::operator=(const StakeVoteRegistrationDelegationCert& other)
{
  if (this != &other)
  {
    StakeVoteRegistrationDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

StakeVoteRegistrationDelegationCert&
StakeVoteRegistrationDelegationCert::operator=(StakeVoteRegistrationDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_stake_vote_registration_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
StakeVoteRegistrationDelegationCert::operator==(const StakeVoteRegistrationDelegationCert& other) const
{
  return credential() == other.credential() && poolKeyHash() == other.poolKeyHash() && drep() == other.drep() && deposit() == other.deposit();
}

bool
StakeVoteRegistrationDelegationCert::operator!=(const StakeVoteRegistrationDelegationCert& other) const
{
  return !(*this == other);
}

StakeVoteRegistrationDelegationCert
StakeVoteRegistrationDelegationCert::adopt(cardano_stake_vote_registration_delegation_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null StakeVoteRegistrationDelegationCert");
  }

  return StakeVoteRegistrationDelegationCert(value);
}

cardano_stake_vote_registration_delegation_cert_t*
StakeVoteRegistrationDelegationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_stake_vote_registration_delegation_cert_t*
StakeVoteRegistrationDelegationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_stake_vote_registration_delegation_cert_t*
StakeVoteRegistrationDelegationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

StakeVoteRegistrationDelegationCert::StakeVoteRegistrationDelegationCert(cardano_stake_vote_registration_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_stake_vote_registration_delegation_cert_t*
StakeVoteRegistrationDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from StakeVoteRegistrationDelegationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(StakeVoteRegistrationDelegationCert& lhs, StakeVoteRegistrationDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
