/**
 * @file PlutusV3ScriptList.cpp
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

#include <Cometa/AuxiliaryData/PlutusV3ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV3Script.h>

#include <cardano/auxiliary_data/plutus_v3_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v3_script.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV3ScriptList::PlutusV3ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v3_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v3_script_list_new(&value);

  CardanoObject<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV3ScriptList");

  m_value = owner.release();
}

PlutusV3ScriptList::PlutusV3ScriptList(std::initializer_list<PlutusV3Script> scripts)
: PlutusV3ScriptList()
{
  for (const PlutusV3Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV3ScriptList::PlutusV3ScriptList(const PlutusV3ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v3_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v3_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV3ScriptList");

  m_value = owner.release();
}

PlutusV3ScriptList::PlutusV3ScriptList(PlutusV3ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

PlutusV3ScriptList::~PlutusV3ScriptList()
{
  cardano_plutus_v3_script_list_unref(&m_value);
}

PlutusV3ScriptList
PlutusV3ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV3ScriptList value(*decodeCbor<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref>(cbor, cardano_plutus_v3_script_list_from_cbor));

  return value;
}

PlutusV3ScriptList
PlutusV3ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV3ScriptList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_plutus_v3_script_list_get_length(checked());
}

bool
PlutusV3ScriptList::empty() const
{
  return size() == 0U;
}

void
PlutusV3ScriptList::pushBack(const PlutusV3Script& script)
{
  if (m_materialized)
  {
    push_back(script);
    return;
  }

  PlutusV3Script copy(script);

  checkCardanoError(cardano_plutus_v3_script_list_add(checked(), checkedNativeHandle(copy)), "PlutusV3ScriptList pushBack");
}

std::vector<PlutusV3Script>
PlutusV3ScriptList::values() const
{
  std::vector<PlutusV3Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV3ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v3_script_list_to_cbor);
}

std::string
PlutusV3ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v3_script_list_to_cip116_json);
}

void
PlutusV3ScriptList::swap(PlutusV3ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

PlutusV3ScriptList&
PlutusV3ScriptList::operator=(const PlutusV3ScriptList& other)
{
  if (this != &other)
  {
    PlutusV3ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV3ScriptList&
PlutusV3ScriptList::operator=(PlutusV3ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v3_script_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
PlutusV3ScriptList::operator==(const PlutusV3ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV3ScriptList::operator!=(const PlutusV3ScriptList& other) const
{
  return !(*this == other);
}

PlutusV3ScriptList::const_reference
PlutusV3ScriptList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

PlutusV3ScriptList::iterator
PlutusV3ScriptList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

PlutusV3ScriptList::iterator
PlutusV3ScriptList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

PlutusV3ScriptList::const_iterator
PlutusV3ScriptList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV3ScriptList::const_iterator
PlutusV3ScriptList::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusV3ScriptList::const_iterator
PlutusV3ScriptList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV3ScriptList::const_iterator
PlutusV3ScriptList::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusV3ScriptList::reverse_iterator
PlutusV3ScriptList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

PlutusV3ScriptList::reverse_iterator
PlutusV3ScriptList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

PlutusV3ScriptList::const_reverse_iterator
PlutusV3ScriptList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV3ScriptList::const_reverse_iterator
PlutusV3ScriptList::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV3ScriptList::const_reverse_iterator
PlutusV3ScriptList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV3ScriptList::const_reverse_iterator
PlutusV3ScriptList::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV3ScriptList::reference
PlutusV3ScriptList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

PlutusV3ScriptList::reference
PlutusV3ScriptList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

PlutusV3ScriptList::const_reference
PlutusV3ScriptList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

PlutusV3ScriptList::reference
PlutusV3ScriptList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

PlutusV3ScriptList::const_reference
PlutusV3ScriptList::front() const
{
  materialize();

  return m_elements.front();
}

PlutusV3ScriptList::reference
PlutusV3ScriptList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

PlutusV3ScriptList::const_reference
PlutusV3ScriptList::back() const
{
  materialize();

  return m_elements.back();
}

void
PlutusV3ScriptList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
PlutusV3ScriptList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
PlutusV3ScriptList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
PlutusV3ScriptList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
PlutusV3ScriptList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

PlutusV3ScriptList::size_type
PlutusV3ScriptList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
PlutusV3ScriptList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

PlutusV3ScriptList::iterator
PlutusV3ScriptList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

PlutusV3ScriptList::iterator
PlutusV3ScriptList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

PlutusV3ScriptList::iterator
PlutusV3ScriptList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

PlutusV3ScriptList::iterator
PlutusV3ScriptList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

PlutusV3ScriptList
PlutusV3ScriptList::adopt(cardano_plutus_v3_script_list_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PlutusV3ScriptList");
  }

  return PlutusV3ScriptList(*value);
}

cardano_plutus_v3_script_list_t*
PlutusV3ScriptList::nativeHandle()
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

const cardano_plutus_v3_script_list_t*
PlutusV3ScriptList::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_plutus_v3_script_list_t*
PlutusV3ScriptList::release()
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

PlutusV3ScriptList::PlutusV3ScriptList(cardano_plutus_v3_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v3_script_list_t*
PlutusV3ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV3ScriptList");
  }

  synchronize();

  return m_value;
}

void
PlutusV3ScriptList::materialize() const
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
PlutusV3ScriptList::synchronize() const
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

  PlutusV3ScriptList replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

PlutusV3Script
PlutusV3ScriptList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV3ScriptList index");
  }

  cardano_plutus_v3_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v3_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v3_script_t, cardano_plutus_v3_script_unref> owner(value);

  checkCardanoError(result, "PlutusV3ScriptList at");

  const PlutusV3Script script = PlutusV3Script::adopt(owner.release());

  return PlutusV3Script(script);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV3ScriptList& lhs, PlutusV3ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
