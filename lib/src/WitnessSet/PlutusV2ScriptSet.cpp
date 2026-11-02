/**
 * @file PlutusV2ScriptSet.cpp
 *
 * @brief Ledger Plutus V2 script collections.
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

#include <Cometa/WitnessSet/PlutusV2ScriptSet.h>

#include "../CardanoInterop.h"

#include <cardano/scripts/plutus_scripts/plutus_v2_script.h>
#include <cardano/witness_set/plutus_v2_script_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV2ScriptSet::PlutusV2ScriptSet()
: m_value(nullptr)
{
  cardano_plutus_v2_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v2_script_set_new(&value);
  CardanoObject<cardano_plutus_v2_script_set_t, cardano_plutus_v2_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV2ScriptSet");
  m_value = owner.release();
}

PlutusV2ScriptSet::PlutusV2ScriptSet(const PlutusV2ScriptSet& other)
: m_value(nullptr)
{
  cardano_plutus_v2_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v2_script_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_plutus_v2_script_set_t, cardano_plutus_v2_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV2ScriptSet copy");
  m_value = owner.release();
}

PlutusV2ScriptSet::PlutusV2ScriptSet(PlutusV2ScriptSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV2ScriptSet::~PlutusV2ScriptSet()
{
  cardano_plutus_v2_script_set_unref(&m_value);
}

PlutusV2ScriptSet
PlutusV2ScriptSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusV2ScriptSet(decodeCbor<cardano_plutus_v2_script_set_t, cardano_plutus_v2_script_set_unref>(cbor, cardano_plutus_v2_script_set_from_cbor));
}

PlutusV2ScriptSet
PlutusV2ScriptSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV2ScriptSet::size() const
{
  return cardano_plutus_v2_script_set_get_length(checked());
}

void
PlutusV2ScriptSet::add(const PlutusV2Script& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index).hash() == value.hash())
    {
      return;
    }
  }

  const PlutusV2Script copy(value);
  checkCardanoError(cardano_plutus_v2_script_set_add(checked(), copy.checked()), "PlutusV2ScriptSet add");
}

PlutusV2Script
PlutusV2ScriptSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV2Script index is out of range");
  }

  cardano_plutus_v2_script_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_plutus_v2_script_set_get(checked(), index, &value);
  CardanoObject<cardano_plutus_v2_script_t, cardano_plutus_v2_script_unref> owner(value);

  checkCardanoError(status, "PlutusV2ScriptSet at");
  const PlutusV2Script copied(*owner.release());
  return PlutusV2Script(copied);
}

bool
PlutusV2ScriptSet::useTag() const
{
  return cardano_plutus_v2_script_set_get_use_tag(checked());
}

void
PlutusV2ScriptSet::setUseTag(bool value)
{
  checkCardanoError(cardano_plutus_v2_script_set_set_use_tag(checked(), value), "PlutusV2ScriptSet setUseTag");
}

std::vector<std::uint8_t>
PlutusV2ScriptSet::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v2_script_set_to_cbor);
}

std::string
PlutusV2ScriptSet::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v2_script_set_to_cip116_json);
}

void
PlutusV2ScriptSet::swap(PlutusV2ScriptSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV2ScriptSet&
PlutusV2ScriptSet::operator=(const PlutusV2ScriptSet& other)
{
  if (this != &other)
  {
    PlutusV2ScriptSet copy(other);
    swap(copy);
  }
  return *this;
}

PlutusV2ScriptSet&
PlutusV2ScriptSet::operator=(PlutusV2ScriptSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v2_script_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
PlutusV2ScriptSet::operator==(const PlutusV2ScriptSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
PlutusV2ScriptSet::operator!=(const PlutusV2ScriptSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV2ScriptSet::PlutusV2ScriptSet(cardano_plutus_v2_script_set_t* value) noexcept
: m_value(value)
{
}

cardano_plutus_v2_script_set_t*
PlutusV2ScriptSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV2ScriptSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV2ScriptSet& lhs, PlutusV2ScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
