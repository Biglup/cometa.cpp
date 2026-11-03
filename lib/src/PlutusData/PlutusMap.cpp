/**
 * @file PlutusMap.cpp
 *
 * @brief Implementation of the PlutusMap value type.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/PlutusData/PlutusMap.h>

#include "../CardanoInterop.h"
#include <Cometa/PlutusData/PlutusList.h>

#include <cardano/plutus_data/plutus_data.h>
#include <cardano/plutus_data/plutus_list.h>
#include <cardano/plutus_data/plutus_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusMap::PlutusMap()
: m_value(nullptr)
{
  cardano_plutus_map_t* value  = nullptr;
  const cardano_error_t result = cardano_plutus_map_new(&value);

  CardanoObject<cardano_plutus_map_t, cardano_plutus_map_unref> owner(value);

  checkCardanoError(result, "PlutusMap");

  m_value = owner.release();
}

PlutusMap::PlutusMap(const PlutusMap& other)
: m_value(nullptr)
{
  cardano_plutus_map_t* value  = nullptr;
  const cardano_error_t result = cardano_plutus_map_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_map_t, cardano_plutus_map_unref> owner(value);

  checkCardanoError(result, "PlutusMap");

  m_value = owner.release();
}

PlutusMap::PlutusMap(PlutusMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

PlutusMap::~PlutusMap()
{
  cardano_plutus_map_unref(&m_value);
}

PlutusMap
PlutusMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusMap(*decodeCbor<cardano_plutus_map_t, cardano_plutus_map_unref>(cbor, cardano_plutus_map_from_cbor));
}

PlutusMap
PlutusMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_plutus_map_get_length(checked());
}

bool
PlutusMap::empty() const
{
  return size() == 0U;
}

std::optional<PlutusData>
PlutusMap::get(const PlutusData& key) const
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_map_get(checked(), key.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "PlutusMap get");

  const PlutusData data(*owner.release());

  return PlutusData(data);
}

void
PlutusMap::insert(const PlutusData& key, const PlutusData& value)
{
  if (m_materialized)
  {
    push_back(value_type(key, value));
    return;
  }

  PlutusData keyCopy(key);
  PlutusData valueCopy(value);
  PlutusMap  replacement(*this);

  checkCardanoError(cardano_plutus_map_insert(replacement.checked(), keyCopy.checked(), valueCopy.checked()), "PlutusMap insert");
  replacement.normalizeEncoding();
  swap(replacement);
}

std::vector<PlutusData>
PlutusMap::keys() const
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_map_get_keys(checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusMap keys");

  const PlutusList list(*owner.release());

  return list.values();
}

std::vector<PlutusData>
PlutusMap::values() const
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_map_get_values(checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusMap values");

  const PlutusList list(*owner.release());

  return list.values();
}

std::vector<std::pair<PlutusData, PlutusData>>
PlutusMap::entries() const
{
  const std::vector<PlutusData>                  keyList   = keys();
  const std::vector<PlutusData>                  valueList = values();
  std::vector<std::pair<PlutusData, PlutusData>> result;

  result.reserve(keyList.size());

  for (std::size_t index = 0U; index < keyList.size(); ++index)
  {
    result.emplace_back(keyList[index], valueList[index]);
  }

  return result;
}

std::vector<std::uint8_t>
PlutusMap::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_map_to_cbor);
}

std::string
PlutusMap::toJson() const
{
  return encodeJson(checked(), cardano_plutus_map_to_cip116_json);
}

void
PlutusMap::normalizeEncoding()
{
  if (m_materialized)
  {
    for (value_type& value: m_elements)
    {
      value.first.normalizeEncoding();
      value.second.normalizeEncoding();
    }
    m_mutableAccess = true;
  }

  cardano_plutus_map_clear_cbor_cache(checked());
}

void
PlutusMap::swap(PlutusMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

PlutusMap&
PlutusMap::operator=(const PlutusMap& other)
{
  if (this != &other)
  {
    PlutusMap copy(other);

    swap(copy);
  }

  return *this;
}

PlutusMap&
PlutusMap::operator=(PlutusMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_map_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
PlutusMap::operator==(const PlutusMap& other) const
{
  return cardano_plutus_map_equals(checked(), other.checked());
}

bool
PlutusMap::operator!=(const PlutusMap& other) const
{
  return !(*this == other);
}

void
PlutusMap::pushBack(const std::pair<PlutusData, PlutusData>& value)
{
  if (m_materialized)
  {
    push_back(value);
    return;
  }

  insert(value.first, value.second);
}

PlutusMap::const_reference
PlutusMap::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

PlutusMap::iterator
PlutusMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

PlutusMap::iterator
PlutusMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

PlutusMap::const_iterator
PlutusMap::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusMap::const_iterator
PlutusMap::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusMap::const_iterator
PlutusMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusMap::const_iterator
PlutusMap::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusMap::reverse_iterator
PlutusMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

PlutusMap::reverse_iterator
PlutusMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

PlutusMap::const_reverse_iterator
PlutusMap::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusMap::const_reverse_iterator
PlutusMap::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusMap::const_reverse_iterator
PlutusMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusMap::const_reverse_iterator
PlutusMap::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusMap::reference
PlutusMap::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

PlutusMap::reference
PlutusMap::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

PlutusMap::const_reference
PlutusMap::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

PlutusMap::reference
PlutusMap::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

PlutusMap::const_reference
PlutusMap::front() const
{
  materialize();

  return m_elements.front();
}

PlutusMap::reference
PlutusMap::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

PlutusMap::const_reference
PlutusMap::back() const
{
  materialize();

  return m_elements.back();
}

void
PlutusMap::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
PlutusMap::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
PlutusMap::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
PlutusMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
PlutusMap::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

PlutusMap::size_type
PlutusMap::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
PlutusMap::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

PlutusMap::iterator
PlutusMap::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

PlutusMap::iterator
PlutusMap::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

PlutusMap::iterator
PlutusMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

PlutusMap::iterator
PlutusMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusMap::PlutusMap(cardano_plutus_map_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_map_t*
PlutusMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusMap");
  }

  synchronize();

  return m_value;
}

void
PlutusMap::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  const std::vector<value_type>          entriesCopy = entries();
  const size_type                        count       = entriesCopy.size();
  std::vector<value_type>                elements;
  std::vector<std::vector<std::uint8_t>> encoding;
  elements.reserve(count);
  encoding.reserve(count);

  for (size_type index = 0; index < count; ++index)
  {
    elements.push_back(entriesCopy[index]);
    encoding.push_back(elements.back().first.toCbor());
    encoding.push_back(elements.back().second.toCbor());
  }

  m_elements.swap(elements);
  m_encoding.swap(encoding);
  m_materialized = true;
}

void
PlutusMap::synchronize() const
{
  if (!m_materialized || !m_mutableAccess)
  {
    return;
  }

  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size());

  for (const value_type& value: m_elements)
  {
    encoding.push_back(value.first.toCbor());
    encoding.push_back(value.second.toCbor());
  }

  if (encoding == m_encoding)
  {
    return;
  }

  const std::vector<std::uint8_t> previous    = encodeCbor(m_value, cardano_plutus_map_to_cbor);
  PlutusMap                       replacement = !previous.empty() && previous.front() == 0xbf
                          ? PlutusMap::fromCbor("bfff")
                          : PlutusMap();

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

std::pair<PlutusData, PlutusData>
PlutusMap::copyElement(std::size_t index) const
{
  return entries().at(index);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusMap& lhs, PlutusMap& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
