/**
 * @file ConstrPlutusData.cpp
 *
 * @brief Implementation of the ConstrPlutusData value type.
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

#include <Cometa/PlutusData/ConstrPlutusData.h>

#include "../CardanoInterop.h"

#include <cardano/plutus_data/constr_plutus_data.h>
#include <cardano/plutus_data/plutus_list.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ConstrPlutusData::ConstrPlutusData(std::uint64_t alternative, const PlutusList& fields)
: m_value(nullptr)
{
  PlutusList copy(fields);

  cardano_constr_plutus_data_t* value  = nullptr;
  const cardano_error_t         result = cardano_constr_plutus_data_new(alternative, copy.checked(), &value);

  CardanoObject<cardano_constr_plutus_data_t, cardano_constr_plutus_data_unref> owner(value);

  checkCardanoError(result, "ConstrPlutusData");

  m_value = owner.release();
}

ConstrPlutusData::ConstrPlutusData(const ConstrPlutusData& other)
: m_value(nullptr)
{
  cardano_constr_plutus_data_t* value  = nullptr;
  const cardano_error_t         result = cardano_constr_plutus_data_deep_clone(other.checked(), &value);

  CardanoObject<cardano_constr_plutus_data_t, cardano_constr_plutus_data_unref> owner(value);

  checkCardanoError(result, "ConstrPlutusData");

  m_value = owner.release();
}

ConstrPlutusData::ConstrPlutusData(ConstrPlutusData&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ConstrPlutusData::~ConstrPlutusData()
{
  cardano_constr_plutus_data_unref(&m_value);
}

ConstrPlutusData
ConstrPlutusData::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ConstrPlutusData(*decodeCbor<cardano_constr_plutus_data_t, cardano_constr_plutus_data_unref>(cbor, cardano_constr_plutus_data_from_cbor));
}

ConstrPlutusData
ConstrPlutusData::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::uint64_t
ConstrPlutusData::alternative() const
{
  std::uint64_t value = 0U;

  checkCardanoError(cardano_constr_plutus_data_get_alternative(checked(), &value), "ConstrPlutusData alternative");

  return value;
}

void
ConstrPlutusData::setAlternative(std::uint64_t alternative)
{
  checkCardanoError(cardano_constr_plutus_data_set_alternative(checked(), alternative), "ConstrPlutusData setAlternative");
  normalizeEncoding();
}

PlutusList
ConstrPlutusData::fields() const
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_constr_plutus_data_get_data(checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "ConstrPlutusData fields");

  const PlutusList data(*owner.release());

  return PlutusList(data);
}

void
ConstrPlutusData::setFields(const PlutusList& fields)
{
  PlutusList copy(fields);

  checkCardanoError(cardano_constr_plutus_data_set_data(checked(), copy.checked()), "ConstrPlutusData setFields");
  normalizeEncoding();
}

std::vector<std::uint8_t>
ConstrPlutusData::toCbor() const
{
  return encodeCbor(checked(), cardano_constr_plutus_data_to_cbor);
}

std::string
ConstrPlutusData::toJson() const
{
  return encodeJson(checked(), cardano_constr_plutus_data_to_cip116_json);
}

void
ConstrPlutusData::normalizeEncoding()
{
  cardano_constr_plutus_data_clear_cbor_cache(checked());
}

void
ConstrPlutusData::swap(ConstrPlutusData& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ConstrPlutusData&
ConstrPlutusData::operator=(const ConstrPlutusData& other)
{
  if (this != &other)
  {
    ConstrPlutusData copy(other);

    swap(copy);
  }

  return *this;
}

ConstrPlutusData&
ConstrPlutusData::operator=(ConstrPlutusData&& other) noexcept
{
  if (this != &other)
  {
    cardano_constr_plutus_data_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ConstrPlutusData::operator==(const ConstrPlutusData& other) const
{
  return cardano_constr_plutus_equals(checked(), other.checked());
}

bool
ConstrPlutusData::operator!=(const ConstrPlutusData& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

ConstrPlutusData::ConstrPlutusData(cardano_constr_plutus_data_t& value) noexcept
: m_value(&value)
{
}

cardano_constr_plutus_data_t*
ConstrPlutusData::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ConstrPlutusData");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ConstrPlutusData& lhs, ConstrPlutusData& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
