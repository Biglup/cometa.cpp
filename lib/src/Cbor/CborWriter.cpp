/**
 * @file CborWriter.cpp
 *
 * @brief Streaming CBOR writer.
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

#include <Cometa/Cbor/CborWriter.h>

#include "../CardanoInterop.h"

#include <cardano/cbor/cbor_writer.h>

#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CborWriter::CborWriter()
: m_value(cardano_cbor_writer_new())
{
  if (!m_value)
  {
    throw std::bad_alloc();
  }
}

CborWriter::CborWriter(const CborWriter& other)
: m_value(nullptr)
{
  cardano_cbor_writer_t* value  = nullptr;
  const cardano_error_t  result = cardano_cbor_writer_deep_clone(other.checked(), &value);

  CardanoObject<cardano_cbor_writer_t, cardano_cbor_writer_unref> owner(value);

  checkCardanoError(result, "CborWriter copy");

  m_value = owner.release();
}

CborWriter::CborWriter(CborWriter&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

CborWriter::~CborWriter()
{
  cardano_cbor_writer_unref(&m_value);
}

void
CborWriter::writeUInt(std::uint64_t value)
{
  checkCardanoError(cardano_cbor_writer_write_uint(checked(), value), "CborWriter writeUInt");
}

void
CborWriter::writeInt(std::int64_t value)
{
  checkCardanoError(cardano_cbor_writer_write_signed_int(checked(), value), "CborWriter writeInt");
}

void
CborWriter::writeNegativeInt(std::uint64_t value)
{
  checkCardanoError(cardano_cbor_writer_write_negative_int(checked(), value), "CborWriter writeNegativeInt");
}

void
CborWriter::writeBool(bool value)
{
  checkCardanoError(cardano_cbor_writer_write_bool(checked(), value), "CborWriter writeBool");
}

void
CborWriter::writeNull()
{
  checkCardanoError(cardano_cbor_writer_write_null(checked()), "CborWriter writeNull");
}

void
CborWriter::writeUndefined()
{
  checkCardanoError(cardano_cbor_writer_write_undefined(checked()), "CborWriter writeUndefined");
}

void
CborWriter::writeEndArray()
{
  checkCardanoError(cardano_cbor_writer_write_end_array(checked()), "CborWriter writeEndArray");
}

void
CborWriter::writeEndMap()
{
  checkCardanoError(cardano_cbor_writer_write_end_map(checked()), "CborWriter writeEndMap");
}

void
CborWriter::writeStartArray(std::optional<std::uint64_t> size)
{
  if (size && *size > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
  {
    throw std::out_of_range("CBOR container size exceeds INT64_MAX");
  }

  const std::int64_t length = size ? static_cast<std::int64_t>(*size) : -1;

  checkCardanoError(cardano_cbor_writer_write_start_array(checked(), length), "CborWriter writeStartArray");
}

void
CborWriter::writeStartMap(std::optional<std::uint64_t> size)
{
  if (size && *size > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
  {
    throw std::out_of_range("CBOR container size exceeds INT64_MAX");
  }

  const std::int64_t length = size ? static_cast<std::int64_t>(*size) : -1;

  checkCardanoError(cardano_cbor_writer_write_start_map(checked(), length), "CborWriter writeStartMap");
}

void
CborWriter::writeBytes(const std::vector<std::uint8_t>& bytes)
{
  checkCardanoError(cardano_cbor_writer_write_bytestring(checked(), byteData(bytes), bytes.size()), "CborWriter writeBytes");
}

void
CborWriter::writeEncoded(const std::vector<std::uint8_t>& bytes)
{
  checkCardanoError(cardano_cbor_writer_write_encoded(checked(), byteData(bytes), bytes.size()), "CborWriter writeEncoded");
}

void
CborWriter::writeText(std::string_view text)
{
  const char* data = text.empty() ? "" : text.data();

  checkCardanoError(cardano_cbor_writer_write_textstring(checked(), data, text.size()), "CborWriter writeText");
}

void
CborWriter::writeBigInt(const BigInt& value)
{
  checkCardanoError(cardano_cbor_writer_write_bigint(checked(), checkedNativeHandle(value)), "CborWriter writeBigInt");
}

void
CborWriter::writeTag(CborTag tag)
{
  if (static_cast<std::uint32_t>(tag) > static_cast<std::uint32_t>(std::numeric_limits<std::int32_t>::max()))
  {
    throw std::out_of_range("CBOR tag exceeds the supported range");
  }

  checkCardanoError(cardano_cbor_writer_write_tag(checked(), static_cast<cardano_cbor_tag_t>(tag)), "CborWriter writeTag");
}

std::vector<std::uint8_t>
CborWriter::toBytes() const
{
  std::vector<std::uint8_t> bytes(cardano_cbor_writer_get_encode_size(checked()));
  std::uint8_t              empty = 0;

  checkCardanoError(cardano_cbor_writer_encode(checked(), bytes.empty() ? &empty : bytes.data(), bytes.size()), "CborWriter toBytes");

  return bytes;
}

std::string
CborWriter::toHex() const
{
  std::string text(cardano_cbor_writer_get_hex_size(checked()), '\0');

  checkCardanoError(cardano_cbor_writer_encode_hex(checked(), text.data(), text.size()), "CborWriter toHex");

  text.pop_back();

  return text;
}

void
CborWriter::reset()
{
  CborWriter empty;

  swap(empty);
}

void
CborWriter::swap(CborWriter& other) noexcept
{
  std::swap(m_value, other.m_value);
}

CborWriter&
CborWriter::operator=(const CborWriter& other)
{
  if (this != &other)
  {
    CborWriter copy(other);

    swap(copy);
  }

  return *this;
}

CborWriter&
CborWriter::operator=(CborWriter&& other) noexcept
{
  if (this != &other)
  {
    cardano_cbor_writer_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

CborWriter
CborWriter::adopt(cardano_cbor_writer_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null CborWriter");
  }

  return CborWriter(value);
}

cardano_cbor_writer_t*
CborWriter::nativeHandle() noexcept
{
  return m_value;
}

const cardano_cbor_writer_t*
CborWriter::nativeHandle() const noexcept
{
  return m_value;
}

cardano_cbor_writer_t*
CborWriter::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

CborWriter::CborWriter(cardano_cbor_writer_t* value) noexcept
: m_value(value)
{
}

cardano_cbor_writer_t*
CborWriter::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CborWriter");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CborWriter& lhs, CborWriter& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
