/**
 * @file PlutusV4ScriptSet.cpp
 *
 * @brief Ledger Plutus V4 script collections.
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

#include <Cometa/WitnessSet/PlutusV4ScriptSet.h>

#include "../CardanoInterop.h"

#include <cardano/scripts/plutus_scripts/plutus_v4_script.h>
#include <cardano/witness_set/plutus_v4_script_set.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusV4ScriptSet::PlutusV4ScriptSet()
: m_value(nullptr)
{
  cardano_plutus_v4_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v4_script_set_new(&value);
  CardanoObject<cardano_plutus_v4_script_set_t, cardano_plutus_v4_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV4ScriptSet");
  m_value = owner.release();
}

PlutusV4ScriptSet::PlutusV4ScriptSet(const PlutusV4ScriptSet& other)
: m_value(nullptr)
{
  cardano_plutus_v4_script_set_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_plutus_v4_script_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_plutus_v4_script_set_t, cardano_plutus_v4_script_set_unref> owner(value);

  checkCardanoError(status, "PlutusV4ScriptSet copy");
  m_value = owner.release();
}

PlutusV4ScriptSet::PlutusV4ScriptSet(PlutusV4ScriptSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

PlutusV4ScriptSet::~PlutusV4ScriptSet()
{
  cardano_plutus_v4_script_set_unref(&m_value);
}

PlutusV4ScriptSet
PlutusV4ScriptSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusV4ScriptSet(decodeCbor<cardano_plutus_v4_script_set_t, cardano_plutus_v4_script_set_unref>(cbor, cardano_plutus_v4_script_set_from_cbor));
}

PlutusV4ScriptSet
PlutusV4ScriptSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusV4ScriptSet::size() const
{
  return cardano_plutus_v4_script_set_get_length(checked());
}

void
PlutusV4ScriptSet::add(const PlutusV4Script& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index).hash() == value.hash())
    {
      return;
    }
  }

  const PlutusV4Script copy(value);
  checkCardanoError(cardano_plutus_v4_script_set_add(checked(), checkedNativeHandle(copy)), "PlutusV4ScriptSet add");
  invalidateElements();
}

PlutusV4Script
PlutusV4ScriptSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusV4Script index is out of range");
  }

  cardano_plutus_v4_script_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_plutus_v4_script_set_get(checked(), index, &value);
  CardanoObject<cardano_plutus_v4_script_t, cardano_plutus_v4_script_unref> owner(value);

  checkCardanoError(status, "PlutusV4ScriptSet at");
  const PlutusV4Script copied = PlutusV4Script::adopt(owner.release());
  return PlutusV4Script(copied);
}

bool
PlutusV4ScriptSet::useTag() const
{
  return cardano_plutus_v4_script_set_get_use_tag(checked());
}

void
PlutusV4ScriptSet::setUseTag(bool value)
{
  checkCardanoError(cardano_plutus_v4_script_set_set_use_tag(checked(), value), "PlutusV4ScriptSet setUseTag");
}

std::vector<std::uint8_t>
PlutusV4ScriptSet::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_v4_script_set_to_cbor);
}

std::string
PlutusV4ScriptSet::toJson() const
{
  return encodeJson(checked(), cardano_plutus_v4_script_set_to_cip116_json);
}

void
PlutusV4ScriptSet::swap(PlutusV4ScriptSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

PlutusV4ScriptSet&
PlutusV4ScriptSet::operator=(const PlutusV4ScriptSet& other)
{
  if (this != &other)
  {
    PlutusV4ScriptSet copy(other);
    swap(copy);
  }
  return *this;
}

PlutusV4ScriptSet&
PlutusV4ScriptSet::operator=(PlutusV4ScriptSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_v4_script_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }
  return *this;
}

bool
PlutusV4ScriptSet::operator==(const PlutusV4ScriptSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
PlutusV4ScriptSet::operator!=(const PlutusV4ScriptSet& other) const
{
  return !(*this == other);
}

PlutusV4ScriptSet::const_iterator
PlutusV4ScriptSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV4ScriptSet::const_iterator
PlutusV4ScriptSet::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusV4ScriptSet::const_iterator
PlutusV4ScriptSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV4ScriptSet::const_iterator
PlutusV4ScriptSet::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusV4ScriptSet::const_reverse_iterator
PlutusV4ScriptSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV4ScriptSet::const_reverse_iterator
PlutusV4ScriptSet::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV4ScriptSet::const_reverse_iterator
PlutusV4ScriptSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV4ScriptSet::const_reverse_iterator
PlutusV4ScriptSet::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV4ScriptSet::iterator
PlutusV4ScriptSet::find(const value_type& value) const
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

PlutusV4ScriptSet::size_type
PlutusV4ScriptSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
PlutusV4ScriptSet::empty() const
{
  return size() == 0;
}

bool
PlutusV4ScriptSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<PlutusV4ScriptSet::iterator, bool>
PlutusV4ScriptSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  PlutusV4ScriptSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

PlutusV4ScriptSet::iterator
PlutusV4ScriptSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
PlutusV4ScriptSet::clear()
{
  PlutusV4ScriptSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

PlutusV4ScriptSet::iterator
PlutusV4ScriptSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

PlutusV4ScriptSet::iterator
PlutusV4ScriptSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  PlutusV4ScriptSet replacement;
  replacement.setUseTag(useTag());

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

PlutusV4ScriptSet::size_type
PlutusV4ScriptSet::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
}

PlutusV4ScriptSet
PlutusV4ScriptSet::adopt(cardano_plutus_v4_script_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PlutusV4ScriptSet");
  }

  return PlutusV4ScriptSet(value);
}

cardano_plutus_v4_script_set_t*
PlutusV4ScriptSet::nativeHandle() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return m_value;
}

const cardano_plutus_v4_script_set_t*
PlutusV4ScriptSet::nativeHandle() const noexcept
{
  return m_value;
}

cardano_plutus_v4_script_set_t*
PlutusV4ScriptSet::release() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PlutusV4ScriptSet::PlutusV4ScriptSet(cardano_plutus_v4_script_set_t* value) noexcept
: m_value(value)
{
}

cardano_plutus_v4_script_set_t*
PlutusV4ScriptSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusV4ScriptSet");
  }
  return m_value;
}

void
PlutusV4ScriptSet::materialize() const
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
PlutusV4ScriptSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
PlutusV4ScriptSet::sameKey(const value_type& left, const value_type& right)
{
  return left.hash() == right.hash();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV4ScriptSet& lhs, PlutusV4ScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
