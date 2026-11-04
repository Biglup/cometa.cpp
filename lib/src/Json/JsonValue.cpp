/**
 * @file JsonValue.cpp
 *
 * @brief JSON value.
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

#include <Cometa/Json/JsonValue.h>

#include <Cometa/Encoding/Bech32.h>
#include <Cometa/Json/JsonWriter.h>

#include "../CardanoInterop.h"

#include <cardano/json/json_object.h>

#include <cmath>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
JsonValue::JsonValue(const JsonValue& other)
: m_value(nullptr)
{
  cardano_json_object_t* value  = nullptr;
  const cardano_error_t  result = cardano_json_object_deep_clone(other.checked(), &value);

  CardanoObject<cardano_json_object_t, cardano_json_object_unref> owner(value);

  checkCardanoError(result, "JsonValue copy");

  m_value = owner.release();
}

JsonValue::JsonValue(JsonValue&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

JsonValue::~JsonValue()
{
  cardano_json_object_unref(&m_value);
}

JsonValue
JsonValue::parse(std::string_view json)
{
  cardano_json_object_t* value  = nullptr;
  const char*            data   = json.empty() ? "" : json.data();
  const cardano_error_t  result = cardano_json_object_parse_ex(data, json.size(), &value);

  CardanoObject<cardano_json_object_t, cardano_json_object_unref> owner(value);

  checkCardanoError(result, "JsonValue parse");

  return JsonValue(*owner.release());
}

JsonType
JsonValue::type() const
{
  return static_cast<JsonType>(cardano_json_object_get_type(checked()));
}

std::string
JsonValue::toJson(JsonFormat format) const
{
  JsonWriter writer(format);

  writer.writeValue(*this);

  return writer.toString();
}

std::size_t
JsonValue::size() const
{
  if (type() == JsonType::Array)
  {
    return cardano_json_object_array_get_length(checked());
  }

  requireType(JsonType::Object);

  return cardano_json_object_get_property_count(checked());
}

bool
JsonValue::contains(std::string_view key) const
{
  requireType(JsonType::Object);

  return cardano_json_object_has_property(checked(), key.empty() ? "" : key.data(), key.size());
}

JsonValue
JsonValue::at(std::string_view key) const
{
  requireType(JsonType::Object);
  cardano_json_object_t* value = nullptr;

  if (!cardano_json_object_get_ex(checked(), key.empty() ? "" : key.data(), key.size(), &value))
  {
    throw std::out_of_range("JSON property does not exist");
  }

  return copyOf(*value);
}

JsonValue
JsonValue::at(std::size_t index) const
{
  requireType(JsonType::Array);
  const cardano_json_object_t* value = cardano_json_object_array_get_ex(checked(), index);

  if (!value)
  {
    throw std::out_of_range("JSON array index is out of range");
  }

  return copyOf(*value);
}

std::string
JsonValue::keyAt(std::size_t index) const
{
  requireType(JsonType::Object);
  std::size_t length = 0;
  const char* key    = cardano_json_object_get_key_at(checked(), index, &length);

  if (!key)
  {
    throw std::out_of_range("JSON property index is out of range");
  }

  return std::string(key, length);
}

JsonValue
JsonValue::valueAt(std::size_t index) const
{
  requireType(JsonType::Object);
  const cardano_json_object_t* value = cardano_json_object_get_value_at_ex(checked(), index);

  if (!value)
  {
    throw std::out_of_range("JSON property index is out of range");
  }

  return copyOf(*value);
}

std::string
JsonValue::asString() const
{
  requireType(JsonType::String);
  std::size_t length = 0;
  const char* text   = cardano_json_object_get_string(checked(), &length);

  return std::string(text, length - 1);
}

std::uint64_t
JsonValue::asUInt() const
{
  std::uint64_t value = 0;

  checkCardanoError(cardano_json_object_get_uint(checked(), &value), "JsonValue asUInt");

  return value;
}

std::int64_t
JsonValue::asInt() const
{
  std::int64_t value = 0;

  checkCardanoError(cardano_json_object_get_signed_int(checked(), &value), "JsonValue asInt");

  return value;
}

double
JsonValue::asDouble() const
{
  double value = 0.0;

  checkCardanoError(cardano_json_object_get_double(checked(), &value), "JsonValue asDouble");

  return value;
}

bool
JsonValue::asBool() const
{
  bool value = false;

  checkCardanoError(cardano_json_object_get_boolean(checked(), &value), "JsonValue asBool");

  return value;
}

bool
JsonValue::isNegative() const
{
  requireType(JsonType::Number);

  return cardano_json_object_get_is_negative_number(checked());
}

bool
JsonValue::isReal() const
{
  requireType(JsonType::Number);

  return cardano_json_object_get_is_real_number(checked());
}

void
JsonValue::swap(JsonValue& other) noexcept
{
  std::swap(m_value, other.m_value);
}

JsonValue&
JsonValue::operator=(const JsonValue& other)
{
  if (this != &other)
  {
    JsonValue copy(other);

    swap(copy);
  }

  return *this;
}

JsonValue&
JsonValue::operator=(JsonValue&& other) noexcept
{
  if (this != &other)
  {
    cardano_json_object_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

JsonValue
JsonValue::adopt(cardano_json_object_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null JsonValue");
  }

  return JsonValue(*value);
}

cardano_json_object_t*
JsonValue::nativeHandle() noexcept
{
  return m_value;
}

const cardano_json_object_t*
JsonValue::nativeHandle() const noexcept
{
  return m_value;
}

cardano_json_object_t*
JsonValue::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

cardano_json_object_t*
JsonValue::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from JsonValue");
  }

  return m_value;
}

JsonValue::JsonValue(cardano_json_object_t& value) noexcept
: m_value(&value)
{
}

JsonValue
JsonValue::copyOf(const cardano_json_object_t& value)
{
  cardano_json_object_t* copy   = nullptr;
  const cardano_error_t  result = cardano_json_object_deep_clone(&value, &copy);

  CardanoObject<cardano_json_object_t, cardano_json_object_unref> owner(copy);

  checkCardanoError(result, "JsonValue child copy");

  return JsonValue(*owner.release());
}

void
JsonValue::requireType(JsonType expected) const
{
  if (type() != expected)
  {
    throw std::invalid_argument("JSON value has the wrong type");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(JsonValue& lhs, JsonValue& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
