/**
 * @file StakeVoteDelegationCert.cpp
 *
 * @brief Stake and vote delegation certificates.
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

#include <Cometa/Certificates/StakeVoteDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/stake_vote_delegation_cert.h>
#include <cardano/common/drep.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
StakeVoteDelegationCert::StakeVoteDelegationCert(const Credential& credential, const Blake2bHash& poolKeyHash, const DRep& drep)
: m_value(nullptr)
{
  if (poolKeyHash.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Credential                                                                              copy(credential);
  const Blake2bHash                                                                             poolCopy(poolKeyHash);
  const DRep                                                                                    drepCopy(drep);
  cardano_stake_vote_delegation_cert_t*                                                         value  = nullptr;
  const cardano_error_t                                                                         status = cardano_stake_vote_delegation_cert_new(copy.checked(), poolCopy.checked(), drepCopy.checked(), &value);
  CardanoObject<cardano_stake_vote_delegation_cert_t, cardano_stake_vote_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeVoteDelegationCert");
  m_value = owner.release();
}

StakeVoteDelegationCert::StakeVoteDelegationCert(const StakeVoteDelegationCert& other)
: m_value(nullptr)
{
  cardano_stake_vote_delegation_cert_t*                                                         value  = nullptr;
  const cardano_error_t                                                                         status = cardano_stake_vote_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_stake_vote_delegation_cert_t, cardano_stake_vote_delegation_cert_unref> owner(value);

  checkCardanoError(status, "StakeVoteDelegationCert copy");
  m_value = owner.release();
}

StakeVoteDelegationCert::StakeVoteDelegationCert(StakeVoteDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

StakeVoteDelegationCert::~StakeVoteDelegationCert()
{
  cardano_stake_vote_delegation_cert_unref(&m_value);
}

StakeVoteDelegationCert
StakeVoteDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return StakeVoteDelegationCert(decodeCbor<cardano_stake_vote_delegation_cert_t, cardano_stake_vote_delegation_cert_unref>(
    cbor, cardano_stake_vote_delegation_cert_from_cbor));
}

StakeVoteDelegationCert
StakeVoteDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
StakeVoteDelegationCert::credential() const
{
  cardano_credential_t* value = cardano_stake_vote_delegation_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Stake and vote delegation certificate has no credential");
  }

  const Credential result(value);
  return Credential(result);
}

void
StakeVoteDelegationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_stake_vote_delegation_cert_set_credential(checked(), copy.checked()), "StakeVoteDelegationCert setCredential");
}

Blake2bHash
StakeVoteDelegationCert::poolKeyHash() const
{
  cardano_blake2b_hash_t* value = cardano_stake_vote_delegation_cert_get_pool_key_hash(checked());
  if (!value)
  {
    throw std::logic_error("Stake and vote delegation certificate has no pool key hash");
  }

  const Blake2bHash result(value);
  return Blake2bHash(result);
}

void
StakeVoteDelegationCert::setPoolKeyHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool key hash must be 28 bytes");
  }

  const Blake2bHash copy(value);
  checkCardanoError(cardano_stake_vote_delegation_cert_set_pool_key_hash(checked(), copy.checked()), "StakeVoteDelegationCert setPoolKeyHash");
}

DRep
StakeVoteDelegationCert::drep() const
{
  cardano_drep_t* value = cardano_stake_vote_delegation_cert_get_drep(checked());
  if (!value)
  {
    throw std::logic_error("Stake and vote delegation certificate has no DRep");
  }

  const DRep result(value);
  return DRep(result);
}

void
StakeVoteDelegationCert::setDRep(const DRep& value)
{
  const DRep copy(value);
  checkCardanoError(cardano_stake_vote_delegation_cert_set_drep(checked(), copy.checked()), "StakeVoteDelegationCert setDRep");
}

std::vector<std::uint8_t>
StakeVoteDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_stake_vote_delegation_cert_to_cbor);
}

std::string
StakeVoteDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_stake_vote_delegation_cert_to_cip116_json);
}

void
StakeVoteDelegationCert::swap(StakeVoteDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

StakeVoteDelegationCert&
StakeVoteDelegationCert::operator=(const StakeVoteDelegationCert& other)
{
  if (this != &other)
  {
    StakeVoteDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

StakeVoteDelegationCert&
StakeVoteDelegationCert::operator=(StakeVoteDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_stake_vote_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
StakeVoteDelegationCert::operator==(const StakeVoteDelegationCert& other) const
{
  return credential() == other.credential() && poolKeyHash() == other.poolKeyHash() && drep() == other.drep();
}

bool
StakeVoteDelegationCert::operator!=(const StakeVoteDelegationCert& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

StakeVoteDelegationCert::StakeVoteDelegationCert(cardano_stake_vote_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_stake_vote_delegation_cert_t*
StakeVoteDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from StakeVoteDelegationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(StakeVoteDelegationCert& lhs, StakeVoteDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
