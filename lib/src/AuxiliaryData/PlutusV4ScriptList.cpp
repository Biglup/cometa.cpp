/**
 * @file PlutusV4ScriptList.cpp
 *
 * @brief An ordered sequence of auxiliary Plutus scripts.
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

#include <Cometa/AuxiliaryData/PlutusV4ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>

#include <cardano/auxiliary_data/plutus_v4_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v4_script.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV4ScriptList::PlutusV4ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v4_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v4_script_list_new(&value);

  CardanoObject<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV4ScriptList");

  m_value = owner.release();
}

PlutusV4ScriptList::PlutusV4ScriptList(std::initializer_list<PlutusV4Script> scripts)
: PlutusV4ScriptList()
{
  for (const PlutusV4Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV4ScriptList::PlutusV4ScriptList(const PlutusV4ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v4_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v4_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV4ScriptList");

  m_value = owner.release();
}

PlutusV4ScriptList::PlutusV4ScriptList(PlutusV4ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

PlutusV4ScriptList::~PlutusV4ScriptList()
{
  cardano_plutus_v4_script_list_unref(&m_value);
}

PlutusV4ScriptList
PlutusV4ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV4ScriptList value(*decodeCbor<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref>(cbor, cardano_plutus_v4_script_list_from_cbor));

  return value;
}

PlutusV4ScriptList
PlutusV4ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV4ScriptList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_plutus_v4_script_list_get_length(checked());
}

bool
PlutusV4ScriptList::empty() const
{
  return size() == 0U;
}

void
PlutusV4ScriptList::pushBack(const PlutusV4Script& script)
{
  if (m_materialized)
  {
    push_back(script);
    return;
  }

  PlutusV4Script copy(script);

  checkCardanoError(cardano_plutus_v4_script_list_add(checked(), checkedNativeHandle(copy)), "PlutusV4ScriptList pushBack");
}

std::vector<PlutusV4Script>
PlutusV4ScriptList::values() const
{
  std::vector<PlutusV4Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV4ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v4_script_list_to_cbor);
}

std::string
PlutusV4ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v4_script_list_to_cip116_json);
}

void
PlutusV4ScriptList::swap(PlutusV4ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

PlutusV4ScriptList&
PlutusV4ScriptList::operator=(const PlutusV4ScriptList& other)
{
  if (this != &other)
  {
    PlutusV4ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV4ScriptList&
PlutusV4ScriptList::operator=(PlutusV4ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v4_script_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
PlutusV4ScriptList::operator==(const PlutusV4ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV4ScriptList::operator!=(const PlutusV4ScriptList& other) const
{
  return !(*this == other);
}

PlutusV4ScriptList::const_reference
PlutusV4ScriptList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

PlutusV4ScriptList::iterator
PlutusV4ScriptList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

PlutusV4ScriptList::iterator
PlutusV4ScriptList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

PlutusV4ScriptList::const_iterator
PlutusV4ScriptList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV4ScriptList::const_iterator
PlutusV4ScriptList::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusV4ScriptList::const_iterator
PlutusV4ScriptList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV4ScriptList::const_iterator
PlutusV4ScriptList::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusV4ScriptList::reverse_iterator
PlutusV4ScriptList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

PlutusV4ScriptList::reverse_iterator
PlutusV4ScriptList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

PlutusV4ScriptList::const_reverse_iterator
PlutusV4ScriptList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV4ScriptList::const_reverse_iterator
PlutusV4ScriptList::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV4ScriptList::const_reverse_iterator
PlutusV4ScriptList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV4ScriptList::const_reverse_iterator
PlutusV4ScriptList::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV4ScriptList::reference
PlutusV4ScriptList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

PlutusV4ScriptList::reference
PlutusV4ScriptList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

PlutusV4ScriptList::const_reference
PlutusV4ScriptList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

PlutusV4ScriptList::reference
PlutusV4ScriptList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

PlutusV4ScriptList::const_reference
PlutusV4ScriptList::front() const
{
  materialize();

  return m_elements.front();
}

PlutusV4ScriptList::reference
PlutusV4ScriptList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

PlutusV4ScriptList::const_reference
PlutusV4ScriptList::back() const
{
  materialize();

  return m_elements.back();
}

void
PlutusV4ScriptList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
PlutusV4ScriptList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
PlutusV4ScriptList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
PlutusV4ScriptList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
PlutusV4ScriptList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

PlutusV4ScriptList::size_type
PlutusV4ScriptList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
PlutusV4ScriptList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

PlutusV4ScriptList::iterator
PlutusV4ScriptList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

PlutusV4ScriptList::iterator
PlutusV4ScriptList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

PlutusV4ScriptList::iterator
PlutusV4ScriptList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

PlutusV4ScriptList::iterator
PlutusV4ScriptList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

PlutusV4ScriptList
PlutusV4ScriptList::adopt(cardano_plutus_v4_script_list_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PlutusV4ScriptList");
  }

  return PlutusV4ScriptList(*value);
}

cardano_plutus_v4_script_list_t*
PlutusV4ScriptList::nativeHandle()
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

const cardano_plutus_v4_script_list_t*
PlutusV4ScriptList::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_plutus_v4_script_list_t*
PlutusV4ScriptList::release()
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

PlutusV4ScriptList::PlutusV4ScriptList(cardano_plutus_v4_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v4_script_list_t*
PlutusV4ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV4ScriptList");
  }

  synchronize();

  return m_value;
}

void
PlutusV4ScriptList::materialize() const
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
PlutusV4ScriptList::synchronize() const
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

  PlutusV4ScriptList replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

PlutusV4Script
PlutusV4ScriptList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV4ScriptList index");
  }

  cardano_plutus_v4_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v4_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(result, "PlutusV4ScriptList at");

  const PlutusV4Script script = PlutusV4Script::adopt(owner.release());

  return PlutusV4Script(script);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV4ScriptList& lhs, PlutusV4ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
