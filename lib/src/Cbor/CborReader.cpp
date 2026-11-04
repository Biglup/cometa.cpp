/**
 * @file CborReader.cpp
 *
 * @brief Streaming CBOR reader.
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

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"

#include <cardano/cbor/cbor_reader.h>

#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CborReader::CborReader(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  if (bytes.empty())
  {
    throw std::invalid_argument("CBOR input must not be empty");
  }

  m_value = cardano_cbor_reader_new(bytes.data(), bytes.size());

  if (!m_value)
  {
    throw std::bad_alloc();
  }
}

CborReader::CborReader(std::string_view hex)
: CborReader(decodeHex(hex))
{
}

CborReader::CborReader(const CborReader& other)
: m_value(nullptr)
{
  cardano_cbor_reader_t* value  = nullptr;
  const cardano_error_t  result = cardano_cbor_reader_deep_clone(other.checked(), &value);

  CardanoObject<cardano_cbor_reader_t, cardano_cbor_reader_unref> owner(value);

  checkCardanoError(result, "CborReader copy");

  m_value = owner.release();
}

CborReader::CborReader(CborReader&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

CborReader::~CborReader()
{
  cardano_cbor_reader_unref(&m_value);
}

CborReaderState
CborReader::peekState() const
{
  cardano_cbor_reader_state_t value = CARDANO_CBOR_READER_STATE_UNDEFINED;

  checkCardanoError(cardano_cbor_reader_peek_state(checked(), &value), "CborReader peekState");

  return static_cast<CborReaderState>(value);
}

std::size_t
CborReader::bytesRemaining() const
{
  std::size_t value = 0;

  checkCardanoError(cardano_cbor_reader_get_bytes_remaining(checked(), &value), "CborReader bytesRemaining");

  return value;
}

std::int64_t
CborReader::readInt()
{
  const bool          negative  = peekState() == CborReaderState::NegativeInteger;
  const std::uint64_t magnitude = negative ? readNegativeInt() : readUInt();

  if (magnitude > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
  {
    throw std::out_of_range("CBOR integer exceeds the signed 64-bit range");
  }

  const std::int64_t value = static_cast<std::int64_t>(magnitude);

  return negative ? -1 - value : value;
}

std::uint64_t
CborReader::readUInt()
{
  std::uint64_t value = 0;

  checkCardanoError(cardano_cbor_reader_read_uint(checked(), &value), "CborReader readUInt");

  return value;
}

std::uint64_t
CborReader::readNegativeInt()
{
  std::uint64_t value = 0;

  checkCardanoError(cardano_cbor_reader_read_negative_int(checked(), &value), "CborReader readNegativeInt");

  return value;
}

double
CborReader::readDouble()
{
  double value = 0.0;

  checkCardanoError(cardano_cbor_reader_read_double(checked(), &value), "CborReader readDouble");

  return value;
}

bool
CborReader::readBool()
{
  bool value = false;

  checkCardanoError(cardano_cbor_reader_read_bool(checked(), &value), "CborReader readBool");

  return value;
}

CborSimpleValue
CborReader::readSimpleValue()
{
  cardano_cbor_simple_value_t value = CARDANO_CBOR_SIMPLE_VALUE_UNDEFINED;

  checkCardanoError(cardano_cbor_reader_read_simple_value(checked(), &value), "CborReader readSimpleValue");

  return static_cast<CborSimpleValue>(value);
}

void
CborReader::skipValue()
{
  checkCardanoError(cardano_cbor_reader_skip_value(checked()), "CborReader skipValue");
}

void
CborReader::readEndArray()
{
  checkCardanoError(cardano_cbor_reader_read_end_array(checked()), "CborReader readEndArray");
}

void
CborReader::readEndMap()
{
  checkCardanoError(cardano_cbor_reader_read_end_map(checked()), "CborReader readEndMap");
}

void
CborReader::readNull()
{
  checkCardanoError(cardano_cbor_reader_read_null(checked()), "CborReader readNull");
}

std::optional<std::uint64_t>
CborReader::readStartArray()
{
  std::int64_t size = 0;

  checkCardanoError(cardano_cbor_reader_read_start_array(checked(), &size), "CborReader readStartArray");

  if (size < 0)
  {
    return std::nullopt;
  }

  return static_cast<std::uint64_t>(size);
}

std::optional<std::uint64_t>
CborReader::readStartMap()
{
  std::int64_t size = 0;

  checkCardanoError(cardano_cbor_reader_read_start_map(checked(), &size), "CborReader readStartMap");

  if (size < 0)
  {
    return std::nullopt;
  }

  return static_cast<std::uint64_t>(size);
}

std::vector<std::uint8_t>
CborReader::remainderBytes() const
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_cbor_reader_get_remainder_bytes(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "CborReader remainderBytes");

  const std::size_t size = cardano_buffer_get_size(owner.get());

  if (size == 0)
  {
    return {};
  }

  const std::uint8_t* data = cardano_buffer_get_data(owner.get());

  return std::vector<std::uint8_t>(data, data + size);
}

std::vector<std::uint8_t>
CborReader::readEncodedValue()
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_cbor_reader_read_encoded_value(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "CborReader readEncodedValue");

  const std::size_t size = cardano_buffer_get_size(owner.get());

  if (size == 0)
  {
    return {};
  }

  const std::uint8_t* data = cardano_buffer_get_data(owner.get());

  return std::vector<std::uint8_t>(data, data + size);
}

std::vector<std::uint8_t>
CborReader::readBytes()
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_cbor_reader_read_bytestring(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "CborReader readBytes");

  const std::size_t size = cardano_buffer_get_size(owner.get());

  if (size == 0)
  {
    return {};
  }

  const std::uint8_t* data = cardano_buffer_get_data(owner.get());

  return std::vector<std::uint8_t>(data, data + size);
}

std::string
CborReader::readText()
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_cbor_reader_read_textstring(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "CborReader readText");

  const std::size_t size = cardano_buffer_get_size(owner.get());

  if (size == 0)
  {
    return {};
  }

  const std::uint8_t* data = cardano_buffer_get_data(owner.get());

  return std::string(reinterpret_cast<const char*>(data), size);
}

BigInt
CborReader::readBigInt()
{
  cardano_bigint_t*     value  = nullptr;
  const cardano_error_t result = cardano_cbor_reader_read_bigint(checked(), &value);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(value);

  checkCardanoError(result, "CborReader readBigInt");

  return BigInt::adopt(owner.release());
}

CborTag
CborReader::peekTag() const
{
  cardano_cbor_tag_t tag = CARDANO_CBOR_TAG_DATE_TIME_STRING;

  const cardano_error_t result = cardano_cbor_reader_peek_tag(checked(), &tag);

  if (result == CARDANO_ERROR_INTEGER_OVERFLOW)
  {
    throw std::out_of_range("CBOR tag exceeds the supported range");
  }

  checkCardanoError(result, "CborReader peekTag");

  return static_cast<CborTag>(tag);
}

CborTag
CborReader::readTag()
{
  const CborTag      result = peekTag();
  cardano_cbor_tag_t tag    = CARDANO_CBOR_TAG_DATE_TIME_STRING;

  checkCardanoError(cardano_cbor_reader_read_tag(checked(), &tag), "CborReader readTag");

  return result;
}

void
CborReader::swap(CborReader& other) noexcept
{
  std::swap(m_value, other.m_value);
}

CborReader&
CborReader::operator=(const CborReader& other)
{
  if (this != &other)
  {
    CborReader copy(other);

    swap(copy);
  }

  return *this;
}

CborReader&
CborReader::operator=(CborReader&& other) noexcept
{
  if (this != &other)
  {
    cardano_cbor_reader_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

CborReader
CborReader::adopt(cardano_cbor_reader_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null CborReader");
  }

  return CborReader(value);
}

cardano_cbor_reader_t*
CborReader::nativeHandle() noexcept
{
  return m_value;
}

const cardano_cbor_reader_t*
CborReader::nativeHandle() const noexcept
{
  return m_value;
}

cardano_cbor_reader_t*
CborReader::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

CborReader::CborReader(cardano_cbor_reader_t* value) noexcept
: m_value(value)
{
}

cardano_cbor_reader_t*
CborReader::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CborReader");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CborReader& lhs, CborReader& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
