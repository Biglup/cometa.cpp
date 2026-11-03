/**
 * @file AccountBalanceIntervalsMap.cpp
 *
 * @brief Account balance requirements by reward address.
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

#include <Cometa/TransactionBody/AccountBalanceIntervalsMap.h>

#include "../CardanoInterop.h"

#include <cardano/transaction_body/account_balance_intervals_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AccountBalanceIntervalsMap::AccountBalanceIntervalsMap()
: m_value(nullptr)
{
  cardano_account_balance_intervals_map_t*                                                            value  = nullptr;
  const cardano_error_t                                                                               status = cardano_account_balance_intervals_map_new(&value);
  CardanoObject<cardano_account_balance_intervals_map_t, cardano_account_balance_intervals_map_unref> owner(value);

  checkCardanoError(status, "AccountBalanceIntervalsMap");
  m_value = owner.release();
}

AccountBalanceIntervalsMap::AccountBalanceIntervalsMap(const AccountBalanceIntervalsMap& other)
: m_value(nullptr)
{
  cardano_account_balance_intervals_map_t*                                                            value  = nullptr;
  const cardano_error_t                                                                               status = cardano_account_balance_intervals_map_deep_clone(other.checked(), &value);
  CardanoObject<cardano_account_balance_intervals_map_t, cardano_account_balance_intervals_map_unref> owner(value);

  checkCardanoError(status, "AccountBalanceIntervalsMap copy");
  m_value = owner.release();
}

AccountBalanceIntervalsMap::AccountBalanceIntervalsMap(AccountBalanceIntervalsMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

AccountBalanceIntervalsMap::~AccountBalanceIntervalsMap()
{
  cardano_account_balance_intervals_map_unref(&m_value);
}

AccountBalanceIntervalsMap
AccountBalanceIntervalsMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return AccountBalanceIntervalsMap(decodeCbor<cardano_account_balance_intervals_map_t, cardano_account_balance_intervals_map_unref>(cbor, cardano_account_balance_intervals_map_from_cbor));
}

AccountBalanceIntervalsMap
AccountBalanceIntervalsMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
AccountBalanceIntervalsMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_account_balance_intervals_map_get_length(checked());
}

bool
AccountBalanceIntervalsMap::empty() const
{
  return size() == 0U;
}

std::optional<AccountBalanceInterval>
AccountBalanceIntervalsMap::get(const RewardAddress& address) const
{
  cardano_account_balance_interval_t*                                                       value  = nullptr;
  const cardano_error_t                                                                     status = cardano_account_balance_intervals_map_get(checked(), address.checked(), &value);
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "AccountBalanceIntervalsMap get");
  const AccountBalanceInterval copied(owner.release());
  return AccountBalanceInterval(copied);
}

void
AccountBalanceIntervalsMap::insert(const RewardAddress& address, const AccountBalanceInterval& interval)
{
  const RewardAddress          copyAddress(address);
  const AccountBalanceInterval copyInterval(interval);
  checkCardanoError(cardano_account_balance_intervals_map_insert(checked(), copyAddress.checked(), copyInterval.checked()), "AccountBalanceIntervalsMap insert");
  invalidateElements();
}

void
AccountBalanceIntervalsMap::insert(std::string_view address, const AccountBalanceInterval& interval)
{
  insert(RewardAddress::fromBech32(address), interval);
  invalidateElements();
}

std::pair<RewardAddress, AccountBalanceInterval>
AccountBalanceIntervalsMap::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Account balance interval index is out of range");
  }

  cardano_reward_address_t*                                                                 key    = nullptr;
  cardano_account_balance_interval_t*                                                       value  = nullptr;
  const cardano_error_t                                                                     status = cardano_account_balance_intervals_map_get_key_value_at(checked(), index, &key, &value);
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref>                     keyOwner(key);
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> valueOwner(value);

  checkCardanoError(status, "AccountBalanceIntervalsMap entryAt");
  RewardAddress          address(*keyOwner.release());
  AccountBalanceInterval interval(valueOwner.release());
  return std::make_pair(std::move(address), std::move(interval));
}

RewardAddressList
AccountBalanceIntervalsMap::keys() const
{
  RewardAddressList result;
  result.reserve(size());

  for (std::size_t index = 0; index < size(); ++index)
  {
    result.push_back(entryAt(index).first);
  }

  return result;
}

std::vector<std::pair<RewardAddress, AccountBalanceInterval>>
AccountBalanceIntervalsMap::entries() const
{
  std::vector<std::pair<RewardAddress, AccountBalanceInterval>> result;
  result.reserve(size());

  for (std::size_t index = 0; index < size(); ++index)
  {
    result.push_back(entryAt(index));
  }

  return result;
}

std::vector<std::uint8_t>
AccountBalanceIntervalsMap::toCbor() const
{
  return encodeCbor(checked(), cardano_account_balance_intervals_map_to_cbor);
}

std::string
AccountBalanceIntervalsMap::toJson() const
{
  return encodeJson(checked(), cardano_account_balance_intervals_map_to_cip116_json);
}

void
AccountBalanceIntervalsMap::swap(AccountBalanceIntervalsMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

AccountBalanceIntervalsMap&
AccountBalanceIntervalsMap::operator=(const AccountBalanceIntervalsMap& other)
{
  if (this != &other)
  {
    AccountBalanceIntervalsMap copy(other);
    swap(copy);
  }
  return *this;
}

AccountBalanceIntervalsMap&
AccountBalanceIntervalsMap::operator=(AccountBalanceIntervalsMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_account_balance_intervals_map_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }
  return *this;
}

bool
AccountBalanceIntervalsMap::operator==(const AccountBalanceIntervalsMap& other) const
{
  return toCbor() == other.toCbor();
}

bool
AccountBalanceIntervalsMap::operator!=(const AccountBalanceIntervalsMap& other) const
{
  return !(*this == other);
}

AccountBalanceIntervalsMap::iterator
AccountBalanceIntervalsMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

AccountBalanceIntervalsMap::const_iterator
AccountBalanceIntervalsMap::begin() const
{
  materialize();

  return m_elements.begin();
}

AccountBalanceIntervalsMap::iterator
AccountBalanceIntervalsMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

AccountBalanceIntervalsMap::const_iterator
AccountBalanceIntervalsMap::end() const
{
  materialize();

  return m_elements.end();
}

AccountBalanceIntervalsMap::reverse_iterator
AccountBalanceIntervalsMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

AccountBalanceIntervalsMap::const_reverse_iterator
AccountBalanceIntervalsMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

AccountBalanceIntervalsMap::reverse_iterator
AccountBalanceIntervalsMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

AccountBalanceIntervalsMap::const_reverse_iterator
AccountBalanceIntervalsMap::rend() const
{
  materialize();

  return m_elements.rend();
}

AccountBalanceIntervalsMap::const_iterator
AccountBalanceIntervalsMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

AccountBalanceIntervalsMap::const_iterator
AccountBalanceIntervalsMap::cend() const
{
  materialize();

  return m_elements.cend();
}

AccountBalanceIntervalsMap::const_reverse_iterator
AccountBalanceIntervalsMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

AccountBalanceIntervalsMap::const_reverse_iterator
AccountBalanceIntervalsMap::crend() const
{
  materialize();

  return m_elements.crend();
}

AccountBalanceIntervalsMap::iterator
AccountBalanceIntervalsMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

AccountBalanceIntervalsMap::const_iterator
AccountBalanceIntervalsMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

AccountBalanceIntervalsMap::size_type
AccountBalanceIntervalsMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
AccountBalanceIntervalsMap::contains(const key_type& key) const
{
  return count(key) != 0;
}

AccountBalanceIntervalsMap::mapped_type&
AccountBalanceIntervalsMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const AccountBalanceIntervalsMap::mapped_type&
AccountBalanceIntervalsMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<AccountBalanceIntervalsMap::iterator, bool>
AccountBalanceIntervalsMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  AccountBalanceIntervalsMap validation;
  validation.insert(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

AccountBalanceIntervalsMap::iterator
AccountBalanceIntervalsMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<AccountBalanceIntervalsMap::iterator, bool>
AccountBalanceIntervalsMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  AccountBalanceIntervalsMap validation;
  validation.insert(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

AccountBalanceIntervalsMap::iterator
AccountBalanceIntervalsMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

AccountBalanceIntervalsMap::iterator
AccountBalanceIntervalsMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

AccountBalanceIntervalsMap::size_type
AccountBalanceIntervalsMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
AccountBalanceIntervalsMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

/* PRIVATE FUNCTIONS *********************************************************/

AccountBalanceIntervalsMap::AccountBalanceIntervalsMap(cardano_account_balance_intervals_map_t* value) noexcept
: m_value(value)
{
}

cardano_account_balance_intervals_map_t*
AccountBalanceIntervalsMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AccountBalanceIntervalsMap");
  }
  synchronize();

  return m_value;
}

void
AccountBalanceIntervalsMap::materialize() const
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
AccountBalanceIntervalsMap::synchronize() const
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

  AccountBalanceIntervalsMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.insert(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
AccountBalanceIntervalsMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
AccountBalanceIntervalsMap::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    encoding.push_back(entry.first.toBytes());
    encoding.push_back(entry.second.toCbor());
  }

  return encoding;
}

bool
AccountBalanceIntervalsMap::compareKeys(const key_type& left, const key_type& right)
{
  return left.toBytes() < right.toBytes();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AccountBalanceIntervalsMap& lhs, AccountBalanceIntervalsMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
