/**
 * @file ScriptInvalidAfter.cpp
 *
 * @brief Requires the validity interval to end at or before a slot.
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

#include <Cometa/Scripts/NativeScripts/ScriptInvalidAfter.h>

#include "../../CardanoInterop.h"
#include <Cometa/Json/JsonValue.h>

#include <cardano/scripts/native_scripts/script_invalid_after.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptInvalidAfter::ScriptInvalidAfter(std::uint64_t slot)
: m_value(nullptr)
{
  cardano_script_invalid_after_t* value  = nullptr;
  const cardano_error_t           result = cardano_script_invalid_after_new(slot, &value);

  CardanoObject<cardano_script_invalid_after_t, cardano_script_invalid_after_unref> owner(value);

  checkCardanoError(result, "ScriptInvalidAfter");

  m_value = owner.release();
}

ScriptInvalidAfter::ScriptInvalidAfter(const ScriptInvalidAfter& other)
: m_value(nullptr)
{
  cardano_script_invalid_after_t* value  = nullptr;
  const cardano_error_t           result = cardano_script_invalid_after_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_invalid_after_t, cardano_script_invalid_after_unref> owner(value);

  checkCardanoError(result, "ScriptInvalidAfter");

  m_value = owner.release();
}

ScriptInvalidAfter::ScriptInvalidAfter(ScriptInvalidAfter&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptInvalidAfter::~ScriptInvalidAfter()
{
  cardano_script_invalid_after_unref(&m_value);
}

ScriptInvalidAfter
ScriptInvalidAfter::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptInvalidAfter value(*decodeCbor<cardano_script_invalid_after_t, cardano_script_invalid_after_unref>(cbor, cardano_script_invalid_after_from_cbor));

  return value;
}

ScriptInvalidAfter
ScriptInvalidAfter::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptInvalidAfter
ScriptInvalidAfter::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "before")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  return ScriptInvalidAfter(value.at("slot").asUInt());
}

std::uint64_t
ScriptInvalidAfter::slot() const
{
  std::uint64_t value = 0U;

  checkCardanoError(cardano_script_invalid_after_get_slot(checked(), &value), "ScriptInvalidAfter slot");

  return value;
}

void
ScriptInvalidAfter::setSlot(std::uint64_t slot)
{
  checkCardanoError(cardano_script_invalid_after_set_slot(checked(), slot), "ScriptInvalidAfter setSlot");
}

std::vector<std::uint8_t>
ScriptInvalidAfter::toCbor() const
{
  return encodeCbor(checked(), cardano_script_invalid_after_to_cbor);
}

std::string
ScriptInvalidAfter::toJson() const
{
  return encodeJson(checked(), cardano_script_invalid_after_to_cip116_json);
}

void
ScriptInvalidAfter::normalizeEncoding()
{
  cardano_script_invalid_after_clear_cbor_cache(checked());
}

void
ScriptInvalidAfter::swap(ScriptInvalidAfter& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptInvalidAfter&
ScriptInvalidAfter::operator=(const ScriptInvalidAfter& other)
{
  if (this != &other)
  {
    ScriptInvalidAfter copy(other);

    swap(copy);
  }

  return *this;
}

ScriptInvalidAfter&
ScriptInvalidAfter::operator=(ScriptInvalidAfter&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_invalid_after_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptInvalidAfter::operator==(const ScriptInvalidAfter& other) const
{
  return cardano_script_invalid_after_equals(checked(), other.checked());
}

bool
ScriptInvalidAfter::operator!=(const ScriptInvalidAfter& other) const
{
  return !(*this == other);
}

ScriptInvalidAfter
ScriptInvalidAfter::adopt(cardano_script_invalid_after_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ScriptInvalidAfter");
  }

  return ScriptInvalidAfter(*value);
}

cardano_script_invalid_after_t*
ScriptInvalidAfter::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_invalid_after_t*
ScriptInvalidAfter::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_invalid_after_t*
ScriptInvalidAfter::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptInvalidAfter::ScriptInvalidAfter(cardano_script_invalid_after_t& value) noexcept
: m_value(&value)
{
}

cardano_script_invalid_after_t*
ScriptInvalidAfter::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptInvalidAfter");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptInvalidAfter& lhs, ScriptInvalidAfter& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
