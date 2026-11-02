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
}

void
DirectDepositMap::insert(std::string_view address, std::uint64_t amount)
{
  insert(RewardAddress::fromBech32(address), amount);
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
    m_value = std::exchange(other.m_value, nullptr);
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
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(DirectDepositMap& lhs, DirectDepositMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
