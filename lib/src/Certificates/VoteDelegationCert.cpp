/**
 * @file VoteDelegationCert.cpp
 *
 * @brief Vote delegation certificates.
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

#include <Cometa/Certificates/VoteDelegationCert.h>

#include "../CardanoInterop.h"

#include <cardano/certs/vote_delegation_cert.h>
#include <cardano/common/drep.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VoteDelegationCert::VoteDelegationCert(const Credential& credential, const DRep& drep)
: m_value(nullptr)
{
  const Credential                                                                  copy(credential);
  const DRep                                                                        drepCopy(drep);
  cardano_vote_delegation_cert_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_vote_delegation_cert_new(checkedNativeHandle(copy), checkedNativeHandle(drepCopy), &value);
  CardanoObject<cardano_vote_delegation_cert_t, cardano_vote_delegation_cert_unref> owner(value);

  checkCardanoError(status, "VoteDelegationCert");
  m_value = owner.release();
}

VoteDelegationCert::VoteDelegationCert(const VoteDelegationCert& other)
: m_value(nullptr)
{
  cardano_vote_delegation_cert_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_vote_delegation_cert_deep_clone(other.checked(), &value);
  CardanoObject<cardano_vote_delegation_cert_t, cardano_vote_delegation_cert_unref> owner(value);

  checkCardanoError(status, "VoteDelegationCert copy");
  m_value = owner.release();
}

VoteDelegationCert::VoteDelegationCert(VoteDelegationCert&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

VoteDelegationCert::~VoteDelegationCert()
{
  cardano_vote_delegation_cert_unref(&m_value);
}

VoteDelegationCert
VoteDelegationCert::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VoteDelegationCert(decodeCbor<cardano_vote_delegation_cert_t, cardano_vote_delegation_cert_unref>(
    cbor, cardano_vote_delegation_cert_from_cbor));
}

VoteDelegationCert
VoteDelegationCert::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Credential
VoteDelegationCert::credential() const
{
  cardano_credential_t* value = cardano_vote_delegation_cert_get_credential(checked());
  if (!value)
  {
    throw std::logic_error("Vote delegation certificate has no credential");
  }

  const Credential result = Credential::adopt(value);
  return Credential(result);
}

void
VoteDelegationCert::setCredential(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_vote_delegation_cert_set_credential(checked(), checkedNativeHandle(copy)), "VoteDelegationCert setCredential");
}

DRep
VoteDelegationCert::drep() const
{
  cardano_drep_t* value = cardano_vote_delegation_cert_get_drep(checked());
  if (!value)
  {
    throw std::logic_error("Vote delegation certificate has no DRep");
  }

  const DRep result = DRep::adopt(value);
  return DRep(result);
}

void
VoteDelegationCert::setDRep(const DRep& value)
{
  const DRep copy(value);
  checkCardanoError(cardano_vote_delegation_cert_set_drep(checked(), checkedNativeHandle(copy)), "VoteDelegationCert setDRep");
}

std::vector<std::uint8_t>
VoteDelegationCert::toCbor() const
{
  return encodeCbor(checked(), cardano_vote_delegation_cert_to_cbor);
}

std::string
VoteDelegationCert::toJson() const
{
  return encodeJson(checked(), cardano_vote_delegation_cert_to_cip116_json);
}

void
VoteDelegationCert::swap(VoteDelegationCert& other) noexcept
{
  std::swap(m_value, other.m_value);
}

VoteDelegationCert&
VoteDelegationCert::operator=(const VoteDelegationCert& other)
{
  if (this != &other)
  {
    VoteDelegationCert copy(other);
    swap(copy);
  }

  return *this;
}

VoteDelegationCert&
VoteDelegationCert::operator=(VoteDelegationCert&& other) noexcept
{
  if (this != &other)
  {
    cardano_vote_delegation_cert_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
VoteDelegationCert::operator==(const VoteDelegationCert& other) const
{
  return credential() == other.credential() && drep() == other.drep();
}

bool
VoteDelegationCert::operator!=(const VoteDelegationCert& other) const
{
  return !(*this == other);
}

VoteDelegationCert
VoteDelegationCert::adopt(cardano_vote_delegation_cert_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null VoteDelegationCert");
  }

  return VoteDelegationCert(value);
}

cardano_vote_delegation_cert_t*
VoteDelegationCert::nativeHandle() noexcept
{
  return m_value;
}

const cardano_vote_delegation_cert_t*
VoteDelegationCert::nativeHandle() const noexcept
{
  return m_value;
}

cardano_vote_delegation_cert_t*
VoteDelegationCert::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

VoteDelegationCert::VoteDelegationCert(cardano_vote_delegation_cert_t* value) noexcept
: m_value(value)
{
}

cardano_vote_delegation_cert_t*
VoteDelegationCert::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VoteDelegationCert");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VoteDelegationCert& lhs, VoteDelegationCert& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
