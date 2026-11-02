/**
 * @file AssetIdMap.cpp
 *
 * @brief Signed asset quantity map operations.
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

#include <Cometa/Assets/AssetIdMap.h>

#include "../CardanoInterop.h"
#include "AssetArithmetic.h"

#include <cardano/assets/asset_id_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AssetIdMap::AssetIdMap()
: m_value(nullptr)
{
  cardano_asset_id_map_t*                                           value  = nullptr;
  const cardano_error_t                                             result = cardano_asset_id_map_new(&value);
  CardanoObject<cardano_asset_id_map_t, cardano_asset_id_map_unref> owner(value);

  checkCardanoError(result, "AssetIdMap");
  m_value = owner.release();
}

AssetIdMap::AssetIdMap(std::initializer_list<std::pair<AssetId, std::int64_t>> entries)
: AssetIdMap()
{
  for (const std::pair<AssetId, std::int64_t>& entry: entries)
  {
    set(entry.first, entry.second);
  }
}

AssetIdMap::AssetIdMap(const AssetIdMap& other)
: m_value(nullptr)
{
  cardano_asset_id_map_t*                                           value  = nullptr;
  const cardano_error_t                                             result = cardano_asset_id_map_deep_clone(other.checked(), &value);
  CardanoObject<cardano_asset_id_map_t, cardano_asset_id_map_unref> owner(value);

  checkCardanoError(result, "AssetIdMap copy");
  m_value = owner.release();
}

AssetIdMap::AssetIdMap(AssetIdMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AssetIdMap::~AssetIdMap()
{
  cardano_asset_id_map_unref(&m_value);
}

std::size_t
AssetIdMap::size() const
{
  return cardano_asset_id_map_get_length(checked());
}

bool
AssetIdMap::empty() const
{
  return size() == 0;
}

std::optional<std::int64_t>
AssetIdMap::get(const AssetId& key) const
{
  std::int64_t          amount = 0;
  const cardano_error_t result = cardano_asset_id_map_get(checked(), key.checked(), &amount);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "AssetIdMap get");
  return amount;
}

std::int64_t
AssetIdMap::at(const AssetId& key) const
{
  const std::optional<std::int64_t> result = get(key);

  if (!result)
  {
    throw std::out_of_range("Asset key is absent");
  }

  return *result;
}

void
AssetIdMap::set(const AssetId& key, std::int64_t amount)
{
  const AssetId copy(key);

  checkCardanoError(cardano_asset_id_map_insert(checked(), copy.checked(), amount), "AssetIdMap set");
}

AssetIdList
AssetIdMap::keys() const
{
  AssetIdList       result;
  const std::size_t count = size();
  result.reserve(count);

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_asset_id_t*                                       key    = nullptr;
    const cardano_error_t                                     status = cardano_asset_id_map_get_key_at(checked(), i, &key);
    CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(key);

    checkCardanoError(status, "AssetIdMap keys");
    const AssetId value(owner.release());
    result.push_back(value);
  }

  return result;
}

std::vector<std::pair<AssetId, std::int64_t>>
AssetIdMap::entries() const
{
  std::vector<std::pair<AssetId, std::int64_t>> result;
  const std::size_t                             count = size();
  result.reserve(count);

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_asset_id_t*                                       key    = nullptr;
    std::int64_t                                              amount = 0;
    const cardano_error_t                                     status = cardano_asset_id_map_get_key_value_at(checked(), i, &key, &amount);
    CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(key);

    checkCardanoError(status, "AssetIdMap entries");
    const AssetId value(owner.release());
    result.emplace_back(value, amount);
  }

  return result;
}

void
AssetIdMap::swap(AssetIdMap& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AssetIdMap&
AssetIdMap::operator=(const AssetIdMap& other)
{
  if (this != &other)
  {
    AssetIdMap copy(other);
    swap(copy);
  }

  return *this;
}

AssetIdMap&
AssetIdMap::operator=(AssetIdMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_asset_id_map_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

AssetIdMap
AssetIdMap::operator+(const AssetIdMap& other) const
{
  const std::vector<std::pair<AssetId, std::int64_t>> right = other.entries();

  for (const std::pair<AssetId, std::int64_t>& entry: right)
  {
    const std::int64_t left = get(entry.first).value_or(0);
    (void)checkedAddAmount(left, entry.second);
  }

  cardano_asset_id_map_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_asset_id_map_add(checked(), other.checked(), &value);
  CardanoObject<cardano_asset_id_map_t, cardano_asset_id_map_unref> owner(value);

  checkCardanoError(status, "AssetIdMap add");
  return AssetIdMap(owner.release());
}

AssetIdMap&
AssetIdMap::operator+=(const AssetIdMap& other)
{
  AssetIdMap result = *this + other;
  swap(result);

  return *this;
}

AssetIdMap
AssetIdMap::operator-(const AssetIdMap& other) const
{
  const std::vector<std::pair<AssetId, std::int64_t>> right = other.entries();

  for (const std::pair<AssetId, std::int64_t>& entry: right)
  {
    const std::int64_t left = get(entry.first).value_or(0);
    (void)checkedSubtractAmount(left, entry.second);
  }

  cardano_asset_id_map_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_asset_id_map_subtract(checked(), other.checked(), &value);
  CardanoObject<cardano_asset_id_map_t, cardano_asset_id_map_unref> owner(value);

  checkCardanoError(status, "AssetIdMap subtract");
  return AssetIdMap(owner.release());
}

AssetIdMap&
AssetIdMap::operator-=(const AssetIdMap& other)
{
  AssetIdMap result = *this - other;
  swap(result);

  return *this;
}

bool
AssetIdMap::operator==(const AssetIdMap& other) const
{
  return cardano_asset_id_map_equals(checked(), other.checked());
}

bool
AssetIdMap::operator!=(const AssetIdMap& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

AssetIdMap::AssetIdMap(cardano_asset_id_map_t* value) noexcept
: m_value(value)
{
}

cardano_asset_id_map_t*
AssetIdMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AssetIdMap");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AssetIdMap& lhs, AssetIdMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
