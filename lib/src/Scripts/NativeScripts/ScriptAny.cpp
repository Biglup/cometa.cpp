/**
 * @file ScriptAny.cpp
 *
 * @brief Requires at least one child script to be satisfied.
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

#include <Cometa/Scripts/NativeScripts/ScriptAny.h>

#include "../../CardanoInterop.h"
#include "../../ValueValidation.h"
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptList.h>

#include <cardano/scripts/native_scripts/native_script_list.h>
#include <cardano/scripts/native_scripts/script_any.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptAny::ScriptAny(const NativeScriptList& scripts)
: m_value(nullptr)
{
  NativeScriptList copy(scripts);

  cardano_script_any_t* value  = nullptr;
  const cardano_error_t result = cardano_script_any_new(checkedNativeHandle(copy), &value);

  CardanoObject<cardano_script_any_t, cardano_script_any_unref> owner(value);

  checkCardanoError(result, "ScriptAny");

  m_value = owner.release();
}

ScriptAny::ScriptAny(const ScriptAny& other)
: m_value(nullptr)
{
  cardano_script_any_t* value  = nullptr;
  const cardano_error_t result = cardano_script_any_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_any_t, cardano_script_any_unref> owner(value);

  checkCardanoError(result, "ScriptAny");

  m_value = owner.release();
}

ScriptAny::ScriptAny(ScriptAny&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptAny::~ScriptAny()
{
  cardano_script_any_unref(&m_value);
}

ScriptAny
ScriptAny::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptAny value(*decodeCbor<cardano_script_any_t, cardano_script_any_unref>(cbor, cardano_script_any_from_cbor));

  validateValue(value.scripts());

  return value;
}

ScriptAny
ScriptAny::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptAny
ScriptAny::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "any")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  return ScriptAny(NativeScriptList::fromJson(json));
}

std::size_t
ScriptAny::size() const
{
  return cardano_script_any_get_length(checked());
}

NativeScriptList
ScriptAny::scripts() const
{
  cardano_native_script_list_t* value  = nullptr;
  const cardano_error_t         result = cardano_script_any_get_scripts(checked(), &value);

  CardanoObject<cardano_native_script_list_t, cardano_native_script_list_unref> owner(value);

  checkCardanoError(result, "ScriptAny scripts");

  const NativeScriptList list = NativeScriptList::adopt(owner.release());

  return NativeScriptList(list);
}

void
ScriptAny::setScripts(const NativeScriptList& scripts)
{
  NativeScriptList copy(scripts);

  checkCardanoError(cardano_script_any_set_scripts(checked(), checkedNativeHandle(copy)), "ScriptAny setScripts");
}

std::vector<std::uint8_t>
ScriptAny::toCbor() const
{
  return encodeCbor(checked(), cardano_script_any_to_cbor);
}

std::string
ScriptAny::toJson() const
{
  return encodeJson(checked(), cardano_script_any_to_cip116_json);
}

void
ScriptAny::normalizeEncoding()
{
  cardano_script_any_clear_cbor_cache(checked());
}

void
ScriptAny::swap(ScriptAny& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptAny&
ScriptAny::operator=(const ScriptAny& other)
{
  if (this != &other)
  {
    ScriptAny copy(other);

    swap(copy);
  }

  return *this;
}

ScriptAny&
ScriptAny::operator=(ScriptAny&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_any_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptAny::operator==(const ScriptAny& other) const
{
  return cardano_script_any_equals(checked(), other.checked());
}

bool
ScriptAny::operator!=(const ScriptAny& other) const
{
  return !(*this == other);
}

ScriptAny
ScriptAny::adopt(cardano_script_any_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ScriptAny");
  }

  return ScriptAny(*value);
}

cardano_script_any_t*
ScriptAny::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_any_t*
ScriptAny::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_any_t*
ScriptAny::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptAny::ScriptAny(cardano_script_any_t& value) noexcept
: m_value(&value)
{
}

cardano_script_any_t*
ScriptAny::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptAny");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptAny& lhs, ScriptAny& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
