/**
 * @file GuardSet.cpp
 *
 * @brief Transaction guards in ledger order.
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

#include <Cometa/TransactionBody/GuardSet.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/common/guard_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
GuardSet::GuardSet()
: m_value(nullptr)
{
  cardano_guard_set_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_guard_set_new(&value);
  CardanoObject<cardano_guard_set_t, cardano_guard_set_unref> owner(value);

  checkCardanoError(status, "GuardSet");
  m_value = owner.release();
}

GuardSet::GuardSet(const GuardSet& other)
: m_value(nullptr)
{
  cardano_guard_set_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_guard_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_guard_set_t, cardano_guard_set_unref> owner(value);

  checkCardanoError(status, "GuardSet copy");
  m_value = owner.release();
}

GuardSet::GuardSet(GuardSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

GuardSet::~GuardSet()
{
  cardano_guard_set_unref(&m_value);
}

GuardSet
GuardSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return GuardSet(decodeCbor<cardano_guard_set_t, cardano_guard_set_unref>(cbor, cardano_guard_set_from_cbor));
}

GuardSet
GuardSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
GuardSet::size() const
{
  return cardano_guard_set_get_length(checked());
}

void
GuardSet::add(const Credential& value)
{
  const Credential copy(value);
  checkCardanoError(cardano_guard_set_add(checked(), copy.checked()), "GuardSet add");
  invalidateElements();
}

Credential
GuardSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Credential index is out of range");
  }

  cardano_credential_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_guard_set_get(checked(), index, &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(value);

  checkCardanoError(status, "GuardSet at");
  const Credential copied(owner.release());
  return Credential(copied);
}

bool
GuardSet::useTag() const
{
  return cardano_guard_set_is_tagged(checked());
}

void
GuardSet::setUseTag(bool value)
{
  checkCardanoError(cardano_guard_set_set_use_tag(checked(), value), "GuardSet setUseTag");
}

std::vector<std::uint8_t>
GuardSet::toCbor() const
{
  return encodeCbor(checked(), cardano_guard_set_to_cbor);
}

std::string
GuardSet::toJson() const
{
  return encodeJson(checked(), cardano_guard_set_to_cip116_json);
}

void
GuardSet::swap(GuardSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

GuardSet&
GuardSet::operator=(const GuardSet& other)
{
  if (this != &other)
  {
    GuardSet copy(other);
    swap(copy);
  }
  return *this;
}

GuardSet&
GuardSet::operator=(GuardSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_guard_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }
  return *this;
}

bool
GuardSet::operator==(const GuardSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
GuardSet::operator!=(const GuardSet& other) const
{
  return !(*this == other);
}

GuardSet::const_iterator
GuardSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

GuardSet::const_iterator
GuardSet::end() const
{
  materialize();

  return m_elements.cend();
}

GuardSet::const_iterator
GuardSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

GuardSet::const_iterator
GuardSet::cend() const
{
  materialize();

  return m_elements.cend();
}

GuardSet::const_reverse_iterator
GuardSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

GuardSet::const_reverse_iterator
GuardSet::rend() const
{
  materialize();

  return m_elements.crend();
}

GuardSet::const_reverse_iterator
GuardSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

GuardSet::const_reverse_iterator
GuardSet::crend() const
{
  materialize();

  return m_elements.crend();
}

GuardSet::iterator
GuardSet::find(const value_type& value) const
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

GuardSet::size_type
GuardSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
GuardSet::empty() const
{
  return size() == 0;
}

bool
GuardSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<GuardSet::iterator, bool>
GuardSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  GuardSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

GuardSet::iterator
GuardSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
GuardSet::clear()
{
  GuardSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

GuardSet::iterator
GuardSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

GuardSet::iterator
GuardSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  GuardSet replacement;
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

GuardSet::size_type
GuardSet::erase(const value_type& value)
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

GuardSet::GuardSet(cardano_guard_set_t* value) noexcept
: m_value(value)
{
}

cardano_guard_set_t*
GuardSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from GuardSet");
  }
  return m_value;
}

void
GuardSet::materialize() const
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
GuardSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
GuardSet::sameKey(const value_type& left, const value_type& right)
{
  return left == right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(GuardSet& lhs, GuardSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
