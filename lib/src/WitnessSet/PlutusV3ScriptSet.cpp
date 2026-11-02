/**
 * @file PlutusV3ScriptSet.cpp
 *
 * @brief Ledger Plutus V3 script collections.
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

#include <Cometa/WitnessSet/PlutusV3ScriptSet.h>

#include "../CardanoInterop.h"

#include <cardano/scripts/plutus_scripts/plutus_v3_script.h>
#include <cardano/witness_set/plutus_v3_script_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV3ScriptSet::PlutusV3ScriptSet()
: m_value(nullptr)
{
  cardano_plutus_v3_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v3_script_set_new(&value);
  CardanoObject<cardano_plutus_v3_script_set_t, cardano_plutus_v3_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV3ScriptSet");
  m_value = owner.release();
}

PlutusV3ScriptSet::PlutusV3ScriptSet(const PlutusV3ScriptSet& other)
: m_value(nullptr)
{
  cardano_plutus_v3_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v3_script_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_plutus_v3_script_set_t, cardano_plutus_v3_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV3ScriptSet copy");
  m_value = owner.release();
}

PlutusV3ScriptSet::PlutusV3ScriptSet(PlutusV3ScriptSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV3ScriptSet::~PlutusV3ScriptSet()
{
  cardano_plutus_v3_script_set_unref(&m_value);
}

PlutusV3ScriptSet
PlutusV3ScriptSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusV3ScriptSet(decodeCbor<cardano_plutus_v3_script_set_t, cardano_plutus_v3_script_set_unref>(cbor, cardano_plutus_v3_script_set_from_cbor));
}

PlutusV3ScriptSet
PlutusV3ScriptSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV3ScriptSet::size() const
{
  return cardano_plutus_v3_script_set_get_length(checked());
}

void
PlutusV3ScriptSet::add(const PlutusV3Script& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index).hash() == value.hash())
    {
      return;
    }
  }

  const PlutusV3Script copy(value);
  checkCardanoError(cardano_plutus_v3_script_set_add(checked(), copy.checked()), "PlutusV3ScriptSet add");
}

PlutusV3Script
PlutusV3ScriptSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV3Script index is out of range");
  }

  cardano_plutus_v3_script_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_plutus_v3_script_set_get(checked(), index, &value);
  CardanoObject<cardano_plutus_v3_script_t, cardano_plutus_v3_script_unref> owner(value);

  checkCardanoError(status, "PlutusV3ScriptSet at");
  const PlutusV3Script copied(*owner.release());
  return PlutusV3Script(copied);
}

bool
PlutusV3ScriptSet::useTag() const
{
  return cardano_plutus_v3_script_set_get_use_tag(checked());
}

void
PlutusV3ScriptSet::setUseTag(bool value)
{
  checkCardanoError(cardano_plutus_v3_script_set_set_use_tag(checked(), value), "PlutusV3ScriptSet setUseTag");
}

std::vector<std::uint8_t>
PlutusV3ScriptSet::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v3_script_set_to_cbor);
}

std::string
PlutusV3ScriptSet::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v3_script_set_to_cip116_json);
}

void
PlutusV3ScriptSet::swap(PlutusV3ScriptSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV3ScriptSet&
PlutusV3ScriptSet::operator=(const PlutusV3ScriptSet& other)
{
  if (this != &other)
  {
    PlutusV3ScriptSet copy(other);
    swap(copy);
  }
  return *this;
}

PlutusV3ScriptSet&
PlutusV3ScriptSet::operator=(PlutusV3ScriptSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v3_script_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
PlutusV3ScriptSet::operator==(const PlutusV3ScriptSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
PlutusV3ScriptSet::operator!=(const PlutusV3ScriptSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV3ScriptSet::PlutusV3ScriptSet(cardano_plutus_v3_script_set_t* value) noexcept
: m_value(value)
{
}

cardano_plutus_v3_script_set_t*
PlutusV3ScriptSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV3ScriptSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV3ScriptSet& lhs, PlutusV3ScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
