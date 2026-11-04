/**
 * @file NativeScriptList.cpp
 *
 * @brief An ordered sequence of native scripts.
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

#include <Cometa/Scripts/NativeScripts/NativeScriptList.h>

#include "../../CardanoInterop.h"
#include "../../ValueValidation.h"
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Scripts/NativeScripts/NativeScript.h>

#include <cardano/scripts/native_scripts/native_script.h>
#include <cardano/scripts/native_scripts/native_script_list.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
NativeScriptList::NativeScriptList()
: m_value(nullptr)
{
  cardano_native_script_list_t* value  = nullptr;
  const cardano_error_t         result = cardano_native_script_list_new(&value);

  CardanoObject<cardano_native_script_list_t, cardano_native_script_list_unref> owner(value);

  checkCardanoError(result, "NativeScriptList");

  m_value = owner.release();
}

NativeScriptList::NativeScriptList(std::initializer_list<NativeScript> scripts)
: NativeScriptList()
{
  for (const NativeScript& script: scripts)
  {
    pushBack(script);
  }
}

NativeScriptList::NativeScriptList(const NativeScriptList& other)
: m_value(nullptr)
{
  cardano_native_script_list_t* value  = nullptr;
  const cardano_error_t         result = cardano_native_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_native_script_list_t, cardano_native_script_list_unref> owner(value);

  checkCardanoError(result, "NativeScriptList");

  m_value = owner.release();
}

NativeScriptList::NativeScriptList(NativeScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

NativeScriptList::~NativeScriptList()
{
  cardano_native_script_list_unref(&m_value);
}

NativeScriptList
NativeScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  NativeScriptList value(*decodeCbor<cardano_native_script_list_t, cardano_native_script_list_unref>(cbor, cardano_native_script_list_from_cbor));

  validateValue(value);

  return value;
}

NativeScriptList
NativeScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

NativeScriptList
NativeScriptList::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json).at("scripts");
  if (value.type() != JsonType::Array)
  {
    throw std::invalid_argument("Expected a scripts array");
  }

  NativeScriptList result;

  for (std::size_t index = 0U; index < value.size(); ++index)
  {
    result.pushBack(NativeScript::fromJson(value.at(index).toJson()));
  }

  return result;
}

std::size_t
NativeScriptList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_native_script_list_get_length(checked());
}

bool
NativeScriptList::empty() const
{
  return size() == 0U;
}

void
NativeScriptList::pushBack(const NativeScript& script)
{
  if (m_materialized)
  {
    push_back(script);
    return;
  }

  NativeScript copy(script);

  checkCardanoError(cardano_native_script_list_add(checked(), checkedNativeHandle(copy)), "NativeScriptList pushBack");
}

std::vector<NativeScript>
NativeScriptList::values() const
{
  std::vector<NativeScript> result;
  const std::size_t         count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
NativeScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_native_script_list_to_cbor);
}

std::string
NativeScriptList::toJson() const
{
  return encodeJson(checked(), cardano_native_script_list_to_cip116_json);
}

void
NativeScriptList::normalizeEncoding()
{
  if (m_materialized)
  {
    for (value_type& value: m_elements)
    {
      value.normalizeEncoding();
    }
    m_mutableAccess = true;
  }

  cardano_native_script_list_clear_cbor_cache(checked());
}

void
NativeScriptList::swap(NativeScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

NativeScriptList&
NativeScriptList::operator=(const NativeScriptList& other)
{
  if (this != &other)
  {
    NativeScriptList copy(other);

    swap(copy);
  }

  return *this;
}

NativeScriptList&
NativeScriptList::operator=(NativeScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_native_script_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
NativeScriptList::operator==(const NativeScriptList& other) const
{
  return cardano_native_script_list_equals(checked(), other.checked());
}

bool
NativeScriptList::operator!=(const NativeScriptList& other) const
{
  return !(*this == other);
}

NativeScriptList::const_reference
NativeScriptList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

NativeScriptList::iterator
NativeScriptList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

NativeScriptList::iterator
NativeScriptList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

NativeScriptList::const_iterator
NativeScriptList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

NativeScriptList::const_iterator
NativeScriptList::end() const
{
  materialize();

  return m_elements.cend();
}

NativeScriptList::const_iterator
NativeScriptList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

NativeScriptList::const_iterator
NativeScriptList::cend() const
{
  materialize();

  return m_elements.cend();
}

NativeScriptList::reverse_iterator
NativeScriptList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

NativeScriptList::reverse_iterator
NativeScriptList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

NativeScriptList::const_reverse_iterator
NativeScriptList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

NativeScriptList::const_reverse_iterator
NativeScriptList::rend() const
{
  materialize();

  return m_elements.crend();
}

NativeScriptList::const_reverse_iterator
NativeScriptList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

NativeScriptList::const_reverse_iterator
NativeScriptList::crend() const
{
  materialize();

  return m_elements.crend();
}

NativeScriptList::reference
NativeScriptList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

NativeScriptList::reference
NativeScriptList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

NativeScriptList::const_reference
NativeScriptList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

NativeScriptList::reference
NativeScriptList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

NativeScriptList::const_reference
NativeScriptList::front() const
{
  materialize();

  return m_elements.front();
}

NativeScriptList::reference
NativeScriptList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

NativeScriptList::const_reference
NativeScriptList::back() const
{
  materialize();

  return m_elements.back();
}

void
NativeScriptList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
NativeScriptList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
NativeScriptList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
NativeScriptList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
NativeScriptList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

NativeScriptList::size_type
NativeScriptList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
NativeScriptList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

NativeScriptList::iterator
NativeScriptList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

NativeScriptList::iterator
NativeScriptList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

NativeScriptList::iterator
NativeScriptList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

NativeScriptList::iterator
NativeScriptList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

NativeScriptList
NativeScriptList::adopt(cardano_native_script_list_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null NativeScriptList");
  }

  return NativeScriptList(*value);
}

cardano_native_script_list_t*
NativeScriptList::nativeHandle()
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

const cardano_native_script_list_t*
NativeScriptList::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_native_script_list_t*
NativeScriptList::release()
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

NativeScriptList::NativeScriptList(cardano_native_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_native_script_list_t*
NativeScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from NativeScriptList");
  }

  synchronize();

  return m_value;
}

void
validateValue(const NativeScriptList& value)
{
  const std::size_t count = value.size();

  for (std::size_t index = 0U; index < count; ++index)
  {
    validateValue(value.at(index));
  }
}

void
NativeScriptList::materialize() const
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
NativeScriptList::synchronize() const
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

  const std::vector<std::uint8_t> previous    = encodeCbor(m_value, cardano_native_script_list_to_cbor);
  NativeScriptList                replacement = !previous.empty() && previous.front() == 0x9f
                   ? NativeScriptList::fromCbor("9fff")
                   : NativeScriptList();

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

NativeScript
NativeScriptList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("NativeScriptList index");
  }

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_list_get(checked(), index, &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScriptList at");

  const NativeScript script = NativeScript::adopt(owner.release());

  return NativeScript(script);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NativeScriptList& lhs, NativeScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
