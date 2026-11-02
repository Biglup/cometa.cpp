/**
 * @file PlutusMap.cpp
 *
 * @brief Implementation of the PlutusMap value type.
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

#include <Cometa/PlutusData/PlutusMap.h>

#include "../CardanoInterop.h"
#include <Cometa/PlutusData/PlutusList.h>

#include <cardano/plutus_data/plutus_data.h>
#include <cardano/plutus_data/plutus_list.h>
#include <cardano/plutus_data/plutus_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusMap::PlutusMap()
: m_value(nullptr)
{
  cardano_plutus_map_t* value  = nullptr;
  const cardano_error_t result = cardano_plutus_map_new(&value);

  CardanoObject<cardano_plutus_map_t, cardano_plutus_map_unref> owner(value);

  checkCardanoError(result, "PlutusMap");

  m_value = owner.release();
}

PlutusMap::PlutusMap(const PlutusMap& other)
: m_value(nullptr)
{
  cardano_plutus_map_t* value  = nullptr;
  const cardano_error_t result = cardano_plutus_map_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_map_t, cardano_plutus_map_unref> owner(value);

  checkCardanoError(result, "PlutusMap");

  m_value = owner.release();
}

PlutusMap::PlutusMap(PlutusMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusMap::~PlutusMap()
{
  cardano_plutus_map_unref(&m_value);
}

PlutusMap
PlutusMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusMap(*decodeCbor<cardano_plutus_map_t, cardano_plutus_map_unref>(cbor, cardano_plutus_map_from_cbor));
}

PlutusMap
PlutusMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusMap::size() const
{
  return cardano_plutus_map_get_length(checked());
}

bool
PlutusMap::empty() const
{
  return size() == 0U;
}

std::optional<PlutusData>
PlutusMap::get(const PlutusData& key) const
{
  cardano_plutus_data_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_map_get(checked(), key.checked(), &value);

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "PlutusMap get");

  const PlutusData data(*owner.release());

  return PlutusData(data);
}

void
PlutusMap::insert(const PlutusData& key, const PlutusData& value)
{
  PlutusData keyCopy(key);
  PlutusData valueCopy(value);
  PlutusMap  replacement(*this);

  checkCardanoError(cardano_plutus_map_insert(replacement.checked(), keyCopy.checked(), valueCopy.checked()), "PlutusMap insert");
  replacement.normalizeEncoding();
  swap(replacement);
}

std::vector<PlutusData>
PlutusMap::keys() const
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_map_get_keys(checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusMap keys");

  const PlutusList list(*owner.release());

  return list.values();
}

std::vector<PlutusData>
PlutusMap::values() const
{
  cardano_plutus_list_t* value  = nullptr;
  const cardano_error_t  result = cardano_plutus_map_get_values(checked(), &value);

  CardanoObject<cardano_plutus_list_t, cardano_plutus_list_unref> owner(value);

  checkCardanoError(result, "PlutusMap values");

  const PlutusList list(*owner.release());

  return list.values();
}

std::vector<std::pair<PlutusData, PlutusData>>
PlutusMap::entries() const
{
  const std::vector<PlutusData>                  keyList   = keys();
  const std::vector<PlutusData>                  valueList = values();
  std::vector<std::pair<PlutusData, PlutusData>> result;

  result.reserve(keyList.size());

  for (std::size_t index = 0U; index < keyList.size(); ++index)
  {
    result.emplace_back(keyList[index], valueList[index]);
  }

  return result;
}

std::vector<std::uint8_t>
PlutusMap::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_map_to_cbor);
}

std::string
PlutusMap::toJson() const
{
  return encodeJson(checked(), cardano_plutus_map_to_cip116_json);
}

void
PlutusMap::normalizeEncoding()
{
  cardano_plutus_map_clear_cbor_cache(checked());
}

void
PlutusMap::swap(PlutusMap& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusMap&
PlutusMap::operator=(const PlutusMap& other)
{
  if (this != &other)
  {
    PlutusMap copy(other);

    swap(copy);
  }

  return *this;
}

PlutusMap&
PlutusMap::operator=(PlutusMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_map_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusMap::operator==(const PlutusMap& other) const
{
  return cardano_plutus_map_equals(checked(), other.checked());
}

bool
PlutusMap::operator!=(const PlutusMap& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusMap::PlutusMap(cardano_plutus_map_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_map_t*
PlutusMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusMap");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusMap& lhs, PlutusMap& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
