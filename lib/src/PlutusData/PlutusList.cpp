/**
 * @file PlutusList.cpp
 *
 * @brief Implementation of the PlutusList value type.
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

#include <Cometa/PlutusData/PlutusList.h>

#include "../CardanoInterop.h"

#include <cardano/plutus_data/plutus_data.h>
#include <cardano/plutus_data/plutus_list.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusList::PlutusList()
: m_value(nullptr)
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_list_new(&value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusList");

  m_value = owner.release();
}

PlutusList::PlutusList(const PlutusList& other)
: m_value(nullptr)
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusList");

  m_value = owner.release();
}

PlutusList::PlutusList(PlutusList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusList::~PlutusList()
{
  cardano_plutus_list_unref(&m_value);
}

PlutusList
PlutusList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusList(*decodeCbor<cardano_plutus_list_t, cardano_plutus_list_unref>(cbor, cardano_plutus_list_from_cbor));
}

PlutusList
PlutusList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusList::size() const
{
  return cardano_plutus_list_get_length(checked());
}

bool
PlutusList::empty() const
{
  return size() == 0U;
}

PlutusData
PlutusList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusList index");
  }

  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(result, "PlutusList at");

  const PlutusData data(*owner.release());

  return PlutusData(data);
}

void
PlutusList::pushBack(const PlutusData& data)
{
  PlutusData copy(data);

  checkCardanoError(cardano_plutus_list_add(checked(), copy.checked()), "PlutusList pushBack");
  normalizeEncoding();
}

std::vector<PlutusData>
PlutusList::values() const
{
  std::vector<PlutusData> result;
  const std::size_t       count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_list_to_cbor);
}

std::string
PlutusList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_list_to_cip116_json);
}

void
PlutusList::normalizeEncoding()
{
  cardano_plutus_list_clear_cbor_cache(checked());
}

void
PlutusList::swap(PlutusList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusList&
PlutusList::operator=(const PlutusList& other)
{
  if (this != &other)
  {
    PlutusList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusList&
PlutusList::operator=(PlutusList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusList::operator==(const PlutusList& other) const
{
  return cardano_plutus_list_equals(checked(), other.checked());
}

bool
PlutusList::operator!=(const PlutusList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusList::PlutusList(cardano_plutus_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_list_t*
PlutusList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusList");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusList& lhs, PlutusList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
