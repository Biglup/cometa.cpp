/**
 * @file PlutusData.cpp
 *
 * @brief Implementation of the PlutusData value type.
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

#include <Cometa/PlutusData/PlutusData.h>

#include "../CardanoInterop.h"
#include <Cometa/PlutusData/ConstrPlutusData.h>
#include <Cometa/PlutusData/PlutusList.h>
#include <Cometa/PlutusData/PlutusMap.h>

#include <cardano/plutus_data/constr_plutus_data.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/plutus_data/plutus_list.h>
#include <cardano/plutus_data/plutus_map.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusData::PlutusData(std::int64_t integer)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_integer_from_int(integer, &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(const BigInt& integer)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_integer(integer.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(const std::vector<std::uint8_t>& bytes)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(const PlutusList& data)
: m_value(nullptr)
{
  PlutusList copy(data);

  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_list(copy.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(PlutusList&& data)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_list(data.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();

  PlutusList consumed(std::move(data));
}

PlutusData::PlutusData(const PlutusMap& data)
: m_value(nullptr)
{
  PlutusMap copy(data);

  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_map(copy.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(PlutusMap&& data)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_map(data.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();

  PlutusMap consumed(std::move(data));
}

PlutusData::PlutusData(const ConstrPlutusData& data)
: m_value(nullptr)
{
  ConstrPlutusData copy(data);

  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_constr(copy.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(ConstrPlutusData&& data)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_constr(data.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();

  ConstrPlutusData consumed(std::move(data));
}

PlutusData::PlutusData(const PlutusData& other)
: m_value(nullptr)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData");

  m_value = owner.release();
}

PlutusData::PlutusData(PlutusData&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusData::~PlutusData()
{
  cardano_plutus_data_unref(&m_value);
}

PlutusData
PlutusData::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusData(*decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(cbor, cardano_plutus_data_from_cbor));
}

PlutusData
PlutusData::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

PlutusData
PlutusData::fromUnsigned(std::uint64_t integer)
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_new_integer_from_uint(integer, &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData fromUnsigned");

  return PlutusData(*owner.release());
}

PlutusData
PlutusData::fromHex(std::string_view hex)
{
  return PlutusData(decodeHex(hex));
}

PlutusDataKind
PlutusData::kind() const
{
  cardano_plutus_data_kind_t value = CARDANO_PLUTUS_DATA_KIND_CONSTR;

  checkCardanoError(cardano_plutus_data_get_kind(checked(), &value), "PlutusData kind");

  return static_cast<PlutusDataKind>(value);
}

PlutusList
PlutusData::asList() const
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_data_to_list(checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusData asList");

  const PlutusList data(*owner.release());

  return PlutusList(data);
}

PlutusMap
PlutusData::asMap() const
{
  cardano_plutus_map_t* value  = nullptr;
  const cardano_error_t result = cardano_plutus_data_to_map(checked(), &value);

  CardanoObject<cardano_plutus_map_t, cardano_plutus_map_unref> owner(value);

  checkCardanoError(result, "PlutusData asMap");

  const PlutusMap data(*owner.release());

  return PlutusMap(data);
}

ConstrPlutusData
PlutusData::asConstructor() const
{
  cardano_constr_plutus_data_t* value  = nullptr;
  const cardano_error_t         result = cardano_plutus_data_to_constr(checked(), &value);

  CardanoObject<cardano_constr_plutus_data_t, cardano_constr_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusData asConstr");

  const ConstrPlutusData data(*owner.release());

  return ConstrPlutusData(data);
}

BigInt
PlutusData::asInteger() const
{
  cardano_bigint_t*     value  = nullptr;
  const cardano_error_t result = cardano_plutus_data_to_integer(checked(), &value);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(value);

  checkCardanoError(result, "PlutusData asInteger");

  return BigInt(*owner.release());
}

std::vector<std::uint8_t>
PlutusData::asBytes() const
{
  cardano_buffer_t*     value  = nullptr;
  const cardano_error_t result = cardano_plutus_data_to_bounded_bytes(checked(), &value);

  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(result, "PlutusData asBytes");

  const std::size_t length = cardano_buffer_get_size(value);
  if (length == 0U)
  {
    return {};
  }

  const std::uint8_t* bytes = cardano_buffer_get_data(value);

  return std::vector<std::uint8_t>(bytes, bytes + length);
}

std::vector<std::uint8_t>
PlutusData::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_data_to_cbor);
}

std::string
PlutusData::toJson() const
{
  return encodeJson(checked(), cardano_plutus_data_to_cip116_json);
}

void
PlutusData::normalizeEncoding()
{
  cardano_plutus_data_clear_cbor_cache(checked());
}

void
PlutusData::swap(PlutusData& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusData&
PlutusData::operator=(const PlutusData& other)
{
  if (this != &other)
  {
    PlutusData copy(other);

    swap(copy);
  }

  return *this;
}

PlutusData&
PlutusData::operator=(PlutusData&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_data_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusData::operator==(const PlutusData& other) const
{
  return cardano_plutus_data_equals(checked(), other.checked());
}

bool
PlutusData::operator!=(const PlutusData& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusData::PlutusData(cardano_plutus_data_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_data_t*
PlutusData::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusData");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusData& lhs, PlutusData& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
