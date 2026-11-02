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

std::uint64_t
WithdrawalMap::at(const RewardAddress& key) const
{
  const std::optional<std::uint64_t> amount = get(key);

  if (!amount)
  {
    throw std::out_of_range("Withdrawal address not found");
  }

  return *amount;
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
}

void
WithdrawalMap::set(std::string_view rewardAddress, std::uint64_t amount)
{
  set(RewardAddress::fromBech32(rewardAddress), amount);
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
  const std::size_t count = size();
  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(entryAt(index).first);
  }

  return result;
}

std::vector<std::pair<RewardAddress, std::uint64_t>>
WithdrawalMap::entries() const
{
  std::vector<std::pair<RewardAddress, std::uint64_t>> result;
  const std::size_t                                    count = size();
  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
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
    m_value = std::exchange(other.m_value, nullptr);
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

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(WithdrawalMap& lhs, WithdrawalMap& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
