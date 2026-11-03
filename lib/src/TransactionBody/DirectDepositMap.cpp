/**
 * @file DirectDepositMap.cpp
 *
 * @brief Direct deposits to reward accounts.
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

#include <Cometa/TransactionBody/DirectDepositMap.h>

#include "../CardanoInterop.h"

#include <cardano/transaction_body/direct_deposit_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
DirectDepositMap::DirectDepositMap()
: m_value(nullptr)
{
  cardano_direct_deposit_map_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_direct_deposit_map_new(&value);
  CardanoObject<cardano_direct_deposit_map_t, cardano_direct_deposit_map_unref> owner(value);

  checkCardanoError(status, "DirectDepositMap");
  m_value = owner.release();
}

DirectDepositMap::DirectDepositMap(const DirectDepositMap& other)
: m_value(nullptr)
{
  cardano_direct_deposit_map_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_direct_deposit_map_deep_clone(other.checked(), &value);
  CardanoObject<cardano_direct_deposit_map_t, cardano_direct_deposit_map_unref> owner(value);

  checkCardanoError(status, "DirectDepositMap copy");
  m_value = owner.release();
}

DirectDepositMap::DirectDepositMap(DirectDepositMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

DirectDepositMap::~DirectDepositMap()
{
  cardano_direct_deposit_map_unref(&m_value);
}

DirectDepositMap
DirectDepositMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return DirectDepositMap(decodeCbor<cardano_direct_deposit_map_t, cardano_direct_deposit_map_unref>(cbor, cardano_direct_deposit_map_from_cbor));
}

DirectDepositMap
DirectDepositMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
DirectDepositMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_direct_deposit_map_get_length(checked());
}

bool
DirectDepositMap::empty() const
{
  return size() == 0U;
}

std::optional<std::uint64_t>
DirectDepositMap::get(const RewardAddress& address) const
{
  std::uint64_t         amount = 0;
  const cardano_error_t status = cardano_direct_deposit_map_get(checked(), address.checked(), &amount);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "DirectDepositMap get");
  return amount;
}

void
DirectDepositMap::insert(const RewardAddress& address, std::uint64_t amount)
{
  const RewardAddress copy(address);
  checkCardanoError(cardano_direct_deposit_map_insert(checked(), copy.checked(), amount), "DirectDepositMap insert");
  invalidateElements();
}

void
DirectDepositMap::insert(std::string_view address, std::uint64_t amount)
{
  insert(RewardAddress::fromBech32(address), amount);
  invalidateElements();
}

std::pair<RewardAddress, std::uint64_t>
DirectDepositMap::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Direct deposit index is out of range");
  }

  cardano_reward_address_t*                                             key    = nullptr;
  std::uint64_t                                                         amount = 0;
  const cardano_error_t                                                 status = cardano_direct_deposit_map_get_key_value_at(checked(), index, &key, &amount);
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(key);

  checkCardanoError(status, "DirectDepositMap entryAt");
  RewardAddress address(*owner.release());
  return std::make_pair(std::move(address), amount);
}

RewardAddressList
DirectDepositMap::keys() const
{
  RewardAddressList result;
  result.reserve(size());

  for (std::size_t index = 0; index < size(); ++index)
  {
    result.push_back(entryAt(index).first);
  }

  return result;
}

std::vector<std::pair<RewardAddress, std::uint64_t>>
DirectDepositMap::entries() const
{
  std::vector<std::pair<RewardAddress, std::uint64_t>> result;
  result.reserve(size());

  for (std::size_t index = 0; index < size(); ++index)
  {
    result.push_back(entryAt(index));
  }

  return result;
}

std::vector<std::uint8_t>
DirectDepositMap::toCbor() const
{
  return encodeCbor(checked(), cardano_direct_deposit_map_to_cbor);
}

std::string
DirectDepositMap::toJson() const
{
  return encodeJson(checked(), cardano_direct_deposit_map_to_cip116_json);
}

void
DirectDepositMap::swap(DirectDepositMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

DirectDepositMap&
DirectDepositMap::operator=(const DirectDepositMap& other)
{
  if (this != &other)
  {
    DirectDepositMap copy(other);
    swap(copy);
  }
  return *this;
}

DirectDepositMap&
DirectDepositMap::operator=(DirectDepositMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_direct_deposit_map_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }
  return *this;
}

bool
DirectDepositMap::operator==(const DirectDepositMap& other) const
{
  return toCbor() == other.toCbor();
}

bool
DirectDepositMap::operator!=(const DirectDepositMap& other) const
{
  return !(*this == other);
}

DirectDepositMap::iterator
DirectDepositMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

DirectDepositMap::const_iterator
DirectDepositMap::begin() const
{
  materialize();

  return m_elements.begin();
}

DirectDepositMap::iterator
DirectDepositMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

DirectDepositMap::const_iterator
DirectDepositMap::end() const
{
  materialize();

  return m_elements.end();
}

DirectDepositMap::reverse_iterator
DirectDepositMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

DirectDepositMap::const_reverse_iterator
DirectDepositMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

DirectDepositMap::reverse_iterator
DirectDepositMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

DirectDepositMap::const_reverse_iterator
DirectDepositMap::rend() const
{
  materialize();

  return m_elements.rend();
}

DirectDepositMap::const_iterator
DirectDepositMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

DirectDepositMap::const_iterator
DirectDepositMap::cend() const
{
  materialize();

  return m_elements.cend();
}

DirectDepositMap::const_reverse_iterator
DirectDepositMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

DirectDepositMap::const_reverse_iterator
DirectDepositMap::crend() const
{
  materialize();

  return m_elements.crend();
}

DirectDepositMap::iterator
DirectDepositMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

DirectDepositMap::const_iterator
DirectDepositMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

DirectDepositMap::size_type
DirectDepositMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
DirectDepositMap::contains(const key_type& key) const
{
  return count(key) != 0;
}

DirectDepositMap::mapped_type&
DirectDepositMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const DirectDepositMap::mapped_type&
DirectDepositMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<DirectDepositMap::iterator, bool>
DirectDepositMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  DirectDepositMap validation;
  validation.insert(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

DirectDepositMap::iterator
DirectDepositMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<DirectDepositMap::iterator, bool>
DirectDepositMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  DirectDepositMap validation;
  validation.insert(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

DirectDepositMap::mapped_type&
DirectDepositMap::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

DirectDepositMap::iterator
DirectDepositMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

DirectDepositMap::iterator
DirectDepositMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

DirectDepositMap::size_type
DirectDepositMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
DirectDepositMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

/* PRIVATE FUNCTIONS *********************************************************/

DirectDepositMap::DirectDepositMap(cardano_direct_deposit_map_t* value) noexcept
: m_value(value)
{
}

cardano_direct_deposit_map_t*
DirectDepositMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from DirectDepositMap");
  }
  synchronize();

  return m_value;
}

void
DirectDepositMap::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  const std::vector<std::pair<key_type, mapped_type>> entriesCopy = entries();
  std::map<key_type, mapped_type, key_compare>        elements(compareKeys);

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
DirectDepositMap::synchronize() const
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

  DirectDepositMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.insert(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
DirectDepositMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
DirectDepositMap::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    encoding.push_back(entry.first.toBytes());
    const std::string second = std::to_string(entry.second);
    encoding.emplace_back(second.begin(), second.end());
  }

  return encoding;
}

bool
DirectDepositMap::compareKeys(const key_type& left, const key_type& right)
{
  return left.toBytes() < right.toBytes();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(DirectDepositMap& lhs, DirectDepositMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
