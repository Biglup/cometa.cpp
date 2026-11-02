/**
 * @file Metadatum.cpp
 *
 * @brief Implementation of the Metadatum value type.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/AuxiliaryData/Metadatum.h>

#include "../CardanoInterop.h"
#include <Cometa/AuxiliaryData/MetadatumList.h>
#include <Cometa/AuxiliaryData/MetadatumMap.h>
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Json/JsonWriter.h>

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/metadatum_list.h>
#include <cardano/auxiliary_data/metadatum_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Metadatum::Metadatum(std::int64_t integer)
: m_value(nullptr)
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_integer_from_int(integer, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum");

  m_value = owner.release();
}

Metadatum::Metadatum(const BigInt& integer)
: m_value(nullptr)
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_integer(integer.checked(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum");

  m_value = owner.release();
}

Metadatum::Metadatum(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum");

  m_value = owner.release();
}

Metadatum::Metadatum(const MetadatumList& data)
: m_value(nullptr)
{
  MetadatumList copy(data);

  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_list(copy.checked(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum");

  m_value = owner.release();
}

Metadatum::Metadatum(const MetadatumMap& data)
: m_value(nullptr)
{
  MetadatumMap copy(data);

  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_map(copy.checked(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum");

  m_value = owner.release();
}

Metadatum::Metadatum(std::string_view text)
: m_value(nullptr)
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_string(text.empty() ? "" : text.data(), text.size(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum text");

  m_value = owner.release();
}

Metadatum::Metadatum(const Metadatum& other)
: m_value(nullptr)
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_deep_clone(other.checked(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum");

  m_value = owner.release();
}

Metadatum::Metadatum(Metadatum&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Metadatum::~Metadatum()
{
  cardano_metadatum_unref(&m_value);
}

Metadatum
Metadatum::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Metadatum(*decodeCbor<cardano_metadatum_t, cardano_metadatum_unref>(cbor, cardano_metadatum_from_cbor));
}

Metadatum
Metadatum::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Metadatum
Metadatum::fromUnsigned(std::uint64_t integer)
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_new_integer_from_uint(integer, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "Metadatum fromUnsigned");

  return Metadatum(*owner.release());
}

Metadatum
Metadatum::fromHex(std::string_view hex)
{
  return Metadatum(decodeHex(hex));
}

Metadatum
Metadatum::fromPlainJson(std::string_view json)
{
  return fromJsonValue(JsonValue::parse(json));
}

MetadatumKind
Metadatum::kind() const
{
  cardano_metadatum_kind_t value = CARDANO_METADATUM_KIND_MAP;

  checkCardanoError(cardano_metadatum_get_kind(checked(), &value), "Metadatum kind");

  return static_cast<MetadatumKind>(value);
}

MetadatumList
Metadatum::asList() const
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_to_list(checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "Metadatum asList");

  const MetadatumList data(*owner.release());

  return MetadatumList(data);
}

MetadatumMap
Metadatum::asMap() const
{
  cardano_metadatum_map_t* value  = nullptr;
  const cardano_error_t    result = cardano_metadatum_to_map(checked(), &value);

  CardanoObject<cardano_metadatum_map_t, cardano_metadatum_map_unref> owner(value);

  checkCardanoError(result, "Metadatum asMap");

  const MetadatumMap data(*owner.release());

  return MetadatumMap(data);
}

std::string
Metadatum::asText() const
{
  if (kind() != MetadatumKind::Text)
  {
    throw Error(ErrorCode::InvalidMetadatumConversion, "Metadatum asText");
  }

  const std::size_t size = cardano_metadatum_get_string_size(checked());
  std::vector<char> text(size);

  checkCardanoError(cardano_metadatum_to_string(checked(), text.data(), text.size()), "Metadatum asText");

  return std::string(text.data(), size - 1U);
}

BigInt
Metadatum::asInteger() const
{
  cardano_bigint_t*     value  = nullptr;
  const cardano_error_t result = cardano_metadatum_to_integer(checked(), &value);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(value);

  checkCardanoError(result, "Metadatum asInteger");

  return BigInt(*owner.release());
}

std::vector<std::uint8_t>
Metadatum::asBytes() const
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_metadatum_to_bounded_bytes(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "Metadatum asBytes");

  const std::size_t length = cardano_buffer_get_size(value);
  if (length == 0U)
  {
    return {};
  }

  const std::uint8_t* bytes = cardano_buffer_get_data(value);

  return std::vector<std::uint8_t>(bytes, bytes + length);
}

std::vector<std::uint8_t>
Metadatum::toCbor() const
{
  return encodeCbor(checked(), cardano_metadatum_to_cbor);
}

std::string
Metadatum::toJson() const
{
  return encodeJson(checked(), cardano_metadatum_to_cip116_json);
}

std::string
Metadatum::toPlainJson() const
{
  JsonWriter writer;

  writePlainJson(writer);

  return writer.toString();
}

void
Metadatum::swap(Metadatum& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Metadatum&
Metadatum::operator=(const Metadatum& other)
{
  if (this != &other)
  {
    Metadatum copy(other);

    swap(copy);
  }

  return *this;
}

Metadatum&
Metadatum::operator=(Metadatum&& other) noexcept
{
  if (this != &other)
  {
    cardano_metadatum_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Metadatum::operator==(const Metadatum& other) const
{
  return cardano_metadatum_equals(checked(), other.checked());
}

bool
Metadatum::operator!=(const Metadatum& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Metadatum::Metadatum(cardano_metadatum_t& value) noexcept
: m_value(&value)
{
}

cardano_metadatum_t*
Metadatum::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Metadatum");
  }

  return m_value;
}

Metadatum
Metadatum::fromJsonValue(const JsonValue& value)
{
  switch (value.type())
  {
    case JsonType::Number:
    {
      if (value.isReal())
      {
        throw std::invalid_argument("Metadata JSON requires integer numbers");
      }

      return Metadatum(BigInt::fromString(value.toJson()));
    }
    case JsonType::String:
      return Metadatum(value.asString());
    case JsonType::Array:
    {
      MetadatumList     list;
      const std::size_t count = value.size();

      for (std::size_t index = 0U; index < count; ++index)
      {
        list.pushBack(fromJsonValue(value.at(index)));
      }

      return Metadatum(list);
    }
    case JsonType::Object:
    {
      MetadatumMap      map;
      const std::size_t count = value.size();

      for (std::size_t index = 0U; index < count; ++index)
      {
        map.insert(Metadatum(value.keyAt(index)), fromJsonValue(value.valueAt(index)));
      }

      return Metadatum(map);
    }
    default:
      throw std::invalid_argument("Metadata JSON does not support null or boolean values");
  }
}

void
Metadatum::writePlainJson(JsonWriter& writer) const
{
  switch (kind())
  {
    case MetadatumKind::Integer:
      writer.writeBigInt(asInteger());
      break;
    case MetadatumKind::Text:
      writer.writeString(asText());
      break;
    case MetadatumKind::List:
    {
      writer.writeStartArray();

      const std::vector<Metadatum> items = asList().values();

      for (const Metadatum& item: items)
      {
        item.writePlainJson(writer);
      }

      writer.writeEndArray();
      break;
    }
    case MetadatumKind::Map:
    {
      writer.writeStartObject();

      const std::vector<std::pair<Metadatum, Metadatum>> items = asMap().entries();

      for (const std::pair<Metadatum, Metadatum>& item: items)
      {
        if (item.first.kind() != MetadatumKind::Text)
        {
          throw std::invalid_argument("Metadata JSON map keys must be text");
        }

        writer.writePropertyName(item.first.asText());
        item.second.writePlainJson(writer);
      }

      writer.writeEndObject();
      break;
    }
    default:
      throw std::invalid_argument("Metadata JSON does not support byte strings");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Metadatum& lhs, Metadatum& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
