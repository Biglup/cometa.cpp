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
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Scripts/NativeScripts/NativeScript.h>

#include <cardano/scripts/native_scripts/native_script.h>
#include <cardano/scripts/native_scripts/native_script_list.h>

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

  value.validate();

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
  return cardano_native_script_list_get_length(checked());
}

bool
NativeScriptList::empty() const
{
  return size() == 0U;
}

NativeScript
NativeScriptList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("NativeScriptList index");
  }

  cardano_native_script_t* value  = nullptr;
  const cardano_error_t    result = cardano_native_script_list_get(checked(), index, &value);

  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(result, "NativeScriptList at");

  const NativeScript script(*owner.release());

  return NativeScript(script);
}

void
NativeScriptList::pushBack(const NativeScript& script)
{
  NativeScript copy(script);

  checkCardanoError(cardano_native_script_list_add(checked(), copy.checked()), "NativeScriptList pushBack");
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
  cardano_native_script_list_clear_cbor_cache(checked());
}

void
NativeScriptList::swap(NativeScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
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
    m_value = std::exchange(other.m_value, nullptr);
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

  return m_value;
}

void
NativeScriptList::validate() const
{
  const std::size_t count = size();

  for (std::size_t index = 0U; index < count; ++index)
  {
    at(index).validate();
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NativeScriptList& lhs, NativeScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
