/**
 * @file PlutusV1ScriptSet.cpp
 *
 * @brief Ledger Plutus V1 script collections.
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

#include <Cometa/WitnessSet/PlutusV1ScriptSet.h>

#include "../CardanoInterop.h"

#include <cardano/scripts/plutus_scripts/plutus_v1_script.h>
#include <cardano/witness_set/plutus_v1_script_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV1ScriptSet::PlutusV1ScriptSet()
: m_value(nullptr)
{
  cardano_plutus_v1_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v1_script_set_new(&value);
  CardanoObject<cardano_plutus_v1_script_set_t, cardano_plutus_v1_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV1ScriptSet");
  m_value = owner.release();
}

PlutusV1ScriptSet::PlutusV1ScriptSet(const PlutusV1ScriptSet& other)
: m_value(nullptr)
{
  cardano_plutus_v1_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v1_script_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_plutus_v1_script_set_t, cardano_plutus_v1_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV1ScriptSet copy");
  m_value = owner.release();
}

PlutusV1ScriptSet::PlutusV1ScriptSet(PlutusV1ScriptSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV1ScriptSet::~PlutusV1ScriptSet()
{
  cardano_plutus_v1_script_set_unref(&m_value);
}

PlutusV1ScriptSet
PlutusV1ScriptSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusV1ScriptSet(decodeCbor<cardano_plutus_v1_script_set_t, cardano_plutus_v1_script_set_unref>(cbor, cardano_plutus_v1_script_set_from_cbor));
}

PlutusV1ScriptSet
PlutusV1ScriptSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV1ScriptSet::size() const
{
  return cardano_plutus_v1_script_set_get_length(checked());
}

void
PlutusV1ScriptSet::add(const PlutusV1Script& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index).hash() == value.hash())
    {
      return;
    }
  }

  const PlutusV1Script copy(value);
  checkCardanoError(cardano_plutus_v1_script_set_add(checked(), copy.checked()), "PlutusV1ScriptSet add");
}

PlutusV1Script
PlutusV1ScriptSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV1Script index is out of range");
  }

  cardano_plutus_v1_script_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_plutus_v1_script_set_get(checked(), index, &value);
  CardanoObject<cardano_plutus_v1_script_t, cardano_plutus_v1_script_unref> owner(value);

  checkCardanoError(status, "PlutusV1ScriptSet at");
  const PlutusV1Script copied(*owner.release());
  return PlutusV1Script(copied);
}

bool
PlutusV1ScriptSet::useTag() const
{
  return cardano_plutus_v1_script_set_get_use_tag(checked());
}

void
PlutusV1ScriptSet::setUseTag(bool value)
{
  checkCardanoError(cardano_plutus_v1_script_set_set_use_tag(checked(), value), "PlutusV1ScriptSet setUseTag");
}

std::vector<std::uint8_t>
PlutusV1ScriptSet::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v1_script_set_to_cbor);
}

std::string
PlutusV1ScriptSet::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v1_script_set_to_cip116_json);
}

void
PlutusV1ScriptSet::swap(PlutusV1ScriptSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV1ScriptSet&
PlutusV1ScriptSet::operator=(const PlutusV1ScriptSet& other)
{
  if (this != &other)
  {
    PlutusV1ScriptSet copy(other);
    swap(copy);
  }
  return *this;
}

PlutusV1ScriptSet&
PlutusV1ScriptSet::operator=(PlutusV1ScriptSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v1_script_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
PlutusV1ScriptSet::operator==(const PlutusV1ScriptSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
PlutusV1ScriptSet::operator!=(const PlutusV1ScriptSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV1ScriptSet::PlutusV1ScriptSet(cardano_plutus_v1_script_set_t* value) noexcept
: m_value(value)
{
}

cardano_plutus_v1_script_set_t*
PlutusV1ScriptSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV1ScriptSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV1ScriptSet& lhs, PlutusV1ScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
