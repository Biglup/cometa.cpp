/**
 * @file Committee.cpp
 *
 * @brief Constitutional committee membership and quorum.
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

#include <Cometa/ProposalProcedures/Committee.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/common/unit_interval.h>
#include <cardano/proposal_procedures/committee.h>
#include <cardano/proposal_procedures/credential_set.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Committee::Committee(const UnitInterval& quorumThreshold)
: m_value(nullptr)
{
  const UnitInterval                                          copy(quorumThreshold);
  cardano_committee_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_committee_new(checkedNativeHandle(copy), &value);
  CardanoObject<cardano_committee_t, cardano_committee_unref> owner(value);

  checkCardanoError(status, "Committee");
  m_value = owner.release();
}

Committee::Committee(const Committee& other)
: m_value(nullptr)
{
  cardano_committee_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_committee_deep_clone(other.checked(), &value);
  CardanoObject<cardano_committee_t, cardano_committee_unref> owner(value);

  checkCardanoError(status, "Committee copy");
  m_value = owner.release();
}

Committee::Committee(Committee&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Committee::~Committee()
{
  cardano_committee_unref(&m_value);
}

Committee
Committee::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Committee(decodeCbor<cardano_committee_t, cardano_committee_unref>(cbor, cardano_committee_from_cbor));
}

Committee
Committee::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

UnitInterval
Committee::quorumThreshold() const
{
  cardano_unit_interval_t*                                            value = cardano_committee_get_quorum_threshold(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("Committee has no quorum threshold");
  }

  const UnitInterval found = UnitInterval::adopt(owner.release());
  return UnitInterval(found);
}

void
Committee::setQuorumThreshold(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_committee_set_quorum_threshold(checked(), checkedNativeHandle(copy)), "Committee setQuorumThreshold");
}

void
Committee::addMember(const Credential& credential, std::uint64_t epoch)
{
  const Credential copy(credential);
  checkCardanoError(cardano_committee_add_member(checked(), checkedNativeHandle(copy), epoch), "Committee addMember");
}

CredentialSet
Committee::keys() const
{
  cardano_credential_set_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_committee_members_keys(checked(), &value);
  CardanoObject<cardano_credential_set_t, cardano_credential_set_unref> owner(value);

  checkCardanoError(status, "Committee keys");
  return CredentialSet::adopt(owner.release());
}

std::uint64_t
Committee::memberEpoch(const Credential& credential) const
{
  return cardano_committee_get_member_epoch(checked(), checkedNativeHandle(credential));
}

Credential
Committee::keyAt(std::size_t index) const
{
  if (index >= keys().size())
  {
    throw std::out_of_range("Committee index is out of range");
  }

  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_committee_get_key_at(checked(), index, &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "Committee keyAt");
  const Credential found = Credential::adopt(owner.release());
  return Credential(found);
}

std::uint64_t
Committee::valueAt(std::size_t index) const
{
  if (index >= keys().size())
  {
    throw std::out_of_range("Committee index is out of range");
  }

  std::uint64_t value = 0;
  checkCardanoError(cardano_committee_get_value_at(checked(), index, &value), "Committee valueAt");
  return value;
}

std::pair<Credential, std::uint64_t>
Committee::at(std::size_t index) const
{
  if (index >= keys().size())
  {
    throw std::out_of_range("Committee index is out of range");
  }

  cardano_credential_t*                                         key    = nullptr;
  std::uint64_t                                                 epoch  = 0;
  const cardano_error_t                                         status = cardano_committee_get_key_value_at(checked(), index, &key, &epoch);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(key);

  checkCardanoError(status, "Committee at");
  const Credential found = Credential::adopt(owner.release());
  return { Credential(found), epoch };
}

std::vector<std::uint8_t>
Committee::toCbor() const
{
  return encodeCbor(checked(), cardano_committee_to_cbor);
}

void
Committee::swap(Committee& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Committee&
Committee::operator=(const Committee& other)
{
  if (this != &other)
  {
    Committee copy(other);
    swap(copy);
  }
  return *this;
}

Committee&
Committee::operator=(Committee&& other) noexcept
{
  if (this != &other)
  {
    cardano_committee_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Committee::operator==(const Committee& other) const
{
  return toCbor() == other.toCbor();
}

bool
Committee::operator!=(const Committee& other) const
{
  return !(*this == other);
}

Committee
Committee::adopt(cardano_committee_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null Committee");
  }

  return Committee(value);
}

cardano_committee_t*
Committee::nativeHandle() noexcept
{
  return m_value;
}

const cardano_committee_t*
Committee::nativeHandle() const noexcept
{
  return m_value;
}

cardano_committee_t*
Committee::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

Committee::Committee(cardano_committee_t* value) noexcept
: m_value(value)
{
}

cardano_committee_t*
Committee::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Committee");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Committee& lhs, Committee& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
