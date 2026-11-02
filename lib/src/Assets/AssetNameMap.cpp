/**
 * @file AssetNameMap.cpp
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

#include <Cometa/Assets/AssetNameMap.h>

#include "../CardanoInterop.h"
#include "AssetArithmetic.h"

#include <cardano/assets/asset_name_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AssetNameMap::AssetNameMap()
: m_value(nullptr)
{
  cardano_asset_name_map_t*                                             value  = nullptr;
  const cardano_error_t                                                 result = cardano_asset_name_map_new(&value);
  CardanoObject<cardano_asset_name_map_t, cardano_asset_name_map_unref> owner(value);

  checkCardanoError(result, "AssetNameMap");
  m_value = owner.release();
}

AssetNameMap::AssetNameMap(std::initializer_list<std::pair<AssetName, std::int64_t>> entries)
: AssetNameMap()
{
  for (const std::pair<AssetName, std::int64_t>& entry: entries)
  {
    set(entry.first, entry.second);
  }
}

AssetNameMap::AssetNameMap(const AssetNameMap& other)
: m_value(nullptr)
{
  cardano_asset_name_map_t*                                             value  = nullptr;
  const cardano_error_t                                                 result = cardano_asset_name_map_deep_clone(other.checked(), &value);
  CardanoObject<cardano_asset_name_map_t, cardano_asset_name_map_unref> owner(value);

  checkCardanoError(result, "AssetNameMap copy");
  m_value = owner.release();
}

AssetNameMap::AssetNameMap(AssetNameMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

AssetNameMap::~AssetNameMap()
{
  cardano_asset_name_map_unref(&m_value);
}

AssetNameMap
AssetNameMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  AssetNameMap        result(decodeCbor<cardano_asset_name_map_t, cardano_asset_name_map_unref>(cbor, cardano_asset_name_map_from_cbor));
  const AssetNameList parsedKeys = result.keys();

  for (std::size_t i = 1; i < parsedKeys.size(); ++i)
  {
    if (parsedKeys[i - 1] == parsedKeys[i])
    {
      throw std::invalid_argument("Duplicate asset name in CBOR map");
    }
  }

  return result;
}

AssetNameMap
AssetNameMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
AssetNameMap::size() const
{
  return cardano_asset_name_map_get_length(checked());
}

bool
AssetNameMap::empty() const
{
  return size() == 0;
}

std::optional<std::int64_t>
AssetNameMap::get(const AssetName& key) const
{
  std::int64_t          amount = 0;
  const cardano_error_t result = cardano_asset_name_map_get(checked(), key.checked(), &amount);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "AssetNameMap get");
  return amount;
}

std::int64_t
AssetNameMap::at(const AssetName& key) const
{
  const std::optional<std::int64_t> result = get(key);

  if (!result)
  {
    throw std::out_of_range("Asset key is absent");
  }

  return *result;
}

void
AssetNameMap::set(const AssetName& key, std::int64_t amount)
{
  const AssetName copy(key);

  checkCardanoError(cardano_asset_name_map_insert(checked(), copy.checked(), amount), "AssetNameMap set");
}

AssetNameList
AssetNameMap::keys() const
{
  AssetNameList     result;
  const std::size_t count = size();
  result.reserve(count);

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_asset_name_t*                                         key    = nullptr;
    const cardano_error_t                                         status = cardano_asset_name_map_get_key_at(checked(), i, &key);
    CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> owner(key);

    checkCardanoError(status, "AssetNameMap keys");
    const AssetName value(owner.release());
    result.push_back(value);
  }

  return result;
}

std::vector<std::pair<AssetName, std::int64_t>>
AssetNameMap::entries() const
{
  std::vector<std::pair<AssetName, std::int64_t>> result;
  const std::size_t                               count = size();
  result.reserve(count);

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_asset_name_t*                                         key    = nullptr;
    std::int64_t                                                  amount = 0;
    const cardano_error_t                                         status = cardano_asset_name_map_get_key_value_at(checked(), i, &key, &amount);
    CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> owner(key);

    checkCardanoError(status, "AssetNameMap entries");
    const AssetName value(owner.release());
    result.emplace_back(value, amount);
  }

  return result;
}

std::vector<std::uint8_t>
AssetNameMap::toCbor() const
{
  return encodeCbor(checked(), cardano_asset_name_map_to_cbor);
}

std::string
AssetNameMap::toJson() const
{
  return encodeJson(checked(), cardano_asset_name_map_to_cip116_json);
}

std::string
AssetNameMap::toJsonEntries() const
{
  return encodeJson(checked(), cardano_asset_name_map_to_cip116_json_ex);
}

void
AssetNameMap::swap(AssetNameMap& other) noexcept
{
  std::swap(m_value, other.m_value);
}

AssetNameMap&
AssetNameMap::operator=(const AssetNameMap& other)
{
  if (this != &other)
  {
    AssetNameMap copy(other);
    swap(copy);
  }

  return *this;
}

AssetNameMap&
AssetNameMap::operator=(AssetNameMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_asset_name_map_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

AssetNameMap
AssetNameMap::operator+(const AssetNameMap& other) const
{
  const std::vector<std::pair<AssetName, std::int64_t>> right = other.entries();

  for (const std::pair<AssetName, std::int64_t>& entry: right)
  {
    const std::int64_t left = get(entry.first).value_or(0);
    (void)checkedAddAmount(left, entry.second);
  }

  cardano_asset_name_map_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_asset_name_map_add(checked(), other.checked(), &value);
  CardanoObject<cardano_asset_name_map_t, cardano_asset_name_map_unref> owner(value);

  checkCardanoError(status, "AssetNameMap add");
  return AssetNameMap(owner.release());
}

AssetNameMap&
AssetNameMap::operator+=(const AssetNameMap& other)
{
  AssetNameMap result = *this + other;
  swap(result);

  return *this;
}

AssetNameMap
AssetNameMap::operator-(const AssetNameMap& other) const
{
  const std::vector<std::pair<AssetName, std::int64_t>> right = other.entries();

  for (const std::pair<AssetName, std::int64_t>& entry: right)
  {
    const std::int64_t left = get(entry.first).value_or(0);
    (void)checkedSubtractAmount(left, entry.second);
  }

  cardano_asset_name_map_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_asset_name_map_subtract(checked(), other.checked(), &value);
  CardanoObject<cardano_asset_name_map_t, cardano_asset_name_map_unref> owner(value);

  checkCardanoError(status, "AssetNameMap subtract");
  return AssetNameMap(owner.release());
}

AssetNameMap&
AssetNameMap::operator-=(const AssetNameMap& other)
{
  AssetNameMap result = *this - other;
  swap(result);

  return *this;
}

bool
AssetNameMap::operator==(const AssetNameMap& other) const
{
  return cardano_asset_name_map_equals(checked(), other.checked());
}

bool
AssetNameMap::operator!=(const AssetNameMap& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

AssetNameMap::AssetNameMap(cardano_asset_name_map_t* value) noexcept
: m_value(value)
{
}

cardano_asset_name_map_t*
AssetNameMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from AssetNameMap");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AssetNameMap& lhs, AssetNameMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
