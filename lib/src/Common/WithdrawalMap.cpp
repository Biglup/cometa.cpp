/**
 * @file WithdrawalMap.cpp
 *
 * @brief Reward addresses and withdrawal amounts in lovelace.
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

#include <Cometa/Common/WithdrawalMap.h>

#include "../CardanoInterop.h"

#include <cardano/address/reward_address.h>
#include <cardano/common/withdrawal_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
WithdrawalMap::WithdrawalMap()
: m_value(nullptr)
{
  cardano_withdrawal_map_t* value  = nullptr;
  const cardano_error_t     result = cardano_withdrawal_map_new(&value);

  CardanoObject<cardano_withdrawal_map_t, cardano_withdrawal_map_unref> owner(value);

  checkCardanoError(result, "WithdrawalMap");

  m_value = owner.release();
}

WithdrawalMap::WithdrawalMap(const WithdrawalMap& other)
: m_value(nullptr)
{
  cardano_withdrawal_map_t* value  = nullptr;
  const cardano_error_t     result = cardano_withdrawal_map_deep_clone(other.checked(), &value);

  CardanoObject<cardano_withdrawal_map_t, cardano_withdrawal_map_unref> owner(value);

  checkCardanoError(result, "WithdrawalMap");

  m_value = owner.release();
}

WithdrawalMap::WithdrawalMap(WithdrawalMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

WithdrawalMap::~WithdrawalMap()
{
  cardano_withdrawal_map_unref(&m_value);
}

WithdrawalMap
WithdrawalMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  WithdrawalMap           value(*decodeCbor<cardano_withdrawal_map_t, cardano_withdrawal_map_unref>(cbor, cardano_withdrawal_map_from_cbor));
  const RewardAddressList addresses = value.keys();

  for (std::size_t index = 1U; index < addresses.size(); ++index)
  {
    if (addresses[index - 1U] == addresses[index])
    {
      throw std::invalid_argument("Duplicate withdrawal address");
    }
  }

  return value;
}

WithdrawalMap
WithdrawalMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
WithdrawalMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_withdrawal_map_get_length(checked());
}

bool
WithdrawalMap::empty() const
{
  return size() == 0U;
}

std::optional<std::uint64_t>
WithdrawalMap::get(const RewardAddress& key) const
{
  std::uint64_t         amount = 0U;
  const cardano_error_t result = cardano_withdrawal_map_get(checked(), key.checked(), &amount);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "WithdrawalMap get");

  return amount;
}

void
WithdrawalMap::set(const RewardAddress& key, std::uint64_t amount)
{
  if (get(key))
  {
    WithdrawalMap                                              replacement;
    const std::vector<std::pair<RewardAddress, std::uint64_t>> current = entries();

    for (const std::pair<RewardAddress, std::uint64_t>& entry: current)
    {
      const std::uint64_t quantity = entry.first == key ? amount : entry.second;
      checkCardanoError(cardano_withdrawal_map_insert(replacement.checked(), entry.first.checked(), quantity), "WithdrawalMap replacement");
    }

    swap(replacement);
    return;
  }

  RewardAddress copy(key);

  checkCardanoError(cardano_withdrawal_map_insert(checked(), copy.checked(), amount), "WithdrawalMap set");
  invalidateElements();
}

void
WithdrawalMap::set(std::string_view rewardAddress, std::uint64_t amount)
{
  set(RewardAddress::fromBech32(rewardAddress), amount);
  invalidateElements();
}

std::pair<RewardAddress, std::uint64_t>
WithdrawalMap::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Withdrawal index");
  }

  cardano_reward_address_t* key    = nullptr;
  std::uint64_t             amount = 0U;
  const cardano_error_t     result = cardano_withdrawal_map_get_key_value_at(checked(), index, &key, &amount);

  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(key);

  checkCardanoError(result, "WithdrawalMap entryAt");

  const RewardAddress address(*owner.release());

  return std::make_pair(RewardAddress(address), amount);
}

RewardAddressList
WithdrawalMap::keys() const
{
  RewardAddressList result;
  const std::size_t elementCount = size();
  result.reserve(elementCount);

  for (std::size_t index = 0U; index < elementCount; ++index)
  {
    result.push_back(entryAt(index).first);
  }

  return result;
}

std::vector<std::pair<RewardAddress, std::uint64_t>>
WithdrawalMap::entries() const
{
  std::vector<std::pair<RewardAddress, std::uint64_t>> result;
  const std::size_t                                    elementCount = size();
  result.reserve(elementCount);

  for (std::size_t index = 0U; index < elementCount; ++index)
  {
    result.push_back(entryAt(index));
  }

  return result;
}

std::vector<std::uint8_t>
WithdrawalMap::toCbor() const
{
  return encodeCbor(checked(), cardano_withdrawal_map_to_cbor);
}

std::string
WithdrawalMap::toJson() const
{
  return encodeJson(checked(), cardano_withdrawal_map_to_cip116_json);
}

void
WithdrawalMap::swap(WithdrawalMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

WithdrawalMap&
WithdrawalMap::operator=(const WithdrawalMap& other)
{
  if (this != &other)
  {
    WithdrawalMap copy(other);

    swap(copy);
  }

  return *this;
}

WithdrawalMap&
WithdrawalMap::operator=(WithdrawalMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_withdrawal_map_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
WithdrawalMap::operator==(const WithdrawalMap& other) const
{
  return entries() == other.entries();
}

bool
WithdrawalMap::operator!=(const WithdrawalMap& other) const
{
  return !(*this == other);
}

WithdrawalMap::iterator
WithdrawalMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

WithdrawalMap::const_iterator
WithdrawalMap::begin() const
{
  materialize();

  return m_elements.begin();
}

WithdrawalMap::iterator
WithdrawalMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

WithdrawalMap::const_iterator
WithdrawalMap::end() const
{
  materialize();

  return m_elements.end();
}

WithdrawalMap::reverse_iterator
WithdrawalMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

WithdrawalMap::const_reverse_iterator
WithdrawalMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

WithdrawalMap::reverse_iterator
WithdrawalMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

WithdrawalMap::const_reverse_iterator
WithdrawalMap::rend() const
{
  materialize();

  return m_elements.rend();
}

WithdrawalMap::const_iterator
WithdrawalMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

WithdrawalMap::const_iterator
WithdrawalMap::cend() const
{
  materialize();

  return m_elements.cend();
}

WithdrawalMap::const_reverse_iterator
WithdrawalMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

WithdrawalMap::const_reverse_iterator
WithdrawalMap::crend() const
{
  materialize();

  return m_elements.crend();
}

WithdrawalMap::iterator
WithdrawalMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

WithdrawalMap::const_iterator
WithdrawalMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

WithdrawalMap::size_type
WithdrawalMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
WithdrawalMap::contains(const key_type& key) const
{
  return count(key) != 0;
}

WithdrawalMap::mapped_type&
WithdrawalMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const WithdrawalMap::mapped_type&
WithdrawalMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<WithdrawalMap::iterator, bool>
WithdrawalMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  WithdrawalMap validation;
  validation.set(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

WithdrawalMap::iterator
WithdrawalMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<WithdrawalMap::iterator, bool>
WithdrawalMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  WithdrawalMap validation;
  validation.set(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

WithdrawalMap::mapped_type&
WithdrawalMap::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

WithdrawalMap::iterator
WithdrawalMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

WithdrawalMap::iterator
WithdrawalMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

WithdrawalMap::size_type
WithdrawalMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
WithdrawalMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

/* PRIVATE FUNCTIONS *********************************************************/

WithdrawalMap::WithdrawalMap(cardano_withdrawal_map_t& value) noexcept
: m_value(&value)
{
}

cardano_withdrawal_map_t*
WithdrawalMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from WithdrawalMap");
  }

  synchronize();

  return m_value;
}

void
WithdrawalMap::materialize() const
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
WithdrawalMap::synchronize() const
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

  WithdrawalMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.set(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
WithdrawalMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
WithdrawalMap::encodeElements() const
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
WithdrawalMap::compareKeys(const key_type& left, const key_type& right)
{
  return left.toBytes() < right.toBytes();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(WithdrawalMap& lhs, WithdrawalMap& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
