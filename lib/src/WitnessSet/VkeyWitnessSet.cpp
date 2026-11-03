/**
 * @file VkeyWitnessSet.cpp
 *
 * @brief Ledger verification-key witness collections.
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

#include <Cometa/WitnessSet/VkeyWitnessSet.h>

#include "../CardanoInterop.h"

#include <cardano/witness_set/vkey_witness.h>
#include <cardano/witness_set/vkey_witness_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VkeyWitnessSet::VkeyWitnessSet()
: m_value(nullptr)
{
  cardano_vkey_witness_set_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_vkey_witness_set_new(&value);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(value);

  checkCardanoError(status, "VkeyWitnessSet");
  m_value = owner.release();
}

VkeyWitnessSet::VkeyWitnessSet(const VkeyWitnessSet& other)
: m_value(nullptr)
{
  cardano_vkey_witness_set_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_vkey_witness_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(value);

  checkCardanoError(status, "VkeyWitnessSet copy");
  m_value = owner.release();
}

VkeyWitnessSet::VkeyWitnessSet(VkeyWitnessSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

VkeyWitnessSet::~VkeyWitnessSet()
{
  cardano_vkey_witness_set_unref(&m_value);
}

VkeyWitnessSet
VkeyWitnessSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VkeyWitnessSet(decodeCbor<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref>(cbor, cardano_vkey_witness_set_from_cbor));
}

VkeyWitnessSet
VkeyWitnessSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
VkeyWitnessSet::size() const
{
  return cardano_vkey_witness_set_get_length(checked());
}

void
VkeyWitnessSet::add(const VkeyWitness& value)
{
  const VkeyWitness copy(value);
  checkCardanoError(cardano_vkey_witness_set_add(checked(), copy.checked()), "VkeyWitnessSet add");
  invalidateElements();
}

VkeyWitness
VkeyWitnessSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("VkeyWitness index is out of range");
  }

  cardano_vkey_witness_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_vkey_witness_set_get(checked(), index, &value);
  CardanoObject<cardano_vkey_witness_t, cardano_vkey_witness_unref> owner(value);

  checkCardanoError(status, "VkeyWitnessSet at");
  const VkeyWitness copied(owner.release());
  return VkeyWitness(copied);
}

void
VkeyWitnessSet::apply(const VkeyWitnessSet& other)
{
  VkeyWitnessSet       updated(*this);
  const VkeyWitnessSet copy(other);
  checkCardanoError(cardano_vkey_witness_set_apply(updated.checked(), copy.checked()), "VkeyWitnessSet apply");
  swap(updated);
  invalidateElements();
}

bool
VkeyWitnessSet::useTag() const
{
  return cardano_vkey_witness_set_get_use_tag(checked());
}

void
VkeyWitnessSet::setUseTag(bool value)
{
  checkCardanoError(cardano_vkey_witness_set_set_use_tag(checked(), value), "VkeyWitnessSet setUseTag");
}

std::vector<std::uint8_t>
VkeyWitnessSet::toCbor() const
{
  return encodeCbor(checked(), cardano_vkey_witness_set_to_cbor);
}

std::string
VkeyWitnessSet::toJson() const
{
  return encodeJson(checked(), cardano_vkey_witness_set_to_cip116_json);
}

void
VkeyWitnessSet::swap(VkeyWitnessSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

VkeyWitnessSet&
VkeyWitnessSet::operator=(const VkeyWitnessSet& other)
{
  if (this != &other)
  {
    VkeyWitnessSet copy(other);
    swap(copy);
  }
  return *this;
}

VkeyWitnessSet&
VkeyWitnessSet::operator=(VkeyWitnessSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_vkey_witness_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }
  return *this;
}

bool
VkeyWitnessSet::operator==(const VkeyWitnessSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
VkeyWitnessSet::operator!=(const VkeyWitnessSet& other) const
{
  return !(*this == other);
}

VkeyWitnessSet::const_iterator
VkeyWitnessSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

VkeyWitnessSet::const_iterator
VkeyWitnessSet::end() const
{
  materialize();

  return m_elements.cend();
}

VkeyWitnessSet::const_iterator
VkeyWitnessSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

VkeyWitnessSet::const_iterator
VkeyWitnessSet::cend() const
{
  materialize();

  return m_elements.cend();
}

VkeyWitnessSet::const_reverse_iterator
VkeyWitnessSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

VkeyWitnessSet::const_reverse_iterator
VkeyWitnessSet::rend() const
{
  materialize();

  return m_elements.crend();
}

VkeyWitnessSet::const_reverse_iterator
VkeyWitnessSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

VkeyWitnessSet::const_reverse_iterator
VkeyWitnessSet::crend() const
{
  materialize();

  return m_elements.crend();
}

VkeyWitnessSet::iterator
VkeyWitnessSet::find(const value_type& value) const
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

VkeyWitnessSet::size_type
VkeyWitnessSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
VkeyWitnessSet::empty() const
{
  return size() == 0;
}

bool
VkeyWitnessSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<VkeyWitnessSet::iterator, bool>
VkeyWitnessSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  VkeyWitnessSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

VkeyWitnessSet::iterator
VkeyWitnessSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
VkeyWitnessSet::clear()
{
  VkeyWitnessSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

VkeyWitnessSet::iterator
VkeyWitnessSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

VkeyWitnessSet::iterator
VkeyWitnessSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  VkeyWitnessSet replacement;
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

VkeyWitnessSet::size_type
VkeyWitnessSet::erase(const value_type& value)
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

VkeyWitnessSet::VkeyWitnessSet(cardano_vkey_witness_set_t* value) noexcept
: m_value(value)
{
}

cardano_vkey_witness_set_t*
VkeyWitnessSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VkeyWitnessSet");
  }
  return m_value;
}

void
VkeyWitnessSet::materialize() const
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
VkeyWitnessSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
VkeyWitnessSet::sameKey(const value_type& left, const value_type& right)
{
  return left.publicKey() == right.publicKey();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VkeyWitnessSet& lhs, VkeyWitnessSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
