/**
 * @file UnitInterval.cpp
 *
 * @brief An unsigned rational number encoded with CBOR tag 30.
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

#include <Cometa/Common/UnitInterval.h>

#include "../CardanoInterop.h"

#include <cardano/common/unit_interval.h>

#include <cmath>
#include <limits>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
UnitInterval::UnitInterval(std::uint64_t numerator, std::uint64_t denominator)
: m_value(nullptr)
{
  cardano_unit_interval_t* value  = nullptr;
  const cardano_error_t    result = cardano_unit_interval_new(numerator, denominator, &value);

  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(result, "UnitInterval");

  m_value = owner.release();
}

UnitInterval::UnitInterval(const UnitInterval& other)
: m_value(nullptr)
{
  cardano_unit_interval_t* value  = nullptr;
  const cardano_error_t    result = cardano_unit_interval_deep_clone(other.checked(), &value);

  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(result, "UnitInterval copy");

  m_value = owner.release();
}

UnitInterval::UnitInterval(UnitInterval&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

UnitInterval::~UnitInterval()
{
  cardano_unit_interval_unref(&m_value);
}

UnitInterval
UnitInterval::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return UnitInterval(decodeCbor<cardano_unit_interval_t, cardano_unit_interval_unref>(cbor, cardano_unit_interval_from_cbor));
}

UnitInterval
UnitInterval::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

UnitInterval
UnitInterval::fromDouble(double value)
{
  if (!std::isfinite(value) || value < 0.0)
  {
    throw std::invalid_argument("UnitInterval requires a finite nonnegative number");
  }

  if (value >= std::ldexp(1.0, 64))
  {
    throw std::overflow_error("UnitInterval numerator exceeds 64 bits");
  }

  double              integral = 0.0;
  const double        fraction = std::modf(value, &integral);
  const std::uint64_t whole    = static_cast<std::uint64_t>(integral);

  cardano_unit_interval_t* parsed = nullptr;
  const cardano_error_t    result = cardano_unit_interval_from_double(fraction, &parsed);

  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(parsed);

  checkCardanoError(result, "UnitInterval fromDouble");

  UnitInterval        converted(owner.release());
  const std::uint64_t fractionalNumerator   = converted.numerator();
  const std::uint64_t fractionalDenominator = converted.denominator();

  if (whole > (std::numeric_limits<std::uint64_t>::max() - fractionalNumerator) / fractionalDenominator)
  {
    throw std::overflow_error("UnitInterval numerator exceeds 64 bits");
  }

  converted.setNumerator(whole * fractionalDenominator + fractionalNumerator);

  return converted;
}

double
UnitInterval::toDouble() const
{
  if (denominator() == 0)
  {
    throw std::domain_error("Cannot divide by a zero denominator");
  }

  return cardano_unit_interval_to_double(checked());
}

std::uint64_t
UnitInterval::numerator() const
{
  return cardano_unit_interval_get_numerator(checked());
}

void
UnitInterval::setNumerator(std::uint64_t numerator)
{
  checkCardanoError(cardano_unit_interval_set_numerator(checked(), numerator), "UnitInterval setNumerator");
}

std::uint64_t
UnitInterval::denominator() const
{
  return cardano_unit_interval_get_denominator(checked());
}

void
UnitInterval::setDenominator(std::uint64_t denominator)
{
  checkCardanoError(cardano_unit_interval_set_denominator(checked(), denominator), "UnitInterval setDenominator");
}

std::vector<std::uint8_t>
UnitInterval::toCbor() const
{
  return encodeCbor(checked(), cardano_unit_interval_to_cbor);
}

std::string
UnitInterval::toJson() const
{
  return encodeJson(checked(), cardano_unit_interval_to_cip116_json);
}

void
UnitInterval::swap(UnitInterval& other) noexcept
{
  std::swap(m_value, other.m_value);
}

UnitInterval&
UnitInterval::operator=(const UnitInterval& other)
{
  if (this != &other)
  {
    UnitInterval copy(other);

    swap(copy);
  }

  return *this;
}

UnitInterval&
UnitInterval::operator=(UnitInterval&& other) noexcept
{
  if (this != &other)
  {
    cardano_unit_interval_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
UnitInterval::operator==(const UnitInterval& other) const
{
  return numerator() == other.numerator() && denominator() == other.denominator();
}

bool
UnitInterval::operator!=(const UnitInterval& other) const
{
  return !(*this == other);
}

UnitInterval
UnitInterval::adopt(cardano_unit_interval_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null UnitInterval");
  }

  return UnitInterval(value);
}

cardano_unit_interval_t*
UnitInterval::nativeHandle() noexcept
{
  return m_value;
}

const cardano_unit_interval_t*
UnitInterval::nativeHandle() const noexcept
{
  return m_value;
}

cardano_unit_interval_t*
UnitInterval::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

UnitInterval::UnitInterval(cardano_unit_interval_t* value) noexcept
: m_value(value)
{
}

cardano_unit_interval_t*
UnitInterval::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from UnitInterval");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(UnitInterval& lhs, UnitInterval& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
