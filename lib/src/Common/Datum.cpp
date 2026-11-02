/**
 * @file Datum.cpp
 *
 * @brief Implementation of the Datum value type.
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

#include <Cometa/Common/Datum.h>

#include "../CardanoInterop.h"

#include <cardano/common/datum.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Datum::Datum(const Blake2bHash& hash)
: m_value(nullptr)
{
  const std::vector<std::uint8_t> bytes = hash.toBytes();

  if (bytes.size() != 32U)
  {
    throw std::invalid_argument("Datum hash must be 32 bytes");
  }

  cardano_datum_t*      value  = nullptr;
  const cardano_error_t result = cardano_datum_new_data_hash_bytes(byteData(bytes), bytes.size(), &value);

  CardanoObject<cardano_datum_t, cardano_datum_unref> owner(value);

  checkCardanoError(result, "Datum");

  m_value = owner.release();
}

Datum::Datum(const PlutusData& data)
: m_value(nullptr)
{
  PlutusData copy(data);

  cardano_datum_t*      value  = nullptr;
  const cardano_error_t result = cardano_datum_new_inline_data(copy.checked(), &value);

  CardanoObject<cardano_datum_t, cardano_datum_unref> owner(value);

  checkCardanoError(result, "Datum");

  m_value = owner.release();
}

Datum::Datum(const Datum& other)
: m_value(nullptr)
{
  cardano_datum_t*      value  = nullptr;
  const cardano_error_t result = cardano_datum_deep_clone(other.checked(), &value);

  CardanoObject<cardano_datum_t, cardano_datum_unref> owner(value);

  checkCardanoError(result, "Datum");

  m_value = owner.release();
}

Datum::Datum(Datum&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Datum::~Datum()
{
  cardano_datum_unref(&m_value);
}

Datum
Datum::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Datum(*decodeCbor<cardano_datum_t, cardano_datum_unref>(cbor, cardano_datum_from_cbor));
}

Datum
Datum::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

DatumType
Datum::type() const
{
  cardano_datum_type_t value = CARDANO_DATUM_TYPE_DATA_HASH;

  checkCardanoError(cardano_datum_get_type(checked(), &value), "Datum type");

  return static_cast<DatumType>(value);
}

std::optional<PlutusData>
Datum::inlineData() const
{
  if (type() != DatumType::InlineData)
  {
    return std::nullopt;
  }

  const PlutusData data(*cardano_datum_get_inline_data(checked()));

  return PlutusData(data);
}

Blake2bHash
Datum::hash() const
{
  if (type() == DatumType::InlineData)
  {
    return Blake2bHash::compute(inlineData()->toCbor());
  }

  const std::uint8_t* bytes = cardano_datum_get_data_hash_bytes(checked());

  return Blake2bHash(std::vector<std::uint8_t>(bytes, bytes + 32U));
}

void
Datum::setHash(const Blake2bHash& hash)
{
  if (type() != DatumType::DataHash)
  {
    throw std::logic_error("Inline datum hashes are computed from their data");
  }

  Datum replacement(hash);

  swap(replacement);
}

std::vector<std::uint8_t>
Datum::toCbor() const
{
  return encodeCbor(checked(), cardano_datum_to_cbor);
}

std::string
Datum::toJson() const
{
  return encodeJson(checked(), cardano_datum_to_cip116_json);
}

void
Datum::swap(Datum& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Datum&
Datum::operator=(const Datum& other)
{
  if (this != &other)
  {
    Datum copy(other);

    swap(copy);
  }

  return *this;
}

Datum&
Datum::operator=(Datum&& other) noexcept
{
  if (this != &other)
  {
    cardano_datum_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Datum::operator==(const Datum& other) const
{
  return cardano_datum_equals(checked(), other.checked());
}

bool
Datum::operator!=(const Datum& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Datum::Datum(cardano_datum_t& value) noexcept
: m_value(&value)
{
}

cardano_datum_t*
Datum::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Datum");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Datum& lhs, Datum& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
