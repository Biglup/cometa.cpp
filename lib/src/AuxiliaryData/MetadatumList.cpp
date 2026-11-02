/**
 * @file MetadatumList.cpp
 *
 * @brief Implementation of the MetadatumList value type.
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

#include <Cometa/AuxiliaryData/MetadatumList.h>

#include "../CardanoInterop.h"

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/metadatum_list.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MetadatumList::MetadatumList()
: m_value(nullptr)
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_list_new(&value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumList");

  m_value = owner.release();
}

MetadatumList::MetadatumList(std::initializer_list<Metadatum> values)
: MetadatumList()
{
  for (const Metadatum& value: values)
  {
    pushBack(value);
  }
}

MetadatumList::MetadatumList(const MetadatumList& other)
: m_value(nullptr)
{
  cardano_metadatum_list_t* value  = nullptr;
  const cardano_error_t     result = cardano_metadatum_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_metadatum_list_t, cardano_metadatum_list_unref> owner(value);

  checkCardanoError(result, "MetadatumList");

  m_value = owner.release();
}

MetadatumList::MetadatumList(MetadatumList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

MetadatumList::~MetadatumList()
{
  cardano_metadatum_list_unref(&m_value);
}

MetadatumList
MetadatumList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return MetadatumList(*decodeCbor<cardano_metadatum_list_t, cardano_metadatum_list_unref>(cbor, cardano_metadatum_list_from_cbor));
}

MetadatumList
MetadatumList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
MetadatumList::size() const
{
  return cardano_metadatum_list_get_length(checked());
}

bool
MetadatumList::empty() const
{
  return size() == 0U;
}

Metadatum
MetadatumList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("MetadatumList index");
  }

  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_metadatum_list_get(checked(), index, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "MetadatumList at");

  const Metadatum data(*owner.release());

  return Metadatum(data);
}

void
MetadatumList::pushBack(const Metadatum& data)
{
  Metadatum copy(data);

  checkCardanoError(cardano_metadatum_list_add(checked(), copy.checked()), "MetadatumList pushBack");
}

std::vector<Metadatum>
MetadatumList::values() const
{
  std::vector<Metadatum> result;
  const std::size_t      count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
MetadatumList::toCbor() const
{
  return encodeCbor(checked(), cardano_metadatum_list_to_cbor);
}

std::string
MetadatumList::toJson() const
{
  return encodeJson(checked(), cardano_metadatum_list_to_cip116_json);
}

void
MetadatumList::swap(MetadatumList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

MetadatumList&
MetadatumList::operator=(const MetadatumList& other)
{
  if (this != &other)
  {
    MetadatumList copy(other);

    swap(copy);
  }

  return *this;
}

MetadatumList&
MetadatumList::operator=(MetadatumList&& other) noexcept
{
  if (this != &other)
  {
    cardano_metadatum_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
MetadatumList::operator==(const MetadatumList& other) const
{
  return cardano_metadatum_list_equals(checked(), other.checked());
}

bool
MetadatumList::operator!=(const MetadatumList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

MetadatumList::MetadatumList(cardano_metadatum_list_t& value) noexcept
: m_value(&value)
{
}

cardano_metadatum_list_t*
MetadatumList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MetadatumList");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MetadatumList& lhs, MetadatumList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
