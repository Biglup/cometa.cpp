/**
 * @file PlutusV4ScriptList.cpp
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

#include <Cometa/AuxiliaryData/PlutusV4ScriptList.h>

#include "../CardanoInterop.h"
#include <Cometa/Scripts/PlutusScripts/PlutusV4Script.h>

#include <cardano/auxiliary_data/plutus_v4_script_list.h>
#include <cardano/scripts/plutus_scripts/plutus_v4_script.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV4ScriptList::PlutusV4ScriptList()
: m_value(nullptr)
{
  cardano_plutus_v4_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v4_script_list_new(&value);

  CardanoObject<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV4ScriptList");

  m_value = owner.release();
}

PlutusV4ScriptList::PlutusV4ScriptList(std::initializer_list<PlutusV4Script> scripts)
: PlutusV4ScriptList()
{
  for (const PlutusV4Script& script: scripts)
  {
    pushBack(script);
  }
}

PlutusV4ScriptList::PlutusV4ScriptList(const PlutusV4ScriptList& other)
: m_value(nullptr)
{
  cardano_plutus_v4_script_list_t* value  = nullptr;
  const cardano_error_t            result = cardano_plutus_v4_script_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref> owner(value);

  checkCardanoError(result, "PlutusV4ScriptList");

  m_value = owner.release();
}

PlutusV4ScriptList::PlutusV4ScriptList(PlutusV4ScriptList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PlutusV4ScriptList::~PlutusV4ScriptList()
{
  cardano_plutus_v4_script_list_unref(&m_value);
}

PlutusV4ScriptList
PlutusV4ScriptList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  PlutusV4ScriptList value(*decodeCbor<cardano_plutus_v4_script_list_t, cardano_plutus_v4_script_list_unref>(cbor, cardano_plutus_v4_script_list_from_cbor));

  return value;
}

PlutusV4ScriptList
PlutusV4ScriptList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV4ScriptList::size() const
{
  return cardano_plutus_v4_script_list_get_length(checked());
}

bool
PlutusV4ScriptList::empty() const
{
  return size() == 0U;
}

PlutusV4Script
PlutusV4ScriptList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV4ScriptList index");
  }

  cardano_plutus_v4_script_t* value  = nullptr;
  const cardano_error_t       result = cardano_plutus_v4_script_list_get(checked(), index, &value);

  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(result, "PlutusV4ScriptList at");

  const PlutusV4Script script(*owner.release());

  return PlutusV4Script(script);
}

void
PlutusV4ScriptList::pushBack(const PlutusV4Script& script)
{
  PlutusV4Script copy(script);

  checkCardanoError(cardano_plutus_v4_script_list_add(checked(), copy.checked()), "PlutusV4ScriptList pushBack");
}

std::vector<PlutusV4Script>
PlutusV4ScriptList::values() const
{
  std::vector<PlutusV4Script> result;
  const std::size_t           count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
PlutusV4ScriptList::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v4_script_list_to_cbor);
}

std::string
PlutusV4ScriptList::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v4_script_list_to_cip116_json);
}

void
PlutusV4ScriptList::swap(PlutusV4ScriptList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PlutusV4ScriptList&
PlutusV4ScriptList::operator=(const PlutusV4ScriptList& other)
{
  if (this != &other)
  {
    PlutusV4ScriptList copy(other);

    swap(copy);
  }

  return *this;
}

PlutusV4ScriptList&
PlutusV4ScriptList::operator=(PlutusV4ScriptList&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v4_script_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
PlutusV4ScriptList::operator==(const PlutusV4ScriptList& other) const
{
  return values() == other.values();
}

bool
PlutusV4ScriptList::operator!=(const PlutusV4ScriptList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV4ScriptList::PlutusV4ScriptList(cardano_plutus_v4_script_list_t& value) noexcept
: m_value(&value)
{
}

cardano_plutus_v4_script_list_t*
PlutusV4ScriptList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV4ScriptList");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV4ScriptList& lhs, PlutusV4ScriptList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
