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
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
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
  invalidateElements();
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
  invalidateElements();
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
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
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
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
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

RedeemerList::const_iterator
RedeemerList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

RedeemerList::const_iterator
RedeemerList::end() const
{
  materialize();

  return m_elements.cend();
}

RedeemerList::const_iterator
RedeemerList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

RedeemerList::const_iterator
RedeemerList::cend() const
{
  materialize();

  return m_elements.cend();
}

RedeemerList::const_reverse_iterator
RedeemerList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

RedeemerList::const_reverse_iterator
RedeemerList::rend() const
{
  materialize();

  return m_elements.crend();
}

RedeemerList::const_reverse_iterator
RedeemerList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

RedeemerList::const_reverse_iterator
RedeemerList::crend() const
{
  materialize();

  return m_elements.crend();
}

RedeemerList::iterator
RedeemerList::find(const value_type& value) const
{
  materialize();

  for (iterator position = m_elements.cbegin(); position != m_elements.cend(); ++position)
  {
    if (sameKey(*position, value))
    {
      return position;
    }
  }

  return m_elements.cend();
}

RedeemerList::size_type
RedeemerList::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
RedeemerList::empty() const
{
  return size() == 0;
}

bool
RedeemerList::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<RedeemerList::iterator, bool>
RedeemerList::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  RedeemerList replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

RedeemerList::iterator
RedeemerList::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
RedeemerList::clear()
{
  RedeemerList replacement;

  swap(replacement);
}

RedeemerList::iterator
RedeemerList::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

RedeemerList::iterator
RedeemerList::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  RedeemerList replacement;

  for (size_type index = 0; index < m_elements.size(); ++index)
  {
    if (index < start || index >= finish)
    {
      replacement.add(m_elements[index]);
    }
  }

  const iterator next = std::next(replacement.cbegin(), static_cast<difference_type>(start));
  swap(replacement);

  return next;
}

RedeemerList::size_type
RedeemerList::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
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

void
RedeemerList::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  std::vector<value_type> elements;
  const size_type         elementCount = size();
  elements.reserve(elementCount);

  for (size_type index = 0; index < elementCount; ++index)
  {
    elements.push_back(at(index));
  }

  m_elements.swap(elements);
  m_materialized = true;
}

void
RedeemerList::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
RedeemerList::sameKey(const value_type& left, const value_type& right)
{
  return left.tag() == right.tag() && left.index() == right.index();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RedeemerList& lhs, RedeemerList& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
