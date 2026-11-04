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

#include <utility>

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
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
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
  checkCardanoError(cardano_plutus_v1_script_set_add(checked(), checkedNativeHandle(copy)), "PlutusV1ScriptSet add");
  invalidateElements();
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
  const PlutusV1Script copied = PlutusV1Script::adopt(owner.release());
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
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
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
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
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

PlutusV1ScriptSet::const_iterator
PlutusV1ScriptSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV1ScriptSet::const_iterator
PlutusV1ScriptSet::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusV1ScriptSet::const_iterator
PlutusV1ScriptSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusV1ScriptSet::const_iterator
PlutusV1ScriptSet::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusV1ScriptSet::const_reverse_iterator
PlutusV1ScriptSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV1ScriptSet::const_reverse_iterator
PlutusV1ScriptSet::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV1ScriptSet::const_reverse_iterator
PlutusV1ScriptSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusV1ScriptSet::const_reverse_iterator
PlutusV1ScriptSet::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusV1ScriptSet::iterator
PlutusV1ScriptSet::find(const value_type& value) const
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

PlutusV1ScriptSet::size_type
PlutusV1ScriptSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
PlutusV1ScriptSet::empty() const
{
  return size() == 0;
}

bool
PlutusV1ScriptSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<PlutusV1ScriptSet::iterator, bool>
PlutusV1ScriptSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  PlutusV1ScriptSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

PlutusV1ScriptSet::iterator
PlutusV1ScriptSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
PlutusV1ScriptSet::clear()
{
  PlutusV1ScriptSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

PlutusV1ScriptSet::iterator
PlutusV1ScriptSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

PlutusV1ScriptSet::iterator
PlutusV1ScriptSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  PlutusV1ScriptSet replacement;
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

PlutusV1ScriptSet::size_type
PlutusV1ScriptSet::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
}

PlutusV1ScriptSet
PlutusV1ScriptSet::adopt(cardano_plutus_v1_script_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PlutusV1ScriptSet");
  }

  return PlutusV1ScriptSet(value);
}

cardano_plutus_v1_script_set_t*
PlutusV1ScriptSet::nativeHandle() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return m_value;
}

const cardano_plutus_v1_script_set_t*
PlutusV1ScriptSet::nativeHandle() const noexcept
{
  return m_value;
}

cardano_plutus_v1_script_set_t*
PlutusV1ScriptSet::release() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return std::exchange(m_value, nullptr);
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

void
PlutusV1ScriptSet::materialize() const
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
PlutusV1ScriptSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
PlutusV1ScriptSet::sameKey(const value_type& left, const value_type& right)
{
  return left.hash() == right.hash();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusV1ScriptSet& lhs, PlutusV1ScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
