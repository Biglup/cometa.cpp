/**
 * @file PlutusList.cpp
 *
 * @brief Implementation of the PlutusList value type.
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

#include <Cometa/PlutusData/PlutusList.h>

#include "../CardanoInterop.h"

#include <cardano/plutus_data/plutus_data.h>
#include <cardano/plutus_data/plutus_list.h>

#include <algorithm>
#include <functional>
#include <iterator>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusList::PlutusList()
: m_value(nullptr)
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_list_new(&value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusList");

  m_value = owner.release();
}

PlutusList::PlutusList(const PlutusList& other)
: m_value(nullptr)
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusList");

  m_value = owner.release();
}

PlutusList::PlutusList(PlutusList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

PlutusList::~PlutusList()
{
  cardano_plutus_list_unref(&m_value);
}

PlutusList
PlutusList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusList(*decodeCbor<cardano_plutus_list_t, cardano_plutus_list_unref>(cbor, cardano_plutus_list_from_cbor));
}

PlutusList
PlutusList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_plutus_list_get_length(checked());
}

bool
PlutusList::empty() const
{
  return size() == 0U;
}

void
PlutusList::pushBack(const PlutusData& data)
{
  if (m_materialized)
  {
    push_back(data);
    return;
  }

  PlutusData copy(data);

  checkCardanoError(cardano_plutus_list_add(checked(), copy.checked()), "PlutusList pushBack");
  normalizeEncoding();
}

std::vector<PlutusData>
PlutusList::values() const
{
  std::vector<PlutusData> result;
  const std::size_t       count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_list_to_cbor);
}

std::string
PlutusList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_list_to_cip116_json);
}

void
PlutusList::normalizeEncoding()
{
  if (m_materialized)
  {
    for (value_type& value: m_elements)
    {
      value.normalizeEncoding();
    }
    m_mutableAccess = true;
  }

  cardano_plutus_list_clear_cbor_cache(checked());
}

void
PlutusList::swap(PlutusList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

PlutusList&
PlutusList::operator=(const PlutusList& other)
{
  if (this != &other)
  {
    PlutusList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusList&
PlutusList::operator=(PlutusList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
PlutusList::operator==(const PlutusList& other) const
{
  return cardano_plutus_list_equals(checked(), other.checked());
}

bool
PlutusList::operator!=(const PlutusList& other) const
{
  return !(*this == other);
}

PlutusList::const_reference
PlutusList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

PlutusList::iterator
PlutusList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

PlutusList::iterator
PlutusList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

PlutusList::const_iterator
PlutusList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusList::const_iterator
PlutusList::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusList::const_iterator
PlutusList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusList::const_iterator
PlutusList::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusList::reverse_iterator
PlutusList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

PlutusList::reverse_iterator
PlutusList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

PlutusList::const_reverse_iterator
PlutusList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusList::const_reverse_iterator
PlutusList::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusList::const_reverse_iterator
PlutusList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusList::const_reverse_iterator
PlutusList::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusList::reference
PlutusList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

PlutusList::reference
PlutusList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

PlutusList::const_reference
PlutusList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

PlutusList::reference
PlutusList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

PlutusList::const_reference
PlutusList::front() const
{
  materialize();

  return m_elements.front();
}

PlutusList::reference
PlutusList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

PlutusList::const_reference
PlutusList::back() const
{
  materialize();

  return m_elements.back();
}

void
PlutusList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
PlutusList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
PlutusList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
PlutusList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
PlutusList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

PlutusList::size_type
PlutusList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
PlutusList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

PlutusList::iterator
PlutusList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

PlutusList::iterator
PlutusList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

PlutusList::iterator
PlutusList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

PlutusList::iterator
PlutusList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusList::PlutusList(cardano_plutus_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_list_t*
PlutusList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusList");
  }

  synchronize();

  return m_value;
}

void
PlutusList::materialize() const
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
PlutusList::synchronize() const
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

  PlutusList replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

PlutusData
PlutusList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusList index");
  }

  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusList at");

  const PlutusData data(*owner.release());

  return PlutusData(data);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusList& lhs, PlutusList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
