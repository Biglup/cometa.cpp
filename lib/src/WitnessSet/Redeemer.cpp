/**
 * @file Redeemer.cpp
 *
 * @brief Script redeemer data and execution budget.
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

#include <Cometa/WitnessSet/Redeemer.h>

#include "../CardanoInterop.h"

#include <cardano/common/ex_units.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/witness_set/redeemer.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Redeemer::Redeemer(RedeemerTag tag, std::uint64_t index, const PlutusData& data, const ExUnits& exUnits)
: m_value(nullptr)
{
  const PlutusData                                          dataCopy(data);
  const ExUnits                                             unitsCopy(exUnits);
  cardano_redeemer_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_redeemer_new(static_cast<cardano_redeemer_tag_t>(tag), index, dataCopy.checked(), unitsCopy.checked(), &value);
  CardanoObject<cardano_redeemer_t, cardano_redeemer_unref> owner(value);

  checkCardanoError(status, "Redeemer");
  m_value = owner.release();
}

Redeemer::Redeemer(const Redeemer& other)
: m_value(nullptr)
{
  cardano_redeemer_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_redeemer_deep_clone(other.checked(), &value);
  CardanoObject<cardano_redeemer_t, cardano_redeemer_unref> owner(value);

  checkCardanoError(status, "Redeemer copy");
  m_value = owner.release();
}

Redeemer::Redeemer(Redeemer&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Redeemer::~Redeemer()
{
  cardano_redeemer_unref(&m_value);
}

Redeemer
Redeemer::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Redeemer(decodeCbor<cardano_redeemer_t, cardano_redeemer_unref>(cbor, cardano_redeemer_from_cbor));
}

Redeemer
Redeemer::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

RedeemerTag
Redeemer::tag() const
{
  return static_cast<RedeemerTag>(cardano_redeemer_get_tag(checked()));
}

void
Redeemer::setTag(RedeemerTag value)
{
  checkCardanoError(cardano_redeemer_set_tag(checked(), static_cast<cardano_redeemer_tag_t>(value)), "Redeemer setTag");
  cardano_redeemer_clear_cbor_cache(checked());
}

std::uint64_t
Redeemer::index() const
{
  return cardano_redeemer_get_index(checked());
}

void
Redeemer::setIndex(std::uint64_t value)
{
  checkCardanoError(cardano_redeemer_set_index(checked(), value), "Redeemer setIndex");
  cardano_redeemer_clear_cbor_cache(checked());
}

PlutusData
Redeemer::data() const
{
  cardano_plutus_data_t*                                          value = cardano_redeemer_get_data(checked());
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("Redeemer has no data");
  }

  const PlutusData found(*owner.release());
  return PlutusData(found);
}

void
Redeemer::setData(const PlutusData& value)
{
  const PlutusData copy(value);
  checkCardanoError(cardano_redeemer_set_data(checked(), copy.checked()), "Redeemer setData");
  cardano_redeemer_clear_cbor_cache(checked());
}

ExUnits
Redeemer::exUnits() const
{
  cardano_ex_units_t*                                       value = cardano_redeemer_get_ex_units(checked());
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("Redeemer has no execution budget");
  }

  const ExUnits found(owner.release());
  return ExUnits(found);
}

void
Redeemer::setExUnits(const ExUnits& value)
{
  const ExUnits copy(value);
  checkCardanoError(cardano_redeemer_set_ex_units(checked(), copy.checked()), "Redeemer setExUnits");
  cardano_redeemer_clear_cbor_cache(checked());
}

std::vector<std::uint8_t>
Redeemer::toCbor() const
{
  return encodeCbor(checked(), cardano_redeemer_to_cbor);
}

std::string
Redeemer::toJson() const
{
  return encodeJson(checked(), cardano_redeemer_to_cip116_json);
}

void
Redeemer::swap(Redeemer& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Redeemer&
Redeemer::operator=(const Redeemer& other)
{
  if (this != &other)
  {
    Redeemer copy(other);
    swap(copy);
  }
  return *this;
}

Redeemer&
Redeemer::operator=(Redeemer&& other) noexcept
{
  if (this != &other)
  {
    cardano_redeemer_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Redeemer::operator==(const Redeemer& other) const
{
  return toCbor() == other.toCbor();
}

bool
Redeemer::operator!=(const Redeemer& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Redeemer::Redeemer(cardano_redeemer_t* value) noexcept
: m_value(value)
{
}

cardano_redeemer_t*
Redeemer::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Redeemer");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Redeemer& lhs, Redeemer& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
