/**
 * @file MetadatumMap.cpp
 *
 * @brief Implementation of the MetadatumMap value type.
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

#include <Cometa/AuxiliaryData/MetadatumMap.h>

#include "../CardanoInterop.h"
#include <Cometa/AuxiliaryData/MetadatumList.h>

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/metadatum_list.h>
#include <cardano/auxiliary_data/metadatum_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MetadatumMap::MetadatumMap()
: m_value(nullptr)
{
  cardano_metadatum_map_t* value  = nullptr;
  const cardano_error_t    result = cardano_metadatum_map_new(&value);

  CardanoObject<cardano_metadatum_map_t, cardano_metadatum_map_unref> owner(value);

  checkCardanoError(result, "MetadatumMap");

  m_value = owner.release();
}

MetadatumMap::MetadatumMap(const MetadatumMap& other)
: m_value(nullptr)
{
  cardano_metadatum_map_t* value  = nullptr;
  const cardano_error_t    result = cardano_metadatum_map_deep_clone(other.checked(), &value);

  CardanoObject<cardano_metadatum_map_t, cardano_metadatum_map_unref> owner(value);

  checkCardanoError(result, "MetadatumMap");

  m_value = owner.release();
}

MetadatumMap::MetadatumMap(MetadatumMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

MetadatumMap::~MetadatumMap()
{
  cardano_metadatum_map_unref(&m_value);
}

MetadatumMap
MetadatumMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return MetadatumMap(*decodeCbor<cardano_metadatum_map_t, cardano_metadatum_map_unref>(cbor, cardano_metadatum_map_from_cbor));
}

MetadatumMap
MetadatumMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
MetadatumMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_metadatum_map_get_length(checked());
}

bool
MetadatumMap::empty() const
{
  return size() == 0U;
}

std::optional<Metadatum>
MetadatumMap::get(const Metadatum& key) const
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_map_get(checked(), key.checked(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "MetadatumMap get");

  const Metadatum data(*owner.release());

  return Metadatum(data);
}

void
MetadatumMap::insert(const Metadatum& key, const Metadatum& value)
{
  if (m_materialized)
  {
    push_back(value_type(key, value));
    return;
  }

  Metadatum keyCopy(key);
  Metadatum valueCopy(value);

  checkCardanoError(cardano_metadatum_map_insert(checked(), keyCopy.checked(), valueCopy.checked()), "MetadatumMap insert");
}

std::vector<Metadatum>
MetadatumMap::keys() const
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_map_get_keys(checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumMap keys");

  const MetadatumList list(*owner.release());

  return list.values();
}

std::vector<Metadatum>
MetadatumMap::values() const
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_map_get_values(checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumMap values");

  const MetadatumList list(*owner.release());

  return list.values();
}

std::vector<std::pair<Metadatum, Metadatum>>
MetadatumMap::entries() const
{
  const std::vector<Metadatum>                 keyList   = keys();
  const std::vector<Metadatum>                 valueList = values();
  std::vector<std::pair<Metadatum, Metadatum>> result;

  result.reserve(keyList.size());

  for (std::size_t index = 0U; index < keyList.size(); ++index)
  {
    result.emplace_back(keyList[index], valueList[index]);
  }

  return result;
}

std::pair<Metadatum, Metadatum>
MetadatumMap::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("MetadatumMap index");
  }

  cardano_metadatum_t*  key    = nullptr;
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_map_get_at(checked(), index, &key, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> keyOwner(key);
  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> valueOwner(value);

  checkCardanoError(result, "MetadatumMap entryAt");

  const Metadatum keyValue(*keyOwner.release());
  const Metadatum dataValue(*valueOwner.release());

  return std::make_pair(Metadatum(keyValue), Metadatum(dataValue));
}

std::vector<std::uint8_t>
MetadatumMap::toCbor() const
{
  return encodeCbor(checked(), cardano_metadatum_map_to_cbor);
}

std::string
MetadatumMap::toJson() const
{
  return encodeJson(checked(), cardano_metadatum_map_to_cip116_json);
}

void
MetadatumMap::swap(MetadatumMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

MetadatumMap&
MetadatumMap::operator=(const MetadatumMap& other)
{
  if (this != &other)
  {
    MetadatumMap copy(other);

    swap(copy);
  }

  return *this;
}

MetadatumMap&
MetadatumMap::operator=(MetadatumMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_metadatum_map_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
MetadatumMap::operator==(const MetadatumMap& other) const
{
  return cardano_metadatum_map_equals(checked(), other.checked());
}

bool
MetadatumMap::operator!=(const MetadatumMap& other) const
{
  return !(*this == other);
}

void
MetadatumMap::pushBack(const std::pair<Metadatum, Metadatum>& value)
{
  if (m_materialized)
  {
    push_back(value);
    return;
  }

  insert(value.first, value.second);
}

MetadatumMap::const_reference
MetadatumMap::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

MetadatumMap::iterator
MetadatumMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

MetadatumMap::iterator
MetadatumMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

MetadatumMap::const_iterator
MetadatumMap::begin() const
{
  materialize();

  return m_elements.cbegin();
}

MetadatumMap::const_iterator
MetadatumMap::end() const
{
  materialize();

  return m_elements.cend();
}

MetadatumMap::const_iterator
MetadatumMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

MetadatumMap::const_iterator
MetadatumMap::cend() const
{
  materialize();

  return m_elements.cend();
}

MetadatumMap::reverse_iterator
MetadatumMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

MetadatumMap::reverse_iterator
MetadatumMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

MetadatumMap::const_reverse_iterator
MetadatumMap::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

MetadatumMap::const_reverse_iterator
MetadatumMap::rend() const
{
  materialize();

  return m_elements.crend();
}

MetadatumMap::const_reverse_iterator
MetadatumMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

MetadatumMap::const_reverse_iterator
MetadatumMap::crend() const
{
  materialize();

  return m_elements.crend();
}

MetadatumMap::reference
MetadatumMap::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

MetadatumMap::reference
MetadatumMap::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

MetadatumMap::const_reference
MetadatumMap::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

MetadatumMap::reference
MetadatumMap::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

MetadatumMap::const_reference
MetadatumMap::front() const
{
  materialize();

  return m_elements.front();
}

MetadatumMap::reference
MetadatumMap::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

MetadatumMap::const_reference
MetadatumMap::back() const
{
  materialize();

  return m_elements.back();
}

void
MetadatumMap::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
MetadatumMap::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
MetadatumMap::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
MetadatumMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
MetadatumMap::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

MetadatumMap::size_type
MetadatumMap::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
MetadatumMap::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

MetadatumMap::iterator
MetadatumMap::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

MetadatumMap::iterator
MetadatumMap::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

MetadatumMap::iterator
MetadatumMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

MetadatumMap::iterator
MetadatumMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

/* PRIVATE FUNCTIONS *********************************************************/

MetadatumMap::MetadatumMap(cardano_metadatum_map_t& value) noexcept
: m_value(&value)
{
}

cardano_metadatum_map_t*
MetadatumMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MetadatumMap");
  }

  synchronize();

  return m_value;
}

void
MetadatumMap::materialize() const
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
MetadatumMap::synchronize() const
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

  const std::vector<std::uint8_t> previous    = encodeCbor(m_value, cardano_metadatum_map_to_cbor);
  MetadatumMap                    replacement = !previous.empty() && previous.front() == 0xbf
                       ? MetadatumMap::fromCbor("bfff")
                       : MetadatumMap();

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

std::pair<Metadatum, Metadatum>
MetadatumMap::copyElement(std::size_t index) const
{
  return entries().at(index);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MetadatumMap& lhs, MetadatumMap& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
