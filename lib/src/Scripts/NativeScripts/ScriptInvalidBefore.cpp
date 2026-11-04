/**
 * @file ScriptInvalidBefore.cpp
 *
 * @brief Requires the validity interval to start at or after a slot.
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

#include <Cometa/Scripts/NativeScripts/ScriptInvalidBefore.h>

#include "../../CardanoInterop.h"
#include <Cometa/Json/JsonValue.h>

#include <cardano/scripts/native_scripts/script_invalid_before.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ScriptInvalidBefore::ScriptInvalidBefore(std::uint64_t slot)
: m_value(nullptr)
{
  cardano_script_invalid_before_t* value  = nullptr;
  const cardano_error_t            result = cardano_script_invalid_before_new(slot, &value);

  CardanoObject<cardano_script_invalid_before_t, cardano_script_invalid_before_unref> owner(value);

  checkCardanoError(result, "ScriptInvalidBefore");

  m_value = owner.release();
}

ScriptInvalidBefore::ScriptInvalidBefore(const ScriptInvalidBefore& other)
: m_value(nullptr)
{
  cardano_script_invalid_before_t* value  = nullptr;
  const cardano_error_t            result = cardano_script_invalid_before_deep_clone(other.checked(), &value);

  CardanoObject<cardano_script_invalid_before_t, cardano_script_invalid_before_unref> owner(value);

  checkCardanoError(result, "ScriptInvalidBefore");

  m_value = owner.release();
}

ScriptInvalidBefore::ScriptInvalidBefore(ScriptInvalidBefore&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ScriptInvalidBefore::~ScriptInvalidBefore()
{
  cardano_script_invalid_before_unref(&m_value);
}

ScriptInvalidBefore
ScriptInvalidBefore::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  ScriptInvalidBefore value(*decodeCbor<cardano_script_invalid_before_t, cardano_script_invalid_before_unref>(cbor, cardano_script_invalid_before_from_cbor));

  return value;
}

ScriptInvalidBefore
ScriptInvalidBefore::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ScriptInvalidBefore
ScriptInvalidBefore::fromJson(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  if (value.at("type").asString() != "after")
  {
    throw std::invalid_argument("Unexpected native script type");
  }

  return ScriptInvalidBefore(value.at("slot").asUInt());
}

std::uint64_t
ScriptInvalidBefore::slot() const
{
  std::uint64_t value = 0U;

  checkCardanoError(cardano_script_invalid_before_get_slot(checked(), &value), "ScriptInvalidBefore slot");

  return value;
}

void
ScriptInvalidBefore::setSlot(std::uint64_t slot)
{
  checkCardanoError(cardano_script_invalid_before_set_slot(checked(), slot), "ScriptInvalidBefore setSlot");
}

std::vector<std::uint8_t>
ScriptInvalidBefore::toCbor() const
{
  return encodeCbor(checked(), cardano_script_invalid_before_to_cbor);
}

std::string
ScriptInvalidBefore::toJson() const
{
  return encodeJson(checked(), cardano_script_invalid_before_to_cip116_json);
}

void
ScriptInvalidBefore::normalizeEncoding()
{
  cardano_script_invalid_before_clear_cbor_cache(checked());
}

void
ScriptInvalidBefore::swap(ScriptInvalidBefore& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ScriptInvalidBefore&
ScriptInvalidBefore::operator=(const ScriptInvalidBefore& other)
{
  if (this != &other)
  {
    ScriptInvalidBefore copy(other);

    swap(copy);
  }

  return *this;
}

ScriptInvalidBefore&
ScriptInvalidBefore::operator=(ScriptInvalidBefore&& other) noexcept
{
  if (this != &other)
  {
    cardano_script_invalid_before_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ScriptInvalidBefore::operator==(const ScriptInvalidBefore& other) const
{
  return cardano_script_invalid_before_equals(checked(), other.checked());
}

bool
ScriptInvalidBefore::operator!=(const ScriptInvalidBefore& other) const
{
  return !(*this == other);
}

ScriptInvalidBefore
ScriptInvalidBefore::adopt(cardano_script_invalid_before_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ScriptInvalidBefore");
  }

  return ScriptInvalidBefore(*value);
}

cardano_script_invalid_before_t*
ScriptInvalidBefore::nativeHandle() noexcept
{
  return m_value;
}

const cardano_script_invalid_before_t*
ScriptInvalidBefore::nativeHandle() const noexcept
{
  return m_value;
}

cardano_script_invalid_before_t*
ScriptInvalidBefore::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ScriptInvalidBefore::ScriptInvalidBefore(cardano_script_invalid_before_t& value) noexcept
: m_value(&value)
{
}

cardano_script_invalid_before_t*
ScriptInvalidBefore::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ScriptInvalidBefore");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ScriptInvalidBefore& lhs, ScriptInvalidBefore& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
