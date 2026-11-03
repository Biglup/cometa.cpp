/**
 * @file PlutusDataSet.cpp
 *
 * @brief Ledger Plutus data value collections.
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

#include <Cometa/WitnessSet/PlutusDataSet.h>

#include "../CardanoInterop.h"

#include <cardano/plutus_data/plutus_data.h>
#include <cardano/witness_set/plutus_data_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PlutusDataSet::PlutusDataSet()
: m_value(nullptr)
{
  cardano_plutus_data_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_plutus_data_set_new(&value);
  CardanoObject<cardano_plutus_data_set_t, cardano_plutus_data_set_unref> owner(value);

  checkCardanoError(status, "PlutusDataSet");
  m_value = owner.release();
}

PlutusDataSet::PlutusDataSet(const PlutusDataSet& other)
: m_value(nullptr)
{
  cardano_plutus_data_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_plutus_data_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_plutus_data_set_t, cardano_plutus_data_set_unref> owner(value);

  checkCardanoError(status, "PlutusDataSet copy");
  m_value = owner.release();
}

PlutusDataSet::PlutusDataSet(PlutusDataSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

PlutusDataSet::~PlutusDataSet()
{
  cardano_plutus_data_set_unref(&m_value);
}

PlutusDataSet
PlutusDataSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PlutusDataSet(decodeCbor<cardano_plutus_data_set_t, cardano_plutus_data_set_unref>(cbor, cardano_plutus_data_set_from_cbor));
}

PlutusDataSet
PlutusDataSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PlutusDataSet::size() const
{
  return cardano_plutus_data_set_get_length(checked());
}

void
PlutusDataSet::add(const PlutusData& value)
{
  for (std::size_t index = 0; index < size(); ++index)
  {
    if (at(index) == value)
    {
      return;
    }
  }

  const PlutusData copy(value);
  checkCardanoError(cardano_plutus_data_set_add(checked(), copy.checked()), "PlutusDataSet add");
  const bool tagged = useTag();
  cardano_plutus_data_set_clear_cbor_cache(checked());
  setUseTag(tagged);
  invalidateElements();
}

PlutusData
PlutusDataSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("PlutusData index is out of range");
  }

  cardano_plutus_data_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_plutus_data_set_get(checked(), index, &value);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(status, "PlutusDataSet at");
  const PlutusData copied(*owner.release());
  return PlutusData(copied);
}

bool
PlutusDataSet::useTag() const
{
  return cardano_plutus_data_set_get_use_tag(checked());
}

void
PlutusDataSet::setUseTag(bool value)
{
  cardano_plutus_data_set_clear_cbor_cache(checked());
  checkCardanoError(cardano_plutus_data_set_set_use_tag(checked(), value), "PlutusDataSet setUseTag");
}

std::vector<std::uint8_t>
PlutusDataSet::toCbor() const
{
  return encodeCbor(checked(), cardano_plutus_data_set_to_cbor);
}

std::string
PlutusDataSet::toJson() const
{
  return encodeJson(checked(), cardano_plutus_data_set_to_cip116_json);
}

void
PlutusDataSet::swap(PlutusDataSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

PlutusDataSet&
PlutusDataSet::operator=(const PlutusDataSet& other)
{
  if (this != &other)
  {
    PlutusDataSet copy(other);
    swap(copy);
  }
  return *this;
}

PlutusDataSet&
PlutusDataSet::operator=(PlutusDataSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_plutus_data_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }
  return *this;
}

bool
PlutusDataSet::operator==(const PlutusDataSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
PlutusDataSet::operator!=(const PlutusDataSet& other) const
{
  return !(*this == other);
}

PlutusDataSet::const_iterator
PlutusDataSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusDataSet::const_iterator
PlutusDataSet::end() const
{
  materialize();

  return m_elements.cend();
}

PlutusDataSet::const_iterator
PlutusDataSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PlutusDataSet::const_iterator
PlutusDataSet::cend() const
{
  materialize();

  return m_elements.cend();
}

PlutusDataSet::const_reverse_iterator
PlutusDataSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusDataSet::const_reverse_iterator
PlutusDataSet::rend() const
{
  materialize();

  return m_elements.crend();
}

PlutusDataSet::const_reverse_iterator
PlutusDataSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PlutusDataSet::const_reverse_iterator
PlutusDataSet::crend() const
{
  materialize();

  return m_elements.crend();
}

PlutusDataSet::iterator
PlutusDataSet::find(const value_type& value) const
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

PlutusDataSet::size_type
PlutusDataSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
PlutusDataSet::empty() const
{
  return size() == 0;
}

bool
PlutusDataSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<PlutusDataSet::iterator, bool>
PlutusDataSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  PlutusDataSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

PlutusDataSet::iterator
PlutusDataSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
PlutusDataSet::clear()
{
  PlutusDataSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

PlutusDataSet::iterator
PlutusDataSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

PlutusDataSet::iterator
PlutusDataSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  PlutusDataSet replacement;
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

PlutusDataSet::size_type
PlutusDataSet::erase(const value_type& value)
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

PlutusDataSet::PlutusDataSet(cardano_plutus_data_set_t* value) noexcept
: m_value(value)
{
}

cardano_plutus_data_set_t*
PlutusDataSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PlutusDataSet");
  }
  return m_value;
}

void
PlutusDataSet::materialize() const
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
PlutusDataSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
PlutusDataSet::sameKey(const value_type& left, const value_type& right)
{
  return left == right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PlutusDataSet& lhs, PlutusDataSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
