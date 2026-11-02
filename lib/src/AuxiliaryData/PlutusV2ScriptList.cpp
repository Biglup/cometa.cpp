/**
 * @file PlutusV2ScriptList.cpp
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

#include <Cometa/AuxiliaryData/PlutusV2ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV2Script.h>

#include <cardano/auxiliary_data/plutus_v2_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v2_script.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV2ScriptList::PlutusV2ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v2_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v2_script_list_new(&value);

  CardanoObject<cardano_plutus_v2_script_list_t, cardano_plutus_v2_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV2ScriptList");

  m_value = owner.release();
}

PlutusV2ScriptList::PlutusV2ScriptList(std::initializer_list<PlutusV2Script> scripts)
: PlutusV2ScriptList()
{
  for (const PlutusV2Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV2ScriptList::PlutusV2ScriptList(const PlutusV2ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v2_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v2_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v2_script_list_t, cardano_plutus_v2_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV2ScriptList");

  m_value = owner.release();
}

PlutusV2ScriptList::PlutusV2ScriptList(PlutusV2ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV2ScriptList::~PlutusV2ScriptList()
{
  cardano_plutus_v2_script_list_unref(&m_value);
}

PlutusV2ScriptList
PlutusV2ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV2ScriptList value(*decodeCbor<cardano_plutus_v2_script_list_t, cardano_plutus_v2_script_list_unref>(cbor, cardano_plutus_v2_script_list_from_cbor));

  return value;
}

PlutusV2ScriptList
PlutusV2ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV2ScriptList::size() const
{
  return cardano_plutus_v2_script_list_get_length(checked());
}

bool
PlutusV2ScriptList::empty() const
{
  return size() == 0U;
}

PlutusV2Script
PlutusV2ScriptList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV2ScriptList index");
  }

  cardano_plutus_v2_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v2_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v2_script_t, cardano_plutus_v2_script_unref> owner(value);

  checkCardanoError(result, "PlutusV2ScriptList at");

  const PlutusV2Script script(*owner.release());

  return PlutusV2Script(script);
}

void
PlutusV2ScriptList::pushBack(const PlutusV2Script& script)
{
  PlutusV2Script copy(script);

  checkCardanoError(cardano_plutus_v2_script_list_add(checked(), copy.checked()), "PlutusV2ScriptList pushBack");
}

std::vector<PlutusV2Script>
PlutusV2ScriptList::values() const
{
  std::vector<PlutusV2Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV2ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v2_script_list_to_cbor);
}

std::string
PlutusV2ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v2_script_list_to_cip116_json);
}

void
PlutusV2ScriptList::swap(PlutusV2ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV2ScriptList&
PlutusV2ScriptList::operator=(const PlutusV2ScriptList& other)
{
  if (this != &other)
  {
    PlutusV2ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV2ScriptList&
PlutusV2ScriptList::operator=(PlutusV2ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v2_script_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusV2ScriptList::operator==(const PlutusV2ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV2ScriptList::operator!=(const PlutusV2ScriptList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV2ScriptList::PlutusV2ScriptList(cardano_plutus_v2_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v2_script_list_t*
PlutusV2ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV2ScriptList");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV2ScriptList& lhs, PlutusV2ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
