/**
 * @file BootstrapWitnessSet.cpp
 *
 * @brief Ledger bootstrap witness collections.
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

#include <Cometa/WitnessSet/BootstrapWitnessSet.h>

#include "../CardanoInterop.h"

#include <cardano/witness_set/bootstrap_witness.h>
#include <cardano/witness_set/bootstrap_witness_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
BootstrapWitnessSet::BootstrapWitnessSet()
: m_value(nullptr)
{
  cardano_bootstrap_witness_set_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_bootstrap_witness_set_new(&value);
  CardanoObject<cardano_bootstrap_witness_set_t, cardano_bootstrap_witness_set_unref> owner(value);

  checkCardanoError(status, "BootstrapWitnessSet");
  m_value = owner.release();
}

BootstrapWitnessSet::BootstrapWitnessSet(const BootstrapWitnessSet& other)
: m_value(nullptr)
{
  cardano_bootstrap_witness_set_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_bootstrap_witness_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_bootstrap_witness_set_t, cardano_bootstrap_witness_set_unref> owner(value);

  checkCardanoError(status, "BootstrapWitnessSet copy");
  m_value = owner.release();
}

BootstrapWitnessSet::BootstrapWitnessSet(BootstrapWitnessSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

BootstrapWitnessSet::~BootstrapWitnessSet()
{
  cardano_bootstrap_witness_set_unref(&m_value);
}

BootstrapWitnessSet
BootstrapWitnessSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return BootstrapWitnessSet(decodeCbor<cardano_bootstrap_witness_set_t, cardano_bootstrap_witness_set_unref>(cbor, cardano_bootstrap_witness_set_from_cbor));
}

BootstrapWitnessSet
BootstrapWitnessSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
BootstrapWitnessSet::size() const
{
  return cardano_bootstrap_witness_set_get_length(checked());
}

void
BootstrapWitnessSet::add(const BootstrapWitness& value)
{
  const BootstrapWitness copy(value);
  checkCardanoError(cardano_bootstrap_witness_set_add(checked(), copy.checked()), "BootstrapWitnessSet add");
}

BootstrapWitness
BootstrapWitnessSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("BootstrapWitness index is out of range");
  }

  cardano_bootstrap_witness_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_bootstrap_witness_set_get(checked(), index, &value);
  CardanoObject<cardano_bootstrap_witness_t, cardano_bootstrap_witness_unref> owner(value);

  checkCardanoError(status, "BootstrapWitnessSet at");
  const BootstrapWitness copied(owner.release());
  return BootstrapWitness(copied);
}

bool
BootstrapWitnessSet::useTag() const
{
  return cardano_bootstrap_witness_set_get_use_tag(checked());
}

void
BootstrapWitnessSet::setUseTag(bool value)
{
  checkCardanoError(cardano_bootstrap_witness_set_set_use_tag(checked(), value), "BootstrapWitnessSet setUseTag");
}

std::vector<std::uint8_t>
BootstrapWitnessSet::toCbor() const
{
  return encodeCbor(checked(), cardano_bootstrap_witness_set_to_cbor);
}

std::string
BootstrapWitnessSet::toJson() const
{
  return encodeJson(checked(), cardano_bootstrap_witness_set_to_cip116_json);
}

void
BootstrapWitnessSet::swap(BootstrapWitnessSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

BootstrapWitnessSet&
BootstrapWitnessSet::operator=(const BootstrapWitnessSet& other)
{
  if (this != &other)
  {
    BootstrapWitnessSet copy(other);
    swap(copy);
  }
  return *this;
}

BootstrapWitnessSet&
BootstrapWitnessSet::operator=(BootstrapWitnessSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_bootstrap_witness_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
BootstrapWitnessSet::operator==(const BootstrapWitnessSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
BootstrapWitnessSet::operator!=(const BootstrapWitnessSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

BootstrapWitnessSet::BootstrapWitnessSet(cardano_bootstrap_witness_set_t* value) noexcept
: m_value(value)
{
}

cardano_bootstrap_witness_set_t*
BootstrapWitnessSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from BootstrapWitnessSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(BootstrapWitnessSet& lhs, BootstrapWitnessSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
