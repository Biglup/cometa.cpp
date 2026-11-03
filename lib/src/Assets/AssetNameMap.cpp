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
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
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
  if (m_materialized)
  {
    return m_elements.size();
  }

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

void
AssetNameMap::set(const AssetName& key, std::int64_t amount)
{
  const AssetName copy(key);

  checkCardanoError(cardano_asset_name_map_insert(checked(), copy.checked(), amount), "AssetNameMap set");
  invalidateElements();
}

AssetNameList
AssetNameMap::keys() const
{
  AssetNameList     result;
  const std::size_t elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
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
  const std::size_t                               elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
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
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
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
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
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

AssetNameMap::iterator
AssetNameMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

AssetNameMap::const_iterator
AssetNameMap::begin() const
{
  materialize();

  return m_elements.begin();
}

AssetNameMap::iterator
AssetNameMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

AssetNameMap::const_iterator
AssetNameMap::end() const
{
  materialize();

  return m_elements.end();
}

AssetNameMap::reverse_iterator
AssetNameMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

AssetNameMap::const_reverse_iterator
AssetNameMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

AssetNameMap::reverse_iterator
AssetNameMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

AssetNameMap::const_reverse_iterator
AssetNameMap::rend() const
{
  materialize();

  return m_elements.rend();
}

AssetNameMap::const_iterator
AssetNameMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

AssetNameMap::const_iterator
AssetNameMap::cend() const
{
  materialize();

  return m_elements.cend();
}

AssetNameMap::const_reverse_iterator
AssetNameMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

AssetNameMap::const_reverse_iterator
AssetNameMap::crend() const
{
  materialize();

  return m_elements.crend();
}

AssetNameMap::iterator
AssetNameMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

AssetNameMap::const_iterator
AssetNameMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

AssetNameMap::size_type
AssetNameMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
AssetNameMap::contains(const key_type& key) const
{
  return count(key) != 0;
}

AssetNameMap::mapped_type&
AssetNameMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const AssetNameMap::mapped_type&
AssetNameMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<AssetNameMap::iterator, bool>
AssetNameMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  AssetNameMap validation;
  validation.set(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

AssetNameMap::iterator
AssetNameMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<AssetNameMap::iterator, bool>
AssetNameMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  AssetNameMap validation;
  validation.set(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

AssetNameMap::mapped_type&
AssetNameMap::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

AssetNameMap::iterator
AssetNameMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

AssetNameMap::iterator
AssetNameMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

AssetNameMap::size_type
AssetNameMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
AssetNameMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
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

  synchronize();

  return m_value;
}

void
AssetNameMap::materialize() const
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
AssetNameMap::synchronize() const
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

  AssetNameMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.set(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
AssetNameMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
AssetNameMap::encodeElements() const
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
AssetNameMap::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(AssetNameMap& lhs, AssetNameMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
