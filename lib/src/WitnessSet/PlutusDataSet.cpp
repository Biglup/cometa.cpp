/**
 * @file PlutusDataSet.cpp
 *
 * @brief Ledger Plutus data value collections.
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

#include <Cometa/WitnessSet/PlutusDataSet.h>

#include "../CardanoInterop.h"

#include <cardano/plutus_data/plutus_data.h>
#include <cardano/witness_set/plutus_data_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusDataSet::PlutusDataSet()
: m_value(nullptr)
{
  cardano_plutus_data_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_plutus_data_set_new(&value);
  CardanoObject<cardano_plutus_data_set_t, cardano_plutus_data_set_unref> owner(value);

  checkCardanoError(status, "PlutusDataSet");
  m_value = owner.release();
}

PlutusDataSet::PlutusDataSet(const PlutusDataSet& other)
: m_value(nullptr)
{
  cardano_plutus_data_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_plutus_data_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_plutus_data_set_t, cardano_plutus_data_set_unref> owner(value);

  checkCardanoError(status, "PlutusDataSet copy");
  m_value = owner.release();
}

PlutusDataSet::PlutusDataSet(PlutusDataSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusDataSet::~PlutusDataSet()
{
  cardano_plutus_data_set_unref(&m_value);
}

PlutusDataSet
PlutusDataSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusDataSet(decodeCbor<cardano_plutus_data_set_t, cardano_plutus_data_set_unref>(cbor, cardano_plutus_data_set_from_cbor));
}

PlutusDataSet
PlutusDataSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusDataSet::size() const
{
  return cardano_plutus_data_set_get_length(checked());
}

void
PlutusDataSet::add(const PlutusData& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index) == value)
    {
      return;
    }
  }

  const PlutusData copy(value);
  checkCardanoError(cardano_plutus_data_set_add(checked(), copy.checked()), "PlutusDataSet add");
  const bool tagged = useTag();
  cardano_plutus_data_set_clear_cbor_cache(checked());
  setUseTag(tagged);
}

PlutusData
PlutusDataSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusData index is out of range");
  }

  cardano_plutus_data_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_plutus_data_set_get(checked(), index, &value);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(status, "PlutusDataSet at");
  const PlutusData copied(*owner.release());
  return PlutusData(copied);
}

bool
PlutusDataSet::useTag() const
{
  return cardano_plutus_data_set_get_use_tag(checked());
}

void
PlutusDataSet::setUseTag(bool value)
{
  cardano_plutus_data_set_clear_cbor_cache(checked());
  checkCardanoError(cardano_plutus_data_set_set_use_tag(checked(), value), "PlutusDataSet setUseTag");
}

std::vector<std::uint8_t>
PlutusDataSet::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_data_set_to_cbor);
}

std::string
PlutusDataSet::toJson() const
{
  return encodeJson(checked(), cardano_plutus_data_set_to_cip116_json);
}

void
PlutusDataSet::swap(PlutusDataSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusDataSet&
PlutusDataSet::operator=(const PlutusDataSet& other)
{
  if (this != &other)
  {
    PlutusDataSet copy(other);
    swap(copy);
  }
  return *this;
}

PlutusDataSet&
PlutusDataSet::operator=(PlutusDataSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_data_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
PlutusDataSet::operator==(const PlutusDataSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
PlutusDataSet::operator!=(const PlutusDataSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusDataSet::PlutusDataSet(cardano_plutus_data_set_t* value) noexcept
: m_value(value)
{
}

cardano_plutus_data_set_t*
PlutusDataSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusDataSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusDataSet& lhs, PlutusDataSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
