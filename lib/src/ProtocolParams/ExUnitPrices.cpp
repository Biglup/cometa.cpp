/**
 * @file ExUnitPrices.cpp
 *
 * @brief Prices for Plutus execution units.
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

#include <Cometa/ProtocolParams/ExUnitPrices.h>

#include "../CardanoInterop.h"

#include <cardano/common/unit_interval.h>
#include <cardano/protocol_params/ex_unit_prices.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ExUnitPrices::ExUnitPrices(const UnitInterval& memoryPrice, const UnitInterval& stepsPrice)
: m_value(nullptr)
{
  const UnitInterval                                                    memoryCopy(memoryPrice);
  const UnitInterval                                                    stepsCopy(stepsPrice);
  cardano_ex_unit_prices_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_ex_unit_prices_new(memoryCopy.checked(), stepsCopy.checked(), &value);
  CardanoObject<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref> owner(value);

  checkCardanoError(status, "ExUnitPrices");
  m_value = owner.release();
}

ExUnitPrices::ExUnitPrices(const ExUnitPrices& other)
: m_value(nullptr)
{
  cardano_ex_unit_prices_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_ex_unit_prices_deep_clone(other.checked(), &value);
  CardanoObject<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref> owner(value);

  checkCardanoError(status, "ExUnitPrices copy");
  m_value = owner.release();
}

ExUnitPrices::ExUnitPrices(ExUnitPrices&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ExUnitPrices::~ExUnitPrices()
{
  cardano_ex_unit_prices_unref(&m_value);
}

ExUnitPrices
ExUnitPrices::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ExUnitPrices(decodeCbor<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref>(cbor, cardano_ex_unit_prices_from_cbor));
}

ExUnitPrices
ExUnitPrices::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

UnitInterval
ExUnitPrices::memoryPrice() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_ex_unit_prices_get_memory_prices(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "ExUnitPrices memoryPrice");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

UnitInterval
ExUnitPrices::stepsPrice() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_ex_unit_prices_get_steps_prices(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "ExUnitPrices stepsPrice");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
ExUnitPrices::setMemoryPrice(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_ex_unit_prices_set_memory_prices(checked(), copy.checked()), "ExUnitPrices setMemoryPrice");
}

void
ExUnitPrices::setStepsPrice(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_ex_unit_prices_set_steps_prices(checked(), copy.checked()), "ExUnitPrices setStepsPrice");
}

std::vector<std::uint8_t>
ExUnitPrices::toCbor() const
{
  return encodeCbor(checked(), cardano_ex_unit_prices_to_cbor);
}

std::string
ExUnitPrices::toJson() const
{
  return encodeJson(checked(), cardano_ex_unit_prices_to_cip116_json);
}

void
ExUnitPrices::swap(ExUnitPrices& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ExUnitPrices&
ExUnitPrices::operator=(const ExUnitPrices& other)
{
  if (this != &other)
  {
    ExUnitPrices copy(other);
    swap(copy);
  }
  return *this;
}

ExUnitPrices&
ExUnitPrices::operator=(ExUnitPrices&& other) noexcept
{
  if (this != &other)
  {
    cardano_ex_unit_prices_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ExUnitPrices::operator==(const ExUnitPrices& other) const
{
  return toCbor() == other.toCbor();
}

bool
ExUnitPrices::operator!=(const ExUnitPrices& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

ExUnitPrices::ExUnitPrices(cardano_ex_unit_prices_t* value) noexcept
: m_value(value)
{
}

cardano_ex_unit_prices_t*
ExUnitPrices::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ExUnitPrices");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ExUnitPrices& lhs, ExUnitPrices& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
