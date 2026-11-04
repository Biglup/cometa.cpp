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

#include <utility>

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
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
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
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_committee_members_map_get_length(checked());
}

void
CommitteeMembersMap::insert(const Credential& credential, std::uint64_t epoch)
{
  const Credential copy(credential);
  checkCardanoError(cardano_committee_members_map_insert(checked(), checkedNativeHandle(copy), epoch), "CommitteeMembersMap insert");
  invalidateElements();
}

std::optional<std::uint64_t>
CommitteeMembersMap::get(const Credential& credential) const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_committee_members_map_get(checked(), checkedNativeHandle(credential), &value);

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
  return CredentialSet::adopt(owner.release());
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
  const Credential found = Credential::adopt(owner.release());
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
  const Credential found = Credential::adopt(owner.release());
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
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
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
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
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

CommitteeMembersMap::iterator
CommitteeMembersMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

CommitteeMembersMap::const_iterator
CommitteeMembersMap::begin() const
{
  materialize();

  return m_elements.begin();
}

CommitteeMembersMap::iterator
CommitteeMembersMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

CommitteeMembersMap::const_iterator
CommitteeMembersMap::end() const
{
  materialize();

  return m_elements.end();
}

CommitteeMembersMap::reverse_iterator
CommitteeMembersMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

CommitteeMembersMap::const_reverse_iterator
CommitteeMembersMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

CommitteeMembersMap::reverse_iterator
CommitteeMembersMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

CommitteeMembersMap::const_reverse_iterator
CommitteeMembersMap::rend() const
{
  materialize();

  return m_elements.rend();
}

CommitteeMembersMap::const_iterator
CommitteeMembersMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

CommitteeMembersMap::const_iterator
CommitteeMembersMap::cend() const
{
  materialize();

  return m_elements.cend();
}

CommitteeMembersMap::const_reverse_iterator
CommitteeMembersMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

CommitteeMembersMap::const_reverse_iterator
CommitteeMembersMap::crend() const
{
  materialize();

  return m_elements.crend();
}

CommitteeMembersMap::iterator
CommitteeMembersMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

CommitteeMembersMap::const_iterator
CommitteeMembersMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

CommitteeMembersMap::size_type
CommitteeMembersMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
CommitteeMembersMap::contains(const key_type& key) const
{
  return count(key) != 0;
}

CommitteeMembersMap::mapped_type&
CommitteeMembersMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const CommitteeMembersMap::mapped_type&
CommitteeMembersMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<CommitteeMembersMap::iterator, bool>
CommitteeMembersMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  CommitteeMembersMap validation;
  validation.insert(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

CommitteeMembersMap::iterator
CommitteeMembersMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<CommitteeMembersMap::iterator, bool>
CommitteeMembersMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  CommitteeMembersMap validation;
  validation.insert(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

CommitteeMembersMap::mapped_type&
CommitteeMembersMap::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

CommitteeMembersMap::iterator
CommitteeMembersMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

CommitteeMembersMap::iterator
CommitteeMembersMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

CommitteeMembersMap::size_type
CommitteeMembersMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
CommitteeMembersMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

bool
CommitteeMembersMap::empty() const
{
  return size() == 0;
}

CommitteeMembersMap
CommitteeMembersMap::adopt(cardano_committee_members_map_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null CommitteeMembersMap");
  }

  return CommitteeMembersMap(value);
}

cardano_committee_members_map_t*
CommitteeMembersMap::nativeHandle()
{
  if (m_value)
  {
    synchronize();
  }

  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;

  return m_value;
}

const cardano_committee_members_map_t*
CommitteeMembersMap::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_committee_members_map_t*
CommitteeMembersMap::release()
{
  if (m_value)
  {
    synchronize();
  }

  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;

  return std::exchange(m_value, nullptr);
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
  synchronize();

  return m_value;
}

void
CommitteeMembersMap::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  std::vector<std::pair<key_type, mapped_type>> entriesCopy;

  for (size_type index = 0; index < size(); ++index)
  {
    entriesCopy.push_back(at(index));
  }
  std::map<key_type, mapped_type, key_compare> elements(compareKeys);

  for (const std::pair<key_type, mapped_type>& entry: entriesCopy)
  {
    elements.emplace(entry.first, entry.second);
  }

  m_elements.swap(elements);

  try
  {
    std::vector<std::vector<std::uint8_t>> encoding = encodeElements();
    m_encoding.swap(encoding);
    m_materialized = true;
  }
  catch (...)
  {
    m_elements.clear();
    throw;
  }
}

void
CommitteeMembersMap::synchronize() const
{
  if (!m_materialized || !m_mutableAccess)
  {
    return;
  }

  std::vector<std::vector<std::uint8_t>> encoding = encodeElements();

  if (encoding == m_encoding)
  {
    return;
  }

  CommitteeMembersMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.insert(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
CommitteeMembersMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
CommitteeMembersMap::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    encoding.push_back(entry.first.toCbor());
    const std::string second = std::to_string(entry.second);
    encoding.emplace_back(second.begin(), second.end());
  }

  return encoding;
}

bool
CommitteeMembersMap::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CommitteeMembersMap& lhs, CommitteeMembersMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
