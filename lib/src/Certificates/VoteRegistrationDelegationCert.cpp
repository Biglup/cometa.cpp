/**
 * @file VoteRegistrationDelegationCert.cpp
 *
 * @brief Vote registration and delegation certificates.
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

#include <Cometa/Certificates/VoteRegistrationDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/vote_registration_delegation_cert.h>
#include <cardano/common/drep.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VoteRegistrationDelegationCert::VoteRegistrationDelegationCert(const Credential& credential, std::uint64_t deposit, const DRep& drep)
: m_value(nullptr)
{
  const Credential                                                                                            copy(credential);
  const DRep                                                                                                  drepCopy(drep);
  cardano_vote_registration_delegation_cert_t*                                                                value  = nullptr;
  const cardano_error_t                                                                                       status = cardano_vote_registration_delegation_cert_new(checkedNativeHandle(copy), deposit, checkedNativeHandle(drepCopy), &value);
  CardanoObject<cardano_vote_registration_delegation_cert_t, cardano_vote_registration_delegation_cert_unref> owner(value);

  checkCardanoError(status, "VoteRegistrationDelegationCert");
  m_value = owner.release();
}

VoteRegistrationDelegationCert::VoteRegistrationDelegationCert(const VoteRegistrationDelegationCert& other)
: m_value(nullptr)
{
  cardano_vote_registration_delegation_cert_t*                                                                value  = nullptr;
  const cardano_error_t                                                                                       status = cardano_vote_registration_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_vote_registration_delegation_cert_t, cardano_vote_registration_delegation_cert_unref> owner(value);

  checkCardanoError(status, "VoteRegistrationDelegationCert copy");
  m_value = owner.release();
}

VoteRegistrationDelegationCert::VoteRegistrationDelegationCert(VoteRegistrationDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

VoteRegistrationDelegationCert::~VoteRegistrationDelegationCert()
{
  cardano_vote_registration_delegation_cert_unref(&m_value);
}

VoteRegistrationDelegationCert
VoteRegistrationDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VoteRegistrationDelegationCert(decodeCbor<cardano_vote_registration_delegation_cert_t, cardano_vote_registration_delegation_cert_unref>(
    cbor, cardano_vote_registration_delegation_cert_from_cbor));
}

VoteRegistrationDelegationCert
VoteRegistrationDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
VoteRegistrationDelegationCert::credential() const
{
  cardano_credential_t* value = cardano_vote_registration_delegation_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Vote delegation certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
VoteRegistrationDelegationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_vote_registration_delegation_cert_set_credential(checked(), checkedNativeHandle(copy)), "VoteRegistrationDelegationCert setCredential");
}

DRep
VoteRegistrationDelegationCert::drep() const
{
  cardano_drep_t* value = cardano_vote_registration_delegation_cert_get_drep(checked());
  if (!value)
  {
    throw std::logic_error("Vote delegation certificate has no DRep");
  }

  const DRep result = DRep::adopt(value);
  return DRep(result);
}

void
VoteRegistrationDelegationCert::setDRep(const DRep& value)
{
  const DRep copy(value);
  checkCardanoError(cardano_vote_registration_delegation_cert_set_drep(checked(), checkedNativeHandle(copy)), "VoteRegistrationDelegationCert setDRep");
}

std::uint64_t
VoteRegistrationDelegationCert::deposit() const
{
  return cardano_vote_registration_delegation_cert_get_deposit(checked());
}

void
VoteRegistrationDelegationCert::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_vote_registration_delegation_cert_set_deposit(checked(), value), "VoteRegistrationDelegationCert setDeposit");
}

std::vector<std::uint8_t>
VoteRegistrationDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_vote_registration_delegation_cert_to_cbor);
}

std::string
VoteRegistrationDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_vote_registration_delegation_cert_to_cip116_json);
}

void
VoteRegistrationDelegationCert::swap(VoteRegistrationDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

VoteRegistrationDelegationCert&
VoteRegistrationDelegationCert::operator=(const VoteRegistrationDelegationCert& other)
{
  if (this != &other)
  {
    VoteRegistrationDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

VoteRegistrationDelegationCert&
VoteRegistrationDelegationCert::operator=(VoteRegistrationDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_vote_registration_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
VoteRegistrationDelegationCert::operator==(const VoteRegistrationDelegationCert& other) const
{
  return credential() == other.credential() && drep() == other.drep() && deposit() == other.deposit();
}

bool
VoteRegistrationDelegationCert::operator!=(const VoteRegistrationDelegationCert& other) const
{
  return !(*this == other);
}

VoteRegistrationDelegationCert
VoteRegistrationDelegationCert::adopt(cardano_vote_registration_delegation_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null VoteRegistrationDelegationCert");
  }

  return VoteRegistrationDelegationCert(value);
}

cardano_vote_registration_delegation_cert_t*
VoteRegistrationDelegationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_vote_registration_delegation_cert_t*
VoteRegistrationDelegationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_vote_registration_delegation_cert_t*
VoteRegistrationDelegationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

VoteRegistrationDelegationCert::VoteRegistrationDelegationCert(cardano_vote_registration_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_vote_registration_delegation_cert_t*
VoteRegistrationDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VoteRegistrationDelegationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VoteRegistrationDelegationCert& lhs, VoteRegistrationDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
