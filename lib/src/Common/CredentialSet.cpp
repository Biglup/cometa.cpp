/**
 * @file CredentialSet.cpp
 *
 * @brief Sorted unique Credential set operations.
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

#include <Cometa/Common/CredentialSet.h>

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/credential_set.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CredentialSet::CredentialSet()
: m_value(nullptr)
{
  cardano_credential_set_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_credential_set_new(&value);
  CardanoObject<cardano_credential_set_t, cardano_credential_set_unref> owner(value);

  checkCardanoError(status, "CredentialSet");
  m_value = owner.release();
}

CredentialSet::CredentialSet(std::initializer_list<Credential> values)
: CredentialSet()
{
  for (const Credential& value: values)
  {
    insert(value);
  }
}

CredentialSet::CredentialSet(const CredentialSet& other)
: m_value(nullptr)
{
  cardano_credential_set_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_credential_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_credential_set_t, cardano_credential_set_unref> owner(value);

  checkCardanoError(status, "CredentialSet copy");
  m_value = owner.release();
}

CredentialSet::CredentialSet(CredentialSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

CredentialSet::~CredentialSet()
{
  cardano_credential_set_unref(&m_value);
}

CredentialSet
CredentialSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  const CborReader reader(cbor);

  if (reader.peekState() == CborReaderState::Tag)
  {
    (void)reader.peekTag();
  }

  CredentialSet                 result(decodeCbor<cardano_credential_set_t, cardano_credential_set_unref>(cbor, cardano_credential_set_from_cbor));
  const std::vector<Credential> elements = result.values();

  for (std::size_t i = 1; i < elements.size(); ++i)
  {
    if (elements[i - 1] == elements[i])
    {
      throw std::invalid_argument("Duplicate element in CBOR set");
    }
  }

  return result;
}

CredentialSet
CredentialSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
CredentialSet::size() const
{
  return cardano_credential_set_get_length(checked());
}

bool
CredentialSet::empty() const
{
  return size() == 0;
}

bool
CredentialSet::contains(const Credential& value) const
{
  const std::size_t elementCount = size();

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_credential_t*                                         element = nullptr;
    const cardano_error_t                                         status  = cardano_credential_set_get(checked(), i, &element);
    CardanoObject<cardano_credential_t, cardano_credential_unref> owner(element);

    checkCardanoError(status, "CredentialSet contains");

    if (cardano_credential_equals(owner.get(), checkedNativeHandle(value)))
    {
      return true;
    }
  }

  return false;
}

Credential
CredentialSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Set index is out of range");
  }

  cardano_credential_t*                                         element = nullptr;
  const cardano_error_t                                         status  = cardano_credential_set_get(checked(), index, &element);
  CardanoObject<cardano_credential_t, cardano_credential_unref> owner(element);

  checkCardanoError(status, "CredentialSet at");
  const Credential value = Credential::adopt(owner.release());

  return Credential(value);
}

std::vector<Credential>
CredentialSet::values() const
{
  std::vector<Credential> result;
  const std::size_t       elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    result.push_back(at(i));
  }

  return result;
}

std::vector<std::uint8_t>
CredentialSet::toCbor() const
{
  return encodeCbor(checked(), cardano_credential_set_to_cbor);
}

std::string
CredentialSet::toJson() const
{
  return encodeJson(checked(), cardano_credential_set_to_cip116_json);
}

void
CredentialSet::swap(CredentialSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

CredentialSet&
CredentialSet::operator=(const CredentialSet& other)
{
  if (this != &other)
  {
    CredentialSet copy(other);
    swap(copy);
  }

  return *this;
}

CredentialSet&
CredentialSet::operator=(CredentialSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_credential_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }

  return *this;
}

bool
CredentialSet::operator==(const CredentialSet& other) const
{
  const std::size_t elementCount = size();

  if (elementCount != other.size())
  {
    return false;
  }

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_credential_t*                                         left       = nullptr;
    const cardano_error_t                                         leftStatus = cardano_credential_set_get(checked(), i, &left);
    CardanoObject<cardano_credential_t, cardano_credential_unref> leftOwner(left);

    checkCardanoError(leftStatus, "CredentialSet comparison");

    cardano_credential_t*                                         right       = nullptr;
    const cardano_error_t                                         rightStatus = cardano_credential_set_get(other.checked(), i, &right);
    CardanoObject<cardano_credential_t, cardano_credential_unref> rightOwner(right);

    checkCardanoError(rightStatus, "CredentialSet comparison");

    if (!cardano_credential_equals(leftOwner.get(), rightOwner.get()))
    {
      return false;
    }
  }

  return true;
}

bool
CredentialSet::operator!=(const CredentialSet& other) const
{
  return !(*this == other);
}

CredentialSet::const_iterator
CredentialSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

CredentialSet::const_iterator
CredentialSet::end() const
{
  materialize();

  return m_elements.cend();
}

CredentialSet::const_iterator
CredentialSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

CredentialSet::const_iterator
CredentialSet::cend() const
{
  materialize();

  return m_elements.cend();
}

CredentialSet::const_reverse_iterator
CredentialSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

CredentialSet::const_reverse_iterator
CredentialSet::rend() const
{
  materialize();

  return m_elements.crend();
}

CredentialSet::const_reverse_iterator
CredentialSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

CredentialSet::const_reverse_iterator
CredentialSet::crend() const
{
  materialize();

  return m_elements.crend();
}

CredentialSet::iterator
CredentialSet::find(const value_type& value) const
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

CredentialSet::size_type
CredentialSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

std::pair<CredentialSet::iterator, bool>
CredentialSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  CredentialSet replacement(*this);
  replacement.insertValue(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

CredentialSet::iterator
CredentialSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
CredentialSet::clear()
{
  CredentialSet replacement;

  swap(replacement);
}

CredentialSet::iterator
CredentialSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

CredentialSet::iterator
CredentialSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  CredentialSet replacement;

  for (size_type index = 0; index < m_elements.size(); ++index)
  {
    if (index < start || index >= finish)
    {
      replacement.insertValue(m_elements[index]);
    }
  }

  const iterator next = std::next(replacement.cbegin(), static_cast<difference_type>(start));
  swap(replacement);

  return next;
}

CredentialSet::size_type
CredentialSet::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
}

CredentialSet
CredentialSet::adopt(cardano_credential_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null CredentialSet");
  }

  return CredentialSet(value);
}

cardano_credential_set_t*
CredentialSet::nativeHandle() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return m_value;
}

const cardano_credential_set_t*
CredentialSet::nativeHandle() const noexcept
{
  return m_value;
}

cardano_credential_set_t*
CredentialSet::release() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

CredentialSet::CredentialSet(cardano_credential_set_t* value) noexcept
: m_value(value)
{
}

cardano_credential_set_t*
CredentialSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CredentialSet");
  }

  return m_value;
}

void
CredentialSet::materialize() const
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
CredentialSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
CredentialSet::sameKey(const value_type& left, const value_type& right)
{
  return left == right;
}

bool
CredentialSet::insertValue(const Credential& value)
{
  if (contains(value))
  {
    return false;
  }

  const Credential copy(value);

  checkCardanoError(cardano_credential_set_add(checked(), checkedNativeHandle(copy)), "CredentialSet insert");
  invalidateElements();

  return true;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CredentialSet& lhs, CredentialSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
