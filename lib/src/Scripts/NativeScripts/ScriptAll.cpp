/**
 * @file ScriptAll.cpp
 *
 * @brief Requires every child script to be satisfied.
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

#include <Cometa/Scripts/NativeScripts/ScriptAll.h>

#include "../../CardanoInterop.h"
#include "../../ValueValidation.h"
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptList.h>

#include <cardano/scripts/native_scripts/native_script_list.h>
#include <cardano/scripts/native_scripts/script_all.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptAll::ScriptAll(const NativeScriptList& scripts)
: m_value(nullptr)
{
  NativeScriptList copy(scripts);

  cardano_script_all_t* value  = nullptr;
  const cardano_error_t result = cardano_script_all_new(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_all_t, cardano_script_all_unref> owner(value);

  checkCardanoError(result, "ScriptAll");

  m_value = owner.release();
}

ScriptAll::ScriptAll(const ScriptAll& other)
: m_value(nullptr)
{
  cardano_script_all_t* value  = nullptr;
  const cardano_error_t result = cardano_script_all_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_all_t, cardano_script_all_unref> owner(value);

  checkCardanoError(result, "ScriptAll");

  m_value = owner.release();
}

ScriptAll::ScriptAll(ScriptAll&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptAll::~ScriptAll()
{
  cardano_script_all_unref(&m_value);
}

ScriptAll
ScriptAll::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptAll value(*decodeCbor<cardano_script_all_t, cardano_script_all_unref>(cbor, cardano_script_all_from_cbor));

  validateValue(value.scripts());

  return value;
}

ScriptAll
ScriptAll::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptAll
ScriptAll::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "all")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  return ScriptAll(NativeScriptList::fromJson(json));
}

std::size_t
ScriptAll::size() const
{
  return cardano_script_all_get_length(checked());
}

NativeScriptList
ScriptAll::scripts() const
{
  cardano_native_script_list_t* value  = nullptr;
  const cardano_error_t         result = cardano_script_all_get_scripts(checked(), &value);

  CardanoObject<cardano_native_script_list_t, cardano_native_script_list_unref> owner(value);

  checkCardanoError(result, "ScriptAll scripts");

  const NativeScriptList list = NativeScriptList::adopt(owner.release());

  return NativeScriptList(list);
}

void
ScriptAll::setScripts(const NativeScriptList& scripts)
{
  NativeScriptList copy(scripts);

  checkCardanoError(cardano_script_all_set_scripts(checked(), checkedNativeHandle(copy)), "ScriptAll setScripts");
}

std::vector<std::uint8_t>
ScriptAll::toCbor() const
{
  return encodeCbor(checked(), cardano_script_all_to_cbor);
}

std::string
ScriptAll::toJson() const
{
  return encodeJson(checked(), cardano_script_all_to_cip116_json);
}

void
ScriptAll::normalizeEncoding()
{
  cardano_script_all_clear_cbor_cache(checked());
}

void
ScriptAll::swap(ScriptAll& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptAll&
ScriptAll::operator=(const ScriptAll& other)
{
  if (this != &other)
  {
    ScriptAll copy(other);

    swap(copy);
  }

  return *this;
}

ScriptAll&
ScriptAll::operator=(ScriptAll&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_all_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptAll::operator==(const ScriptAll& other) const
{
  return cardano_script_all_equals(checked(), other.checked());
}

bool
ScriptAll::operator!=(const ScriptAll& other) const
{
  return !(*this == other);
}

ScriptAll
ScriptAll::adopt(cardano_script_all_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ScriptAll");
  }

  return ScriptAll(*value);
}

cardano_script_all_t*
ScriptAll::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_all_t*
ScriptAll::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_all_t*
ScriptAll::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptAll::ScriptAll(cardano_script_all_t& value) noexcept
: m_value(&value)
{
}

cardano_script_all_t*
ScriptAll::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptAll");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptAll& lhs, ScriptAll& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
