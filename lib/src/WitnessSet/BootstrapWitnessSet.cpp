/**
 * @file BootstrapWitnessSet.cpp
 *
 * @brief Ledger bootstrap witness collections.
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

#include <Cometa/WitnessSet/BootstrapWitnessSet.h>

#include "../CardanoInterop.h"

#include <cardano/witness_set/bootstrap_witness.h>
#include <cardano/witness_set/bootstrap_witness_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
BootstrapWitnessSet::BootstrapWitnessSet()
: m_value(nullptr)
{
  cardano_bootstrap_witness_set_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_bootstrap_witness_set_new(&value);
  CardanoObject<cardano_bootstrap_witness_set_t, cardano_bootstrap_witness_set_unref> owner(value);

  checkCardanoError(status, "BootstrapWitnessSet");
  m_value = owner.release();
}

BootstrapWitnessSet::BootstrapWitnessSet(const BootstrapWitnessSet& other)
: m_value(nullptr)
{
  cardano_bootstrap_witness_set_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_bootstrap_witness_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_bootstrap_witness_set_t, cardano_bootstrap_witness_set_unref> owner(value);

  checkCardanoError(status, "BootstrapWitnessSet copy");
  m_value = owner.release();
}

BootstrapWitnessSet::BootstrapWitnessSet(BootstrapWitnessSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

BootstrapWitnessSet::~BootstrapWitnessSet()
{
  cardano_bootstrap_witness_set_unref(&m_value);
}

BootstrapWitnessSet
BootstrapWitnessSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return BootstrapWitnessSet(decodeCbor<cardano_bootstrap_witness_set_t, cardano_bootstrap_witness_set_unref>(cbor, cardano_bootstrap_witness_set_from_cbor));
}

BootstrapWitnessSet
BootstrapWitnessSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
BootstrapWitnessSet::size() const
{
  return cardano_bootstrap_witness_set_get_length(checked());
}

void
BootstrapWitnessSet::add(const BootstrapWitness& value)
{
  const BootstrapWitness copy(value);
  checkCardanoError(cardano_bootstrap_witness_set_add(checked(), copy.checked()), "BootstrapWitnessSet add");
  invalidateElements();
}

BootstrapWitness
BootstrapWitnessSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("BootstrapWitness index is out of range");
  }

  cardano_bootstrap_witness_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_bootstrap_witness_set_get(checked(), index, &value);
  CardanoObject<cardano_bootstrap_witness_t, cardano_bootstrap_witness_unref> owner(value);

  checkCardanoError(status, "BootstrapWitnessSet at");
  const BootstrapWitness copied(owner.release());
  return BootstrapWitness(copied);
}

bool
BootstrapWitnessSet::useTag() const
{
  return cardano_bootstrap_witness_set_get_use_tag(checked());
}

void
BootstrapWitnessSet::setUseTag(bool value)
{
  checkCardanoError(cardano_bootstrap_witness_set_set_use_tag(checked(), value), "BootstrapWitnessSet setUseTag");
}

std::vector<std::uint8_t>
BootstrapWitnessSet::toCbor() const
{
  return encodeCbor(checked(), cardano_bootstrap_witness_set_to_cbor);
}

std::string
BootstrapWitnessSet::toJson() const
{
  return encodeJson(checked(), cardano_bootstrap_witness_set_to_cip116_json);
}

void
BootstrapWitnessSet::swap(BootstrapWitnessSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

BootstrapWitnessSet&
BootstrapWitnessSet::operator=(const BootstrapWitnessSet& other)
{
  if (this != &other)
  {
    BootstrapWitnessSet copy(other);
    swap(copy);
  }
  return *this;
}

BootstrapWitnessSet&
BootstrapWitnessSet::operator=(BootstrapWitnessSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_bootstrap_witness_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }
  return *this;
}

bool
BootstrapWitnessSet::operator==(const BootstrapWitnessSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
BootstrapWitnessSet::operator!=(const BootstrapWitnessSet& other) const
{
  return !(*this == other);
}

BootstrapWitnessSet::const_iterator
BootstrapWitnessSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

BootstrapWitnessSet::const_iterator
BootstrapWitnessSet::end() const
{
  materialize();

  return m_elements.cend();
}

BootstrapWitnessSet::const_iterator
BootstrapWitnessSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

BootstrapWitnessSet::const_iterator
BootstrapWitnessSet::cend() const
{
  materialize();

  return m_elements.cend();
}

BootstrapWitnessSet::const_reverse_iterator
BootstrapWitnessSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

BootstrapWitnessSet::const_reverse_iterator
BootstrapWitnessSet::rend() const
{
  materialize();

  return m_elements.crend();
}

BootstrapWitnessSet::const_reverse_iterator
BootstrapWitnessSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

BootstrapWitnessSet::const_reverse_iterator
BootstrapWitnessSet::crend() const
{
  materialize();

  return m_elements.crend();
}

BootstrapWitnessSet::iterator
BootstrapWitnessSet::find(const value_type& value) const
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

BootstrapWitnessSet::size_type
BootstrapWitnessSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
BootstrapWitnessSet::empty() const
{
  return size() == 0;
}

bool
BootstrapWitnessSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<BootstrapWitnessSet::iterator, bool>
BootstrapWitnessSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  BootstrapWitnessSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

BootstrapWitnessSet::iterator
BootstrapWitnessSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
BootstrapWitnessSet::clear()
{
  BootstrapWitnessSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

BootstrapWitnessSet::iterator
BootstrapWitnessSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

BootstrapWitnessSet::iterator
BootstrapWitnessSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  BootstrapWitnessSet replacement;
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

BootstrapWitnessSet::size_type
BootstrapWitnessSet::erase(const value_type& value)
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

BootstrapWitnessSet::BootstrapWitnessSet(cardano_bootstrap_witness_set_t* value) noexcept
: m_value(value)
{
}

cardano_bootstrap_witness_set_t*
BootstrapWitnessSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from BootstrapWitnessSet");
  }
  return m_value;
}

void
BootstrapWitnessSet::materialize() const
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
BootstrapWitnessSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
BootstrapWitnessSet::sameKey(const value_type& left, const value_type& right)
{
  return left.publicKey() == right.publicKey() && left.chainCode() == right.chainCode() && left.attributes() == right.attributes();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(BootstrapWitnessSet& lhs, BootstrapWitnessSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
