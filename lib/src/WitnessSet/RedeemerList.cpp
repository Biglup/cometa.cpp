/**
 * @file RedeemerList.cpp
 *
 * @brief Ledger redeemer collections.
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

#include <Cometa/WitnessSet/RedeemerList.h>

#include "../CardanoInterop.h"

#include <cardano/witness_set/redeemer.h>
#include <cardano/witness_set/redeemer_list.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
RedeemerList::RedeemerList()
: m_value(nullptr)
{
  cardano_redeemer_list_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_redeemer_list_new(&value);
  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> owner(value);

  checkCardanoError(status, "RedeemerList");
  m_value = owner.release();
}

RedeemerList::RedeemerList(const RedeemerList& other)
: m_value(nullptr)
{
  cardano_redeemer_list_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_redeemer_list_deep_clone(other.checked(), &value);
  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> owner(value);

  checkCardanoError(status, "RedeemerList copy");
  m_value = owner.release();
}

RedeemerList::RedeemerList(RedeemerList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

RedeemerList::~RedeemerList()
{
  cardano_redeemer_list_unref(&m_value);
}

RedeemerList
RedeemerList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return RedeemerList(decodeCbor<cardano_redeemer_list_t, cardano_redeemer_list_unref>(cbor, cardano_redeemer_list_from_cbor));
}

RedeemerList
RedeemerList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
RedeemerList::size() const
{
  return cardano_redeemer_list_get_length(checked());
}

void
RedeemerList::add(const Redeemer& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    const Redeemer existing = at(index);

    if (existing.tag() == value.tag() && existing.index() == value.index())
    {
      throw std::invalid_argument("RedeemerList already contains this purpose and index");
    }
  }

  const Redeemer copy(value);
  checkCardanoError(cardano_redeemer_list_add(checked(), copy.checked()), "RedeemerList add");
  cardano_redeemer_list_clear_cbor_cache(checked());
}

Redeemer
RedeemerList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Redeemer index is out of range");
  }

  cardano_redeemer_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_redeemer_list_get(checked(), index, &value);
  CardanoObject<cardano_redeemer_t, cardano_redeemer_unref> owner(value);

  checkCardanoError(status, "RedeemerList at");
  const Redeemer copied(owner.release());
  return Redeemer(copied);
}

void
RedeemerList::setExUnits(RedeemerTag tag, std::uint64_t index, const ExUnits& value)
{
  checkCardanoError(cardano_redeemer_list_set_ex_units(checked(), static_cast<cardano_redeemer_tag_t>(tag), index, value.memory(), value.cpuSteps()), "RedeemerList setExUnits");
  cardano_redeemer_list_clear_cbor_cache(checked());
}

std::vector<std::uint8_t>
RedeemerList::toCbor() const
{
  return encodeCbor(checked(), cardano_redeemer_list_to_cbor);
}

std::string
RedeemerList::toJson() const
{
  return encodeJson(checked(), cardano_redeemer_list_to_cip116_json);
}

void
RedeemerList::swap(RedeemerList& other) noexcept
{
  std::swap(m_value, other.m_value);
}

RedeemerList&
RedeemerList::operator=(const RedeemerList& other)
{
  if (this != &other)
  {
    RedeemerList copy(other);
    swap(copy);
  }
  return *this;
}

RedeemerList&
RedeemerList::operator=(RedeemerList&& other) noexcept
{
  if (this != &other)
  {
    cardano_redeemer_list_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
RedeemerList::operator==(const RedeemerList& other) const
{
  return toCbor() == other.toCbor();
}

bool
RedeemerList::operator!=(const RedeemerList& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

RedeemerList::RedeemerList(cardano_redeemer_list_t* value) noexcept
: m_value(value)
{
}

cardano_redeemer_list_t*
RedeemerList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from RedeemerList");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RedeemerList& lhs, RedeemerList& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
