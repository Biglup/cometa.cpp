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
}

void
AccountBalanceIntervalsMap::insert(std::string_view address, const AccountBalanceInterval& interval)
{
  insert(RewardAddress::fromBech32(address), interval);
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
    m_value = std::exchange(other.m_value, nullptr);
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
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AccountBalanceIntervalsMap& lhs, AccountBalanceIntervalsMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
