/**
 * @file VkeyWitnessSet.cpp
 *
 * @brief Ledger verification-key witness collections.
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

#include <Cometa/WitnessSet/VkeyWitnessSet.h>

#include "../CardanoInterop.h"

#include <cardano/witness_set/vkey_witness.h>
#include <cardano/witness_set/vkey_witness_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VkeyWitnessSet::VkeyWitnessSet()
: m_value(nullptr)
{
  cardano_vkey_witness_set_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_vkey_witness_set_new(&value);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(value);

  checkCardanoError(status, "VkeyWitnessSet");
  m_value = owner.release();
}

VkeyWitnessSet::VkeyWitnessSet(const VkeyWitnessSet& other)
: m_value(nullptr)
{
  cardano_vkey_witness_set_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_vkey_witness_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(value);

  checkCardanoError(status, "VkeyWitnessSet copy");
  m_value = owner.release();
}

VkeyWitnessSet::VkeyWitnessSet(VkeyWitnessSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

VkeyWitnessSet::~VkeyWitnessSet()
{
  cardano_vkey_witness_set_unref(&m_value);
}

VkeyWitnessSet
VkeyWitnessSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VkeyWitnessSet(decodeCbor<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref>(cbor, cardano_vkey_witness_set_from_cbor));
}

VkeyWitnessSet
VkeyWitnessSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
VkeyWitnessSet::size() const
{
  return cardano_vkey_witness_set_get_length(checked());
}

void
VkeyWitnessSet::add(const VkeyWitness& value)
{
  const VkeyWitness copy(value);
  checkCardanoError(cardano_vkey_witness_set_add(checked(), copy.checked()), "VkeyWitnessSet add");
}

VkeyWitness
VkeyWitnessSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("VkeyWitness index is out of range");
  }

  cardano_vkey_witness_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_vkey_witness_set_get(checked(), index, &value);
  CardanoObject<cardano_vkey_witness_t, cardano_vkey_witness_unref> owner(value);

  checkCardanoError(status, "VkeyWitnessSet at");
  const VkeyWitness copied(owner.release());
  return VkeyWitness(copied);
}

void
VkeyWitnessSet::apply(const VkeyWitnessSet& other)
{
  VkeyWitnessSet       updated(*this);
  const VkeyWitnessSet copy(other);
  checkCardanoError(cardano_vkey_witness_set_apply(updated.checked(), copy.checked()), "VkeyWitnessSet apply");
  swap(updated);
}

bool
VkeyWitnessSet::useTag() const
{
  return cardano_vkey_witness_set_get_use_tag(checked());
}

void
VkeyWitnessSet::setUseTag(bool value)
{
  checkCardanoError(cardano_vkey_witness_set_set_use_tag(checked(), value), "VkeyWitnessSet setUseTag");
}

std::vector<std::uint8_t>
VkeyWitnessSet::toCbor() const
{
  return encodeCbor(checked(), cardano_vkey_witness_set_to_cbor);
}

std::string
VkeyWitnessSet::toJson() const
{
  return encodeJson(checked(), cardano_vkey_witness_set_to_cip116_json);
}

void
VkeyWitnessSet::swap(VkeyWitnessSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

VkeyWitnessSet&
VkeyWitnessSet::operator=(const VkeyWitnessSet& other)
{
  if (this != &other)
  {
    VkeyWitnessSet copy(other);
    swap(copy);
  }
  return *this;
}

VkeyWitnessSet&
VkeyWitnessSet::operator=(VkeyWitnessSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_vkey_witness_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
VkeyWitnessSet::operator==(const VkeyWitnessSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
VkeyWitnessSet::operator!=(const VkeyWitnessSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

VkeyWitnessSet::VkeyWitnessSet(cardano_vkey_witness_set_t* value) noexcept
: m_value(value)
{
}

cardano_vkey_witness_set_t*
VkeyWitnessSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VkeyWitnessSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VkeyWitnessSet& lhs, VkeyWitnessSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
