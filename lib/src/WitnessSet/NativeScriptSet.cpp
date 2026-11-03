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
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
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
  invalidateElements();
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
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
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
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
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

NativeScriptSet::const_iterator
NativeScriptSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

NativeScriptSet::const_iterator
NativeScriptSet::end() const
{
  materialize();

  return m_elements.cend();
}

NativeScriptSet::const_iterator
NativeScriptSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

NativeScriptSet::const_iterator
NativeScriptSet::cend() const
{
  materialize();

  return m_elements.cend();
}

NativeScriptSet::const_reverse_iterator
NativeScriptSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

NativeScriptSet::const_reverse_iterator
NativeScriptSet::rend() const
{
  materialize();

  return m_elements.crend();
}

NativeScriptSet::const_reverse_iterator
NativeScriptSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

NativeScriptSet::const_reverse_iterator
NativeScriptSet::crend() const
{
  materialize();

  return m_elements.crend();
}

NativeScriptSet::iterator
NativeScriptSet::find(const value_type& value) const
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

NativeScriptSet::size_type
NativeScriptSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
NativeScriptSet::empty() const
{
  return size() == 0;
}

bool
NativeScriptSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<NativeScriptSet::iterator, bool>
NativeScriptSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  NativeScriptSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

NativeScriptSet::iterator
NativeScriptSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
NativeScriptSet::clear()
{
  NativeScriptSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

NativeScriptSet::iterator
NativeScriptSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

NativeScriptSet::iterator
NativeScriptSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  NativeScriptSet replacement;
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

NativeScriptSet::size_type
NativeScriptSet::erase(const value_type& value)
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

void
NativeScriptSet::materialize() const
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
NativeScriptSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
NativeScriptSet::sameKey(const value_type& left, const value_type& right)
{
  return left.hash() == right.hash();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NativeScriptSet& lhs, NativeScriptSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
