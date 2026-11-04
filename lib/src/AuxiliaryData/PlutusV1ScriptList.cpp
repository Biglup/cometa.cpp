/**
 * @file PlutusV1ScriptList.cpp
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

#include <Cometa/AuxiliaryData/PlutusV1ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV1Script.h>

#include <cardano/auxiliary_data/plutus_v1_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v1_script.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV1ScriptList::PlutusV1ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v1_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v1_script_list_new(&value);

  CardanoObject<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV1ScriptList");

  m_value = owner.release();
}

PlutusV1ScriptList::PlutusV1ScriptList(std::initializer_list<PlutusV1Script> scripts)
: PlutusV1ScriptList()
{
  for (const PlutusV1Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV1ScriptList::PlutusV1ScriptList(const PlutusV1ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v1_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v1_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV1ScriptList");

  m_value = owner.release();
}

PlutusV1ScriptList::PlutusV1ScriptList(PlutusV1ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

PlutusV1ScriptList::~PlutusV1ScriptList()
{
  cardano_plutus_v1_script_list_unref(&m_value);
}

PlutusV1ScriptList
PlutusV1ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV1ScriptList value(*decodeCbor<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref>(cbor, cardano_plutus_v1_script_list_from_cbor));

  return value;
}

PlutusV1ScriptList
PlutusV1ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV1ScriptList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_plutus_v1_script_list_get_length(checked());
}

bool
PlutusV1ScriptList::empty() const
{
  return size() == 0U;
}

void
PlutusV1ScriptList::pushBack(const PlutusV1Script& script)
{
  if (m_materialized)
  {
    push_back(script);
    return;
  }

  PlutusV1Script copy(script);

  checkCardanoError(cardano_plutus_v1_script_list_add(checked(), checkedNativeHandle(copy)), "PlutusV1ScriptList pushBack");
}

std::vector<PlutusV1Script>
PlutusV1ScriptList::values() const
{
  std::vector<PlutusV1Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV1ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v1_script_list_to_cbor);
}

std::string
PlutusV1ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v1_script_list_to_cip116_json);
}

void
PlutusV1ScriptList::swap(PlutusV1ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

PlutusV1ScriptList&
PlutusV1ScriptList::operator=(const PlutusV1ScriptList& other)
{
  if (this != &other)
  {
    PlutusV1ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV1ScriptList&
PlutusV1ScriptList::operator=(PlutusV1ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v1_script_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
PlutusV1ScriptList::operator==(const PlutusV1ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV1ScriptList::operator!=(const PlutusV1ScriptList& other) const
{
  return !(*this == other);
}

PlutusV1ScriptList::const_reference
PlutusV1ScriptList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

PlutusV1ScriptList::iterator
PlutusV1ScriptList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

PlutusV1ScriptList::iterator
PlutusV1ScriptList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

PlutusV1ScriptList::const_iterator
PlutusV1ScriptList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV1ScriptList::const_iterator
PlutusV1ScriptList::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusV1ScriptList::const_iterator
PlutusV1ScriptList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV1ScriptList::const_iterator
PlutusV1ScriptList::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusV1ScriptList::reverse_iterator
PlutusV1ScriptList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

PlutusV1ScriptList::reverse_iterator
PlutusV1ScriptList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

PlutusV1ScriptList::const_reverse_iterator
PlutusV1ScriptList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV1ScriptList::const_reverse_iterator
PlutusV1ScriptList::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV1ScriptList::const_reverse_iterator
PlutusV1ScriptList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV1ScriptList::const_reverse_iterator
PlutusV1ScriptList::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV1ScriptList::reference
PlutusV1ScriptList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

PlutusV1ScriptList::reference
PlutusV1ScriptList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

PlutusV1ScriptList::const_reference
PlutusV1ScriptList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

PlutusV1ScriptList::reference
PlutusV1ScriptList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

PlutusV1ScriptList::const_reference
PlutusV1ScriptList::front() const
{
  materialize();

  return m_elements.front();
}

PlutusV1ScriptList::reference
PlutusV1ScriptList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

PlutusV1ScriptList::const_reference
PlutusV1ScriptList::back() const
{
  materialize();

  return m_elements.back();
}

void
PlutusV1ScriptList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
PlutusV1ScriptList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
PlutusV1ScriptList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
PlutusV1ScriptList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
PlutusV1ScriptList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

PlutusV1ScriptList::size_type
PlutusV1ScriptList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
PlutusV1ScriptList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

PlutusV1ScriptList::iterator
PlutusV1ScriptList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

PlutusV1ScriptList::iterator
PlutusV1ScriptList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

PlutusV1ScriptList::iterator
PlutusV1ScriptList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

PlutusV1ScriptList::iterator
PlutusV1ScriptList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

PlutusV1ScriptList
PlutusV1ScriptList::adopt(cardano_plutus_v1_script_list_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PlutusV1ScriptList");
  }

  return PlutusV1ScriptList(*value);
}

cardano_plutus_v1_script_list_t*
PlutusV1ScriptList::nativeHandle()
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

const cardano_plutus_v1_script_list_t*
PlutusV1ScriptList::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_plutus_v1_script_list_t*
PlutusV1ScriptList::release()
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

PlutusV1ScriptList::PlutusV1ScriptList(cardano_plutus_v1_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v1_script_list_t*
PlutusV1ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV1ScriptList");
  }

  synchronize();

  return m_value;
}

void
PlutusV1ScriptList::materialize() const
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
PlutusV1ScriptList::synchronize() const
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

  PlutusV1ScriptList replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

PlutusV1Script
PlutusV1ScriptList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV1ScriptList index");
  }

  cardano_plutus_v1_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v1_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v1_script_t, cardano_plutus_v1_script_unref> owner(value);

  checkCardanoError(result, "PlutusV1ScriptList at");

  const PlutusV1Script script = PlutusV1Script::adopt(owner.release());

  return PlutusV1Script(script);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV1ScriptList& lhs, PlutusV1ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
