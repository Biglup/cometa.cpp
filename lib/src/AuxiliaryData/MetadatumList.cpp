/**
 * @file MetadatumList.cpp
 *
 * @brief Implementation of the MetadatumList value type.
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

#include <Cometa/AuxiliaryData/MetadatumList.h>

#include "../CardanoInterop.h"

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/metadatum_list.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MetadatumList::MetadatumList()
: m_value(nullptr)
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_list_new(&value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumList");

  m_value = owner.release();
}

MetadatumList::MetadatumList(std::initializer_list<Metadatum> values)
: MetadatumList()
{
  for (const Metadatum& value: values)
  {
    pushBack(value);
  }
}

MetadatumList::MetadatumList(const MetadatumList& other)
: m_value(nullptr)
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumList");

  m_value = owner.release();
}

MetadatumList::MetadatumList(MetadatumList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

MetadatumList::~MetadatumList()
{
  cardano_metadatum_list_unref(&m_value);
}

MetadatumList
MetadatumList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return MetadatumList(*decodeCbor<cardano_metadatum_list_t, cardano_metadatum_list_unref>(cbor, cardano_metadatum_list_from_cbor));
}

MetadatumList
MetadatumList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
MetadatumList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_metadatum_list_get_length(checked());
}

bool
MetadatumList::empty() const
{
  return size() == 0U;
}

void
MetadatumList::pushBack(const Metadatum& data)
{
  if (m_materialized)
  {
    push_back(data);
    return;
  }

  Metadatum copy(data);

  checkCardanoError(cardano_metadatum_list_add(checked(), checkedNativeHandle(copy)), "MetadatumList pushBack");
}

std::vector<Metadatum>
MetadatumList::values() const
{
  std::vector<Metadatum> result;
  const std::size_t      count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
MetadatumList::toCbor() const
{
  return encodeCbor(checked(), cardano_metadatum_list_to_cbor);
}

std::string
MetadatumList::toJson() const
{
  return encodeJson(checked(), cardano_metadatum_list_to_cip116_json);
}

void
MetadatumList::swap(MetadatumList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

MetadatumList&
MetadatumList::operator=(const MetadatumList& other)
{
  if (this != &other)
  {
    MetadatumList copy(other);

    swap(copy);
  }

  return *this;
}

MetadatumList&
MetadatumList::operator=(MetadatumList&& other) noexcept
{
  if (this != &other)
  {
    cardano_metadatum_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
MetadatumList::operator==(const MetadatumList& other) const
{
  return cardano_metadatum_list_equals(checked(), other.checked());
}

bool
MetadatumList::operator!=(const MetadatumList& other) const
{
  return !(*this == other);
}

MetadatumList::const_reference
MetadatumList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

MetadatumList::iterator
MetadatumList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

MetadatumList::iterator
MetadatumList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

MetadatumList::const_iterator
MetadatumList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

MetadatumList::const_iterator
MetadatumList::end() const
{
  materialize();

  return m_elements.cend();
}

MetadatumList::const_iterator
MetadatumList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

MetadatumList::const_iterator
MetadatumList::cend() const
{
  materialize();

  return m_elements.cend();
}

MetadatumList::reverse_iterator
MetadatumList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

MetadatumList::reverse_iterator
MetadatumList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

MetadatumList::const_reverse_iterator
MetadatumList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

MetadatumList::const_reverse_iterator
MetadatumList::rend() const
{
  materialize();

  return m_elements.crend();
}

MetadatumList::const_reverse_iterator
MetadatumList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

MetadatumList::const_reverse_iterator
MetadatumList::crend() const
{
  materialize();

  return m_elements.crend();
}

MetadatumList::reference
MetadatumList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

MetadatumList::reference
MetadatumList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

MetadatumList::const_reference
MetadatumList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

MetadatumList::reference
MetadatumList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

MetadatumList::const_reference
MetadatumList::front() const
{
  materialize();

  return m_elements.front();
}

MetadatumList::reference
MetadatumList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

MetadatumList::const_reference
MetadatumList::back() const
{
  materialize();

  return m_elements.back();
}

void
MetadatumList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
MetadatumList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
MetadatumList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
MetadatumList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
MetadatumList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

MetadatumList::size_type
MetadatumList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
MetadatumList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

MetadatumList::iterator
MetadatumList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

MetadatumList::iterator
MetadatumList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

MetadatumList::iterator
MetadatumList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

MetadatumList::iterator
MetadatumList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

MetadatumList
MetadatumList::adopt(cardano_metadatum_list_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null MetadatumList");
  }

  return MetadatumList(*value);
}

cardano_metadatum_list_t*
MetadatumList::nativeHandle()
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

const cardano_metadatum_list_t*
MetadatumList::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_metadatum_list_t*
MetadatumList::release()
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

MetadatumList::MetadatumList(cardano_metadatum_list_t& value) noexcept
: m_value(&value)
{
}

cardano_metadatum_list_t*
MetadatumList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MetadatumList");
  }

  synchronize();

  return m_value;
}

void
MetadatumList::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  const size_type                        count = size();
  std::vector<value_type>                elements;
  std::vector<std::vector<std::uint8_t>> encoding;
  elements.reserve(count);
  encoding.reserve(count);

  for (size_type index = 0; index < count; ++index)
  {
    elements.push_back(copyElement(index));
    encoding.push_back(elements.back().toCbor());
  }

  m_elements.swap(elements);
  m_encoding.swap(encoding);
  m_materialized = true;
}

void
MetadatumList::synchronize() const
{
  if (!m_materialized || !m_mutableAccess)
  {
    return;
  }

  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size());

  std::transform(
    m_elements.cbegin(),
    m_elements.cend(),
    std::back_inserter(encoding),
    std::mem_fn(&value_type::toCbor));

  if (encoding == m_encoding)
  {
    return;
  }

  MetadatumList replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

Metadatum
MetadatumList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("MetadatumList index");
  }

  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_list_get(checked(), index, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "MetadatumList at");

  const Metadatum data = Metadatum::adopt(owner.release());

  return Metadatum(data);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MetadatumList& lhs, MetadatumList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
