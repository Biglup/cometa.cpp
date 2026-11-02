/**
 * @file PlutusV1ScriptList.cpp
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

#include <Cometa/AuxiliaryData/PlutusV1ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV1Script.h>

#include <cardano/auxiliary_data/plutus_v1_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v1_script.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV1ScriptList::PlutusV1ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v1_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v1_script_list_new(&value);

  CardanoObject<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV1ScriptList");

  m_value = owner.release();
}

PlutusV1ScriptList::PlutusV1ScriptList(std::initializer_list<PlutusV1Script> scripts)
: PlutusV1ScriptList()
{
  for (const PlutusV1Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV1ScriptList::PlutusV1ScriptList(const PlutusV1ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v1_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v1_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV1ScriptList");

  m_value = owner.release();
}

PlutusV1ScriptList::PlutusV1ScriptList(PlutusV1ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV1ScriptList::~PlutusV1ScriptList()
{
  cardano_plutus_v1_script_list_unref(&m_value);
}

PlutusV1ScriptList
PlutusV1ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV1ScriptList value(*decodeCbor<cardano_plutus_v1_script_list_t, cardano_plutus_v1_script_list_unref>(cbor, cardano_plutus_v1_script_list_from_cbor));

  return value;
}

PlutusV1ScriptList
PlutusV1ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV1ScriptList::size() const
{
  return cardano_plutus_v1_script_list_get_length(checked());
}

bool
PlutusV1ScriptList::empty() const
{
  return size() == 0U;
}

PlutusV1Script
PlutusV1ScriptList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV1ScriptList index");
  }

  cardano_plutus_v1_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v1_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v1_script_t, cardano_plutus_v1_script_unref> owner(value);

  checkCardanoError(result, "PlutusV1ScriptList at");

  const PlutusV1Script script(*owner.release());

  return PlutusV1Script(script);
}

void
PlutusV1ScriptList::pushBack(const PlutusV1Script& script)
{
  PlutusV1Script copy(script);

  checkCardanoError(cardano_plutus_v1_script_list_add(checked(), copy.checked()), "PlutusV1ScriptList pushBack");
}

std::vector<PlutusV1Script>
PlutusV1ScriptList::values() const
{
  std::vector<PlutusV1Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV1ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v1_script_list_to_cbor);
}

std::string
PlutusV1ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v1_script_list_to_cip116_json);
}

void
PlutusV1ScriptList::swap(PlutusV1ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV1ScriptList&
PlutusV1ScriptList::operator=(const PlutusV1ScriptList& other)
{
  if (this != &other)
  {
    PlutusV1ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV1ScriptList&
PlutusV1ScriptList::operator=(PlutusV1ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v1_script_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusV1ScriptList::operator==(const PlutusV1ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV1ScriptList::operator!=(const PlutusV1ScriptList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV1ScriptList::PlutusV1ScriptList(cardano_plutus_v1_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v1_script_list_t*
PlutusV1ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV1ScriptList");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV1ScriptList& lhs, PlutusV1ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
