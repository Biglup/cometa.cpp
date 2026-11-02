/**
 * @file NativeScriptSet.cpp
 *
 * @brief Ledger native script collections.
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

#include <Cometa/WitnessSet/NativeScriptSet.h>

#include "../CardanoInterop.h"

#include <cardano/scripts/native_scripts/native_script.h>
#include <cardano/witness_set/native_script_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
NativeScriptSet::NativeScriptSet()
: m_value(nullptr)
{
  cardano_native_script_set_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_native_script_set_new(&value);
  CardanoObject<cardano_native_script_set_t, cardano_native_script_set_unref> owner(value);

  checkCardanoError(status, "NativeScriptSet");
  m_value = owner.release();
}

NativeScriptSet::NativeScriptSet(const NativeScriptSet& other)
: m_value(nullptr)
{
  cardano_native_script_set_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_native_script_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_native_script_set_t, cardano_native_script_set_unref> owner(value);

  checkCardanoError(status, "NativeScriptSet copy");
  m_value = owner.release();
}

NativeScriptSet::NativeScriptSet(NativeScriptSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

NativeScriptSet::~NativeScriptSet()
{
  cardano_native_script_set_unref(&m_value);
}

NativeScriptSet
NativeScriptSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return NativeScriptSet(decodeCbor<cardano_native_script_set_t, cardano_native_script_set_unref>(cbor, cardano_native_script_set_from_cbor));
}

NativeScriptSet
NativeScriptSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
NativeScriptSet::size() const
{
  return cardano_native_script_set_get_length(checked());
}

void
NativeScriptSet::add(const NativeScript& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index).hash() == value.hash())
    {
      return;
    }
  }

  const NativeScript copy(value);
  checkCardanoError(cardano_native_script_set_add(checked(), copy.checked()), "NativeScriptSet add");
}

NativeScript
NativeScriptSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("NativeScript index is out of range");
  }

  cardano_native_script_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_native_script_set_get(checked(), index, &value);
  CardanoObject<cardano_native_script_t, cardano_native_script_unref> owner(value);

  checkCardanoError(status, "NativeScriptSet at");
  const NativeScript copied(*owner.release());
  return NativeScript(copied);
}

bool
NativeScriptSet::useTag() const
{
  return cardano_native_script_set_get_use_tag(checked());
}

void
NativeScriptSet::setUseTag(bool value)
{
  checkCardanoError(cardano_native_script_set_set_use_tag(checked(), value), "NativeScriptSet setUseTag");
}

std::vector<std::uint8_t>
NativeScriptSet::toCbor() const
{
  return encodeCbor(checked(), cardano_native_script_set_to_cbor);
}

std::string
NativeScriptSet::toJson() const
{
  return encodeJson(checked(), cardano_native_script_set_to_cip116_json);
}

void
NativeScriptSet::swap(NativeScriptSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

NativeScriptSet&
NativeScriptSet::operator=(const NativeScriptSet& other)
{
  if (this != &other)
  {
    NativeScriptSet copy(other);
    swap(copy);
  }
  return *this;
}

NativeScriptSet&
NativeScriptSet::operator=(NativeScriptSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_native_script_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
NativeScriptSet::operator==(const NativeScriptSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
NativeScriptSet::operator!=(const NativeScriptSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

NativeScriptSet::NativeScriptSet(cardano_native_script_set_t* value) noexcept
: m_value(value)
{
}

cardano_native_script_set_t*
NativeScriptSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from NativeScriptSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NativeScriptSet& lhs, NativeScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
