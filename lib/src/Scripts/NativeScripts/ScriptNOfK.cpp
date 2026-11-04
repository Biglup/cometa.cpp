/**
 * @file ScriptNOfK.cpp
 *
 * @brief Requires a specified number of child scripts to be satisfied.
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

#include <Cometa/Scripts/NativeScripts/ScriptNOfK.h>

#include "../../CardanoInterop.h"
#include "../../ValueValidation.h"
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Scripts/NativeScripts/NativeScriptList.h>

#include <cardano/scripts/native_scripts/native_script_list.h>
#include <cardano/scripts/native_scripts/script_n_of_k.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptNOfK::ScriptNOfK(std::size_t required, const NativeScriptList& scripts)
: m_value(nullptr)
{
  NativeScriptList copy(scripts);

  cardano_script_n_of_k_t* value  = nullptr;
  const cardano_error_t    result = cardano_script_n_of_k_new(checkedNativeHandle(copy), required, &value);

  CardanoObject<cardano_script_n_of_k_t, cardano_script_n_of_k_unref> owner(value);

  checkCardanoError(result, "ScriptNOfK");

  m_value = owner.release();
}

ScriptNOfK::ScriptNOfK(const ScriptNOfK& other)
: m_value(nullptr)
{
  cardano_script_n_of_k_t* value  = nullptr;
  const cardano_error_t    result = cardano_script_n_of_k_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_n_of_k_t, cardano_script_n_of_k_unref> owner(value);

  checkCardanoError(result, "ScriptNOfK");

  m_value = owner.release();
}

ScriptNOfK::ScriptNOfK(ScriptNOfK&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptNOfK::~ScriptNOfK()
{
  cardano_script_n_of_k_unref(&m_value);
}

ScriptNOfK
ScriptNOfK::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptNOfK value(*decodeCbor<cardano_script_n_of_k_t, cardano_script_n_of_k_unref>(cbor, cardano_script_n_of_k_from_cbor));

  validateValue(value.scripts());

  return value;
}

ScriptNOfK
ScriptNOfK::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptNOfK
ScriptNOfK::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "atLeast")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  const std::uint64_t threshold = value.at("required").asUInt();
  const std::size_t   count     = static_cast<std::size_t>(threshold);

  if (static_cast<std::uint64_t>(count) != threshold)
  {
    throw std::out_of_range("Threshold exceeds the platform size range");
  }

  return ScriptNOfK(count, NativeScriptList::fromJson(json));
}

std::size_t
ScriptNOfK::size() const
{
  return cardano_script_n_of_k_get_length(checked());
}

NativeScriptList
ScriptNOfK::scripts() const
{
  cardano_native_script_list_t* value  = nullptr;
  const cardano_error_t         result = cardano_script_n_of_k_get_scripts(checked(), &value);

  CardanoObject<cardano_native_script_list_t, cardano_native_script_list_unref> owner(value);

  checkCardanoError(result, "ScriptNOfK scripts");

  const NativeScriptList list = NativeScriptList::adopt(owner.release());

  return NativeScriptList(list);
}

void
ScriptNOfK::setScripts(const NativeScriptList& scripts)
{
  NativeScriptList copy(scripts);

  checkCardanoError(cardano_script_n_of_k_set_scripts(checked(), checkedNativeHandle(copy)), "ScriptNOfK setScripts");
}

std::size_t
ScriptNOfK::required() const
{
  return cardano_script_n_of_k_get_required(checked());
}

void
ScriptNOfK::setRequired(std::size_t required)
{
  checkCardanoError(cardano_script_n_of_k_set_required(checked(), required), "ScriptNOfK setRequired");
}

std::vector<std::uint8_t>
ScriptNOfK::toCbor() const
{
  return encodeCbor(checked(), cardano_script_n_of_k_to_cbor);
}

std::string
ScriptNOfK::toJson() const
{
  return encodeJson(checked(), cardano_script_n_of_k_to_cip116_json);
}

void
ScriptNOfK::normalizeEncoding()
{
  cardano_script_n_of_k_clear_cbor_cache(checked());
}

void
ScriptNOfK::swap(ScriptNOfK& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptNOfK&
ScriptNOfK::operator=(const ScriptNOfK& other)
{
  if (this != &other)
  {
    ScriptNOfK copy(other);

    swap(copy);
  }

  return *this;
}

ScriptNOfK&
ScriptNOfK::operator=(ScriptNOfK&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_n_of_k_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptNOfK::operator==(const ScriptNOfK& other) const
{
  return cardano_script_n_of_k_equals(checked(), other.checked());
}

bool
ScriptNOfK::operator!=(const ScriptNOfK& other) const
{
  return !(*this == other);
}

ScriptNOfK
ScriptNOfK::adopt(cardano_script_n_of_k_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ScriptNOfK");
  }

  return ScriptNOfK(*value);
}

cardano_script_n_of_k_t*
ScriptNOfK::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_n_of_k_t*
ScriptNOfK::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_n_of_k_t*
ScriptNOfK::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptNOfK::ScriptNOfK(cardano_script_n_of_k_t& value) noexcept
: m_value(&value)
{
}

cardano_script_n_of_k_t*
ScriptNOfK::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptNOfK");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptNOfK& lhs, ScriptNOfK& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
