/**
 * @file MetadatumMap.cpp
 *
 * @brief Implementation of the MetadatumMap value type.
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

#include <Cometa/AuxiliaryData/MetadatumMap.h>

#include "../CardanoInterop.h"
#include <Cometa/AuxiliaryData/MetadatumList.h>

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/metadatum_list.h>
#include <cardano/auxiliary_data/metadatum_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MetadatumMap::MetadatumMap()
: m_value(nullptr)
{
  cardano_metadatum_map_t* value  = nullptr;
  const cardano_error_t    result = cardano_metadatum_map_new(&value);

  CardanoObject<cardano_metadatum_map_t, cardano_metadatum_map_unref> owner(value);

  checkCardanoError(result, "MetadatumMap");

  m_value = owner.release();
}

MetadatumMap::MetadatumMap(const MetadatumMap& other)
: m_value(nullptr)
{
  cardano_metadatum_map_t* value  = nullptr;
  const cardano_error_t    result = cardano_metadatum_map_deep_clone(other.checked(), &value);

  CardanoObject<cardano_metadatum_map_t, cardano_metadatum_map_unref> owner(value);

  checkCardanoError(result, "MetadatumMap");

  m_value = owner.release();
}

MetadatumMap::MetadatumMap(MetadatumMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

MetadatumMap::~MetadatumMap()
{
  cardano_metadatum_map_unref(&m_value);
}

MetadatumMap
MetadatumMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return MetadatumMap(*decodeCbor<cardano_metadatum_map_t, cardano_metadatum_map_unref>(cbor, cardano_metadatum_map_from_cbor));
}

MetadatumMap
MetadatumMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
MetadatumMap::size() const
{
  return cardano_metadatum_map_get_length(checked());
}

bool
MetadatumMap::empty() const
{
  return size() == 0U;
}

std::optional<Metadatum>
MetadatumMap::get(const Metadatum& key) const
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_map_get(checked(), key.checked(), &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "MetadatumMap get");

  const Metadatum data(*owner.release());

  return Metadatum(data);
}

void
MetadatumMap::insert(const Metadatum& key, const Metadatum& value)
{
  Metadatum keyCopy(key);
  Metadatum valueCopy(value);

  checkCardanoError(cardano_metadatum_map_insert(checked(), keyCopy.checked(), valueCopy.checked()), "MetadatumMap insert");
}

std::vector<Metadatum>
MetadatumMap::keys() const
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_map_get_keys(checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumMap keys");

  const MetadatumList list(*owner.release());

  return list.values();
}

std::vector<Metadatum>
MetadatumMap::values() const
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_map_get_values(checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumMap values");

  const MetadatumList list(*owner.release());

  return list.values();
}

std::vector<std::pair<Metadatum, Metadatum>>
MetadatumMap::entries() const
{
  const std::vector<Metadatum>                 keyList   = keys();
  const std::vector<Metadatum>                 valueList = values();
  std::vector<std::pair<Metadatum, Metadatum>> result;

  result.reserve(keyList.size());

  for (std::size_t index = 0U; index < keyList.size(); ++index)
  {
    result.emplace_back(keyList[index], valueList[index]);
  }

  return result;
}

std::pair<Metadatum, Metadatum>
MetadatumMap::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("MetadatumMap index");
  }

  cardano_metadatum_t*  key    = nullptr;
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_map_get_at(checked(), index, &key, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> keyOwner(key);
  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> valueOwner(value);

  checkCardanoError(result, "MetadatumMap entryAt");

  const Metadatum keyValue(*keyOwner.release());
  const Metadatum dataValue(*valueOwner.release());

  return std::make_pair(Metadatum(keyValue), Metadatum(dataValue));
}

std::vector<std::uint8_t>
MetadatumMap::toCbor() const
{
  return encodeCbor(checked(), cardano_metadatum_map_to_cbor);
}

std::string
MetadatumMap::toJson() const
{
  return encodeJson(checked(), cardano_metadatum_map_to_cip116_json);
}

void
MetadatumMap::swap(MetadatumMap& other) noexcept
{
  std::swap(m_value, other.m_value);
}

MetadatumMap&
MetadatumMap::operator=(const MetadatumMap& other)
{
  if (this != &other)
  {
    MetadatumMap copy(other);

    swap(copy);
  }

  return *this;
}

MetadatumMap&
MetadatumMap::operator=(MetadatumMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_metadatum_map_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
MetadatumMap::operator==(const MetadatumMap& other) const
{
  return cardano_metadatum_map_equals(checked(), other.checked());
}

bool
MetadatumMap::operator!=(const MetadatumMap& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

MetadatumMap::MetadatumMap(cardano_metadatum_map_t& value) noexcept
: m_value(&value)
{
}

cardano_metadatum_map_t*
MetadatumMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MetadatumMap");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MetadatumMap& lhs, MetadatumMap& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
