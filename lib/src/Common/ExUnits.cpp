/**
 * @file ExUnits.cpp
 *
 * @brief Execution budget for a Plutus script.
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

#include <Cometa/Common/ExUnits.h>

#include "../CardanoInterop.h"

#include <cardano/common/ex_units.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ExUnits::ExUnits(std::uint64_t memory, std::uint64_t cpuSteps)
: m_value(nullptr)
{
  cardano_ex_units_t*   value  = nullptr;
  const cardano_error_t result = cardano_ex_units_new(memory, cpuSteps, &value);

  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  checkCardanoError(result, "ExUnits");

  m_value = owner.release();
}

ExUnits::ExUnits(const ExUnits& other)
: m_value(nullptr)
{
  cardano_ex_units_t*   value  = nullptr;
  const cardano_error_t result = cardano_ex_units_deep_clone(other.checked(), &value);

  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  checkCardanoError(result, "ExUnits copy");

  m_value = owner.release();
}

ExUnits::ExUnits(ExUnits&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ExUnits::~ExUnits()
{
  cardano_ex_units_unref(&m_value);
}

ExUnits
ExUnits::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ExUnits(decodeCbor<cardano_ex_units_t, cardano_ex_units_unref>(cbor, cardano_ex_units_from_cbor));
}

ExUnits
ExUnits::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::uint64_t
ExUnits::memory() const
{
  return cardano_ex_units_get_memory(checked());
}

void
ExUnits::setMemory(std::uint64_t memory)
{
  checkCardanoError(cardano_ex_units_set_memory(checked(), memory), "ExUnits setMemory");
}

std::uint64_t
ExUnits::cpuSteps() const
{
  return cardano_ex_units_get_cpu_steps(checked());
}

void
ExUnits::setCpuSteps(std::uint64_t cpuSteps)
{
  checkCardanoError(cardano_ex_units_set_cpu_steps(checked(), cpuSteps), "ExUnits setCpuSteps");
}

std::vector<std::uint8_t>
ExUnits::toCbor() const
{
  return encodeCbor(checked(), cardano_ex_units_to_cbor);
}

std::string
ExUnits::toJson() const
{
  return encodeJson(checked(), cardano_ex_units_to_cip116_json);
}

void
ExUnits::swap(ExUnits& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ExUnits&
ExUnits::operator=(const ExUnits& other)
{
  if (this != &other)
  {
    ExUnits copy(other);

    swap(copy);
  }

  return *this;
}

ExUnits&
ExUnits::operator=(ExUnits&& other) noexcept
{
  if (this != &other)
  {
    cardano_ex_units_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ExUnits::operator==(const ExUnits& other) const
{
  return memory() == other.memory() && cpuSteps() == other.cpuSteps();
}

bool
ExUnits::operator!=(const ExUnits& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

ExUnits::ExUnits(cardano_ex_units_t* value) noexcept
: m_value(value)
{
}

cardano_ex_units_t*
ExUnits::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ExUnits");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ExUnits& lhs, ExUnits& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
