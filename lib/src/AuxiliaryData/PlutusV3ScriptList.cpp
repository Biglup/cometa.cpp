/**
 * @file PlutusV3ScriptList.cpp
 *
 * @brief An ordered sequence of auxiliary Plutus scripts.
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

#include <Cometa/AuxiliaryData/PlutusV3ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV3Script.h>

#include <cardano/auxiliary_data/plutus_v3_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v3_script.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV3ScriptList::PlutusV3ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v3_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v3_script_list_new(&value);

  CardanoObject<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV3ScriptList");

  m_value = owner.release();
}

PlutusV3ScriptList::PlutusV3ScriptList(std::initializer_list<PlutusV3Script> scripts)
: PlutusV3ScriptList()
{
  for (const PlutusV3Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV3ScriptList::PlutusV3ScriptList(const PlutusV3ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v3_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v3_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV3ScriptList");

  m_value = owner.release();
}

PlutusV3ScriptList::PlutusV3ScriptList(PlutusV3ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV3ScriptList::~PlutusV3ScriptList()
{
  cardano_plutus_v3_script_list_unref(&m_value);
}

PlutusV3ScriptList
PlutusV3ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV3ScriptList value(*decodeCbor<cardano_plutus_v3_script_list_t, cardano_plutus_v3_script_list_unref>(cbor, cardano_plutus_v3_script_list_from_cbor));

  return value;
}

PlutusV3ScriptList
PlutusV3ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV3ScriptList::size() const
{
  return cardano_plutus_v3_script_list_get_length(checked());
}

bool
PlutusV3ScriptList::empty() const
{
  return size() == 0U;
}

PlutusV3Script
PlutusV3ScriptList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV3ScriptList index");
  }

  cardano_plutus_v3_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v3_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v3_script_t, cardano_plutus_v3_script_unref> owner(value);

  checkCardanoError(result, "PlutusV3ScriptList at");

  const PlutusV3Script script(*owner.release());

  return PlutusV3Script(script);
}

void
PlutusV3ScriptList::pushBack(const PlutusV3Script& script)
{
  PlutusV3Script copy(script);

  checkCardanoError(cardano_plutus_v3_script_list_add(checked(), copy.checked()), "PlutusV3ScriptList pushBack");
}

std::vector<PlutusV3Script>
PlutusV3ScriptList::values() const
{
  std::vector<PlutusV3Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV3ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v3_script_list_to_cbor);
}

std::string
PlutusV3ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v3_script_list_to_cip116_json);
}

void
PlutusV3ScriptList::swap(PlutusV3ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV3ScriptList&
PlutusV3ScriptList::operator=(const PlutusV3ScriptList& other)
{
  if (this != &other)
  {
    PlutusV3ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV3ScriptList&
PlutusV3ScriptList::operator=(PlutusV3ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v3_script_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusV3ScriptList::operator==(const PlutusV3ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV3ScriptList::operator!=(const PlutusV3ScriptList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV3ScriptList::PlutusV3ScriptList(cardano_plutus_v3_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v3_script_list_t*
PlutusV3ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV3ScriptList");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV3ScriptList& lhs, PlutusV3ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
