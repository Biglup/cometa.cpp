/**
 * @file JsonWriter.cpp
 *
 * @brief JSON writer.
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

#include <Cometa/Json/JsonWriter.h>

#include <Cometa/Encoding/Bech32.h>
#include <Cometa/Json/JsonValue.h>

#include "../CardanoInterop.h"

#include <cardano/json/json_writer.h>

#include <cmath>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
JsonWriter::JsonWriter(JsonFormat format)
: m_value(nullptr)
, m_format(format)
{
  if (format != JsonFormat::Compact && format != JsonFormat::Pretty)
  {
    throw std::invalid_argument("Unknown JSON format");
  }

  m_value = cardano_json_writer_new(static_cast<cardano_json_format_t>(format));

  if (!m_value)
  {
    throw std::bad_alloc();
  }
}

JsonWriter::JsonWriter(const JsonWriter& other)
: m_value(nullptr)
, m_format(other.m_format)
{
  cardano_json_writer_t* value  = nullptr;
  const cardano_error_t  result = cardano_json_writer_deep_clone(other.checked(), &value);

  CardanoObject<cardano_json_writer_t, cardano_json_writer_unref> owner(value);

  checkCardanoError(result, "JsonWriter copy");

  m_value = owner.release();
}

JsonWriter::JsonWriter(JsonWriter&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_format(other.m_format)
{
}

JsonWriter::~JsonWriter()
{
  cardano_json_writer_unref(&m_value);
}

void
JsonWriter::writeStartArray()
{
  cardano_json_writer_write_start_array(checked());
  checkWrite();
}

void
JsonWriter::writeEndArray()
{
  cardano_json_writer_write_end_array(checked());
  checkWrite();
}

void
JsonWriter::writeStartObject()
{
  cardano_json_writer_write_start_object(checked());
  checkWrite();
}

void
JsonWriter::writeEndObject()
{
  cardano_json_writer_write_end_object(checked());
  checkWrite();
}

void
JsonWriter::writeNull()
{
  cardano_json_writer_write_null(checked());
  checkWrite();
}

void
JsonWriter::writeBool(bool value)
{
  cardano_json_writer_write_bool(checked(), value);
  checkWrite();
}

void
JsonWriter::writeUInt(std::uint64_t value)
{
  cardano_json_writer_write_uint(checked(), value);
  checkWrite();
}

void
JsonWriter::writeInt(std::int64_t value)
{
  cardano_json_writer_write_signed_int(checked(), value);
  checkWrite();
}

void
JsonWriter::writeUIntAsString(std::uint64_t value)
{
  cardano_json_writer_write_uint_as_string(checked(), value);
  checkWrite();
}

void
JsonWriter::writeIntAsString(std::int64_t value)
{
  cardano_json_writer_write_signed_int_as_string(checked(), value);
  checkWrite();
}

void
JsonWriter::writeDouble(double value)
{
  if (!std::isfinite(value))
  {
    throw std::invalid_argument("JSON numbers must be finite");
  }

  cardano_json_writer_write_double(checked(), value);
  checkWrite();
}

void
JsonWriter::writeDoubleAsString(double value)
{
  if (!std::isfinite(value))
  {
    throw std::invalid_argument("JSON numbers must be finite");
  }

  cardano_json_writer_write_double_as_string(checked(), value);
  checkWrite();
}

void
JsonWriter::writeString(std::string_view text)
{
  cardano_json_writer_write_string(checked(), text.empty() ? "" : text.data(), text.size());
  checkWrite();
}

void
JsonWriter::writePropertyName(std::string_view text)
{
  cardano_json_writer_write_property_name(checked(), text.empty() ? "" : text.data(), text.size());
  checkWrite();
}

void
JsonWriter::writeRaw(std::string_view json)
{
  const JsonValue value = JsonValue::parse(json);

  static_cast<void>(value);
  cardano_json_writer_write_raw_value(checked(), json.data(), json.size());
  checkWrite();
}

void
JsonWriter::writeValue(const JsonValue& value)
{
  cardano_json_writer_write_object(checked(), checkedNativeHandle(value));
  checkWrite();
}

void
JsonWriter::writeBigInt(const BigInt& value)
{
  const std::string text = value.toString();

  cardano_json_writer_write_raw_value(checked(), text.data(), text.size());
  checkWrite();
}

void
JsonWriter::writeBigIntAsString(const BigInt& value)
{
  cardano_json_writer_write_bigint(checked(), checkedNativeHandle(value));
  checkWrite();
}

void
JsonWriter::writeBytesAsHex(const std::vector<std::uint8_t>& bytes)
{
  if (bytes.empty())
  {
    writeString("");
    return;
  }

  cardano_json_writer_write_bytes_as_hex(checked(), bytes.data(), bytes.size());
  checkWrite();
}

void
JsonWriter::writeBytesAsBech32(const std::string_view& prefix, const std::vector<std::uint8_t>& bytes)
{
  writeString(encodeBech32(prefix, bytes));
}

JsonContext
JsonWriter::context() const
{
  return static_cast<JsonContext>(cardano_json_writer_get_context(checked()));
}

std::string
JsonWriter::toString() const
{
  checkWrite();
  const std::size_t size = cardano_json_writer_get_encoded_size(checked());

  if (context() != JsonContext::Root || size <= 1)
  {
    throw std::logic_error("JSON document is incomplete");
  }

  std::string text(size, '\0');

  checkCardanoError(cardano_json_writer_encode(checked(), text.data(), text.size()), "JsonWriter toString");

  text.pop_back();

  return text;
}

void
JsonWriter::reset()
{
  JsonWriter empty(m_format);

  swap(empty);
}

void
JsonWriter::swap(JsonWriter& other) noexcept
{
  std::swap(m_value, other.m_value);
  std::swap(m_format, other.m_format);
}

JsonWriter&
JsonWriter::operator=(const JsonWriter& other)
{
  if (this != &other)
  {
    JsonWriter copy(other);

    swap(copy);
  }

  return *this;
}

JsonWriter&
JsonWriter::operator=(JsonWriter&& other) noexcept
{
  if (this != &other)
  {
    cardano_json_writer_unref(&m_value);
    m_value  = std::exchange(other.m_value, nullptr);
    m_format = other.m_format;
  }

  return *this;
}

JsonWriter
JsonWriter::adopt(cardano_json_writer_t* value, JsonFormat format)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null JsonWriter");
  }

  if (format != JsonFormat::Compact && format != JsonFormat::Pretty)
  {
    throw std::invalid_argument("Unknown JSON format");
  }

  return JsonWriter(value, format);
}

cardano_json_writer_t*
JsonWriter::nativeHandle() noexcept
{
  return m_value;
}

const cardano_json_writer_t*
JsonWriter::nativeHandle() const noexcept
{
  return m_value;
}

cardano_json_writer_t*
JsonWriter::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

JsonWriter::JsonWriter(cardano_json_writer_t* value, JsonFormat format) noexcept
: m_value(value)
, m_format(format)
{
}

cardano_json_writer_t*
JsonWriter::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from JsonWriter");
  }

  return m_value;
}

void
JsonWriter::checkWrite() const
{
  if (cardano_json_writer_get_encoded_size(checked()) == 0)
  {
    char sentinel = '\0';

    checkCardanoError(cardano_json_writer_encode(checked(), &sentinel, 1), "JsonWriter write");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(JsonWriter& lhs, JsonWriter& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
