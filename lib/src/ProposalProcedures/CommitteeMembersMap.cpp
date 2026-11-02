/**
 * @file CommitteeMembersMap.cpp
 *
 * @brief Committee members and their term expiration epochs.
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

#include <Cometa/ProposalProcedures/CommitteeMembersMap.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/proposal_procedures/committee_members_map.h>
#include <cardano/proposal_procedures/credential_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CommitteeMembersMap::CommitteeMembersMap()
: m_value(nullptr)
{
  cardano_committee_members_map_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_committee_members_map_new(&value);
  CardanoObject<cardano_committee_members_map_t, cardano_committee_members_map_unref> owner(value);

  checkCardanoError(status, "CommitteeMembersMap");
  m_value = owner.release();
}

CommitteeMembersMap::CommitteeMembersMap(const CommitteeMembersMap& other)
: m_value(nullptr)
{
  cardano_committee_members_map_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_committee_members_map_deep_clone(other.checked(), &value);
  CardanoObject<cardano_committee_members_map_t, cardano_committee_members_map_unref> owner(value);

  checkCardanoError(status, "CommitteeMembersMap copy");
  m_value = owner.release();
}

CommitteeMembersMap::CommitteeMembersMap(CommitteeMembersMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

CommitteeMembersMap::~CommitteeMembersMap()
{
  cardano_committee_members_map_unref(&m_value);
}

CommitteeMembersMap
CommitteeMembersMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return CommitteeMembersMap(decodeCbor<cardano_committee_members_map_t, cardano_committee_members_map_unref>(cbor, cardano_committee_members_map_from_cbor));
}

CommitteeMembersMap
CommitteeMembersMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
CommitteeMembersMap::size() const
{
  return cardano_committee_members_map_get_length(checked());
}

void
CommitteeMembersMap::insert(const Credential& credential, std::uint64_t epoch)
{
  const Credential copy(credential);
  checkCardanoError(cardano_committee_members_map_insert(checked(), copy.checked(), epoch), "CommitteeMembersMap insert");
}

std::optional<std::uint64_t>
CommitteeMembersMap::get(const Credential& credential) const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_committee_members_map_get(checked(), credential.checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "CommitteeMembersMap get");
  return value;
}

CredentialSet
CommitteeMembersMap::keys() const
{
  cardano_credential_set_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_committee_members_map_get_keys(checked(), &value);
  CardanoObject<cardano_credential_set_t, cardano_credential_set_unref> owner(value);

  checkCardanoError(status, "CommitteeMembersMap keys");
  return CredentialSet(owner.release());
}

Credential
CommitteeMembersMap::keyAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("CommitteeMembersMap index is out of range");
  }

  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_committee_members_map_get_key_at(checked(), index, &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "CommitteeMembersMap keyAt");
  const Credential found(owner.release());
  return Credential(found);
}

std::uint64_t
CommitteeMembersMap::valueAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("CommitteeMembersMap index is out of range");
  }

  std::uint64_t value = 0;
  checkCardanoError(cardano_committee_members_map_get_value_at(checked(), index, &value), "CommitteeMembersMap valueAt");
  return value;
}

std::pair<Credential, std::uint64_t>
CommitteeMembersMap::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("CommitteeMembersMap index is out of range");
  }

  cardano_credential_t*                                         key    = nullptr;
  std::uint64_t                                                 epoch  = 0;
  const cardano_error_t                                         status = cardano_committee_members_map_get_key_value_at(checked(), index, &key, &epoch);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(key);

  checkCardanoError(status, "CommitteeMembersMap at");
  const Credential found(owner.release());
  return { Credential(found), epoch };
}

std::vector<std::uint8_t>
CommitteeMembersMap::toCbor() const
{
  return encodeCbor(checked(), cardano_committee_members_map_to_cbor);
}

std::string
CommitteeMembersMap::toJson() const
{
  return encodeJson(checked(), cardano_committee_members_map_to_cip116_json);
}

void
CommitteeMembersMap::swap(CommitteeMembersMap& other) noexcept
{
  std::swap(m_value, other.m_value);
}

CommitteeMembersMap&
CommitteeMembersMap::operator=(const CommitteeMembersMap& other)
{
  if (this != &other)
  {
    CommitteeMembersMap copy(other);
    swap(copy);
  }
  return *this;
}

CommitteeMembersMap&
CommitteeMembersMap::operator=(CommitteeMembersMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_committee_members_map_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
CommitteeMembersMap::operator==(const CommitteeMembersMap& other) const
{
  return toCbor() == other.toCbor();
}

bool
CommitteeMembersMap::operator!=(const CommitteeMembersMap& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

CommitteeMembersMap::CommitteeMembersMap(cardano_committee_members_map_t* value) noexcept
: m_value(value)
{
}

cardano_committee_members_map_t*
CommitteeMembersMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CommitteeMembersMap");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CommitteeMembersMap& lhs, CommitteeMembersMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
