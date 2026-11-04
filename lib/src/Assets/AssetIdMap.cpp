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

#include <utility>

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
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

AssetIdMap::~AssetIdMap()
{
  cardano_asset_id_map_unref(&m_value);
}

std::size_t
AssetIdMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

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
  const cardano_error_t result = cardano_asset_id_map_get(checked(), checkedNativeHandle(key), &amount);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "AssetIdMap get");
  return amount;
}

void
AssetIdMap::set(const AssetId& key, std::int64_t amount)
{
  const AssetId copy(key);

  checkCardanoError(cardano_asset_id_map_insert(checked(), checkedNativeHandle(copy), amount), "AssetIdMap set");
  invalidateElements();
}

AssetIdList
AssetIdMap::keys() const
{
  AssetIdList       result;
  const std::size_t elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_asset_id_t*                                       key    = nullptr;
    const cardano_error_t                                     status = cardano_asset_id_map_get_key_at(checked(), i, &key);
    CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(key);

    checkCardanoError(status, "AssetIdMap keys");
    const AssetId value = AssetId::adopt(owner.release());
    result.push_back(value);
  }

  return result;
}

std::vector<std::pair<AssetId, std::int64_t>>
AssetIdMap::entries() const
{
  std::vector<std::pair<AssetId, std::int64_t>> result;
  const std::size_t                             elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_asset_id_t*                                       key    = nullptr;
    std::int64_t                                              amount = 0;
    const cardano_error_t                                     status = cardano_asset_id_map_get_key_value_at(checked(), i, &key, &amount);
    CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(key);

    checkCardanoError(status, "AssetIdMap entries");
    const AssetId value = AssetId::adopt(owner.release());
    result.emplace_back(value, amount);
  }

  return result;
}

void
AssetIdMap::swap(AssetIdMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
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
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
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

AssetIdMap::iterator
AssetIdMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

AssetIdMap::const_iterator
AssetIdMap::begin() const
{
  materialize();

  return m_elements.begin();
}

AssetIdMap::iterator
AssetIdMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

AssetIdMap::const_iterator
AssetIdMap::end() const
{
  materialize();

  return m_elements.end();
}

AssetIdMap::reverse_iterator
AssetIdMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

AssetIdMap::const_reverse_iterator
AssetIdMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

AssetIdMap::reverse_iterator
AssetIdMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

AssetIdMap::const_reverse_iterator
AssetIdMap::rend() const
{
  materialize();

  return m_elements.rend();
}

AssetIdMap::const_iterator
AssetIdMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

AssetIdMap::const_iterator
AssetIdMap::cend() const
{
  materialize();

  return m_elements.cend();
}

AssetIdMap::const_reverse_iterator
AssetIdMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

AssetIdMap::const_reverse_iterator
AssetIdMap::crend() const
{
  materialize();

  return m_elements.crend();
}

AssetIdMap::iterator
AssetIdMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

AssetIdMap::const_iterator
AssetIdMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

AssetIdMap::size_type
AssetIdMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
AssetIdMap::contains(const key_type& key) const
{
  return count(key) != 0;
}

AssetIdMap::mapped_type&
AssetIdMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const AssetIdMap::mapped_type&
AssetIdMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<AssetIdMap::iterator, bool>
AssetIdMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  AssetIdMap validation;
  validation.set(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

AssetIdMap::iterator
AssetIdMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<AssetIdMap::iterator, bool>
AssetIdMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  AssetIdMap validation;
  validation.set(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

AssetIdMap::mapped_type&
AssetIdMap::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

AssetIdMap::iterator
AssetIdMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

AssetIdMap::iterator
AssetIdMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

AssetIdMap::size_type
AssetIdMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
AssetIdMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

AssetIdMap
AssetIdMap::adopt(cardano_asset_id_map_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null AssetIdMap");
  }

  return AssetIdMap(value);
}

cardano_asset_id_map_t*
AssetIdMap::nativeHandle()
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

const cardano_asset_id_map_t*
AssetIdMap::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_asset_id_map_t*
AssetIdMap::release()
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

  synchronize();

  return m_value;
}

void
AssetIdMap::materialize() const
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
AssetIdMap::synchronize() const
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

  AssetIdMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.set(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
AssetIdMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
AssetIdMap::encodeElements() const
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
AssetIdMap::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AssetIdMap& lhs, AssetIdMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
