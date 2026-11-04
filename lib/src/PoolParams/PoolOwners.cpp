/**
 * @file PoolOwners.cpp
 *
 * @brief Sorted unique Blake2bHash set operations.
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

#include <Cometa/PoolParams/PoolOwners.h>

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/pool_owners.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolOwners::PoolOwners()
: m_value(nullptr)
{
  cardano_pool_owners_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_owners_new(&value);
  CardanoObject<cardano_pool_owners_t, cardano_pool_owners_unref> owner(value);

  checkCardanoError(status, "PoolOwners");
  m_value = owner.release();
}

PoolOwners::PoolOwners(std::initializer_list<Blake2bHash> values)
: PoolOwners()
{
  for (const Blake2bHash& value: values)
  {
    insert(value);
  }
}

PoolOwners::PoolOwners(const PoolOwners& other)
: m_value(nullptr)
{
  cardano_pool_owners_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_pool_owners_deep_clone(other.checked(), &value);
  CardanoObject<cardano_pool_owners_t, cardano_pool_owners_unref> owner(value);

  checkCardanoError(status, "PoolOwners copy");
  m_value = owner.release();
}

PoolOwners::PoolOwners(PoolOwners&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

PoolOwners::~PoolOwners()
{
  cardano_pool_owners_unref(&m_value);
}

PoolOwners
PoolOwners::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  const CborReader reader(cbor);

  if (reader.peekState() == CborReaderState::Tag)
  {
    (void)reader.peekTag();
  }

  PoolOwners                     result(decodeCbor<cardano_pool_owners_t, cardano_pool_owners_unref>(cbor, cardano_pool_owners_from_cbor));
  const std::vector<Blake2bHash> elements = result.values();

  for (const Blake2bHash& element: elements)
  {
    validateHash(element);
  }

  for (std::size_t i = 1; i < elements.size(); ++i)
  {
    if (elements[i - 1] == elements[i])
    {
      throw std::invalid_argument("Duplicate element in CBOR set");
    }
  }

  return result;
}

PoolOwners
PoolOwners::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
PoolOwners::size() const
{
  return cardano_pool_owners_get_length(checked());
}

bool
PoolOwners::empty() const
{
  return size() == 0;
}

bool
PoolOwners::contains(const Blake2bHash& value) const
{
  validateHash(value);
  const std::size_t elementCount = size();

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_blake2b_hash_t*                                           element = nullptr;
    const cardano_error_t                                             status  = cardano_pool_owners_get(checked(), i, &element);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(element);

    checkCardanoError(status, "PoolOwners contains");

    if (cardano_blake2b_hash_equals(owner.get(), checkedNativeHandle(value)))
    {
      return true;
    }
  }

  return false;
}

Blake2bHash
PoolOwners::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Set index is out of range");
  }

  cardano_blake2b_hash_t*                                           element = nullptr;
  const cardano_error_t                                             status  = cardano_pool_owners_get(checked(), index, &element);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(element);

  checkCardanoError(status, "PoolOwners at");
  const Blake2bHash value = Blake2bHash::adopt(owner.release());

  return Blake2bHash(value);
}

std::vector<Blake2bHash>
PoolOwners::values() const
{
  std::vector<Blake2bHash> result;
  const std::size_t        elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    result.push_back(at(i));
  }

  return result;
}

std::vector<std::uint8_t>
PoolOwners::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_owners_to_cbor);
}

std::string
PoolOwners::toJson() const
{
  return encodeJson(checked(), cardano_pool_owners_to_cip116_json);
}

void
PoolOwners::swap(PoolOwners& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

PoolOwners&
PoolOwners::operator=(const PoolOwners& other)
{
  if (this != &other)
  {
    PoolOwners copy(other);
    swap(copy);
  }

  return *this;
}

PoolOwners&
PoolOwners::operator=(PoolOwners&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_owners_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }

  return *this;
}

bool
PoolOwners::operator==(const PoolOwners& other) const
{
  const std::size_t elementCount = size();

  if (elementCount != other.size())
  {
    return false;
  }

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_blake2b_hash_t*                                           left       = nullptr;
    const cardano_error_t                                             leftStatus = cardano_pool_owners_get(checked(), i, &left);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> leftOwner(left);

    checkCardanoError(leftStatus, "PoolOwners comparison");

    cardano_blake2b_hash_t*                                           right       = nullptr;
    const cardano_error_t                                             rightStatus = cardano_pool_owners_get(other.checked(), i, &right);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> rightOwner(right);

    checkCardanoError(rightStatus, "PoolOwners comparison");

    if (!cardano_blake2b_hash_equals(leftOwner.get(), rightOwner.get()))
    {
      return false;
    }
  }

  return true;
}

bool
PoolOwners::operator!=(const PoolOwners& other) const
{
  return !(*this == other);
}

PoolOwners::const_iterator
PoolOwners::begin() const
{
  materialize();

  return m_elements.cbegin();
}

PoolOwners::const_iterator
PoolOwners::end() const
{
  materialize();

  return m_elements.cend();
}

PoolOwners::const_iterator
PoolOwners::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

PoolOwners::const_iterator
PoolOwners::cend() const
{
  materialize();

  return m_elements.cend();
}

PoolOwners::const_reverse_iterator
PoolOwners::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PoolOwners::const_reverse_iterator
PoolOwners::rend() const
{
  materialize();

  return m_elements.crend();
}

PoolOwners::const_reverse_iterator
PoolOwners::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

PoolOwners::const_reverse_iterator
PoolOwners::crend() const
{
  materialize();

  return m_elements.crend();
}

PoolOwners::iterator
PoolOwners::find(const value_type& value) const
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

PoolOwners::size_type
PoolOwners::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

std::pair<PoolOwners::iterator, bool>
PoolOwners::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  PoolOwners replacement(*this);
  replacement.insertValue(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

PoolOwners::iterator
PoolOwners::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
PoolOwners::clear()
{
  PoolOwners replacement;

  swap(replacement);
}

PoolOwners::iterator
PoolOwners::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

PoolOwners::iterator
PoolOwners::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  PoolOwners replacement;

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

PoolOwners::size_type
PoolOwners::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
}

PoolOwners
PoolOwners::adopt(cardano_pool_owners_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PoolOwners");
  }

  return PoolOwners(value);
}

cardano_pool_owners_t*
PoolOwners::nativeHandle() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return m_value;
}

const cardano_pool_owners_t*
PoolOwners::nativeHandle() const noexcept
{
  return m_value;
}

cardano_pool_owners_t*
PoolOwners::release() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolOwners::PoolOwners(cardano_pool_owners_t* value) noexcept
: m_value(value)
{
}

cardano_pool_owners_t*
PoolOwners::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolOwners");
  }

  return m_value;
}

void
PoolOwners::validateHash(const Blake2bHash& value)
{
  if (value.toBytes().size() != 28U)
  {
    throw std::invalid_argument("Pool owner key hash must be 28 bytes");
  }
}

void
PoolOwners::materialize() const
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
PoolOwners::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
PoolOwners::sameKey(const value_type& left, const value_type& right)
{
  return left == right;
}

bool
PoolOwners::insertValue(const Blake2bHash& value)
{
  validateHash(value);
  if (contains(value))
  {
    return false;
  }

  const Blake2bHash copy(value);

  checkCardanoError(cardano_pool_owners_add(checked(), checkedNativeHandle(copy)), "PoolOwners insert");
  invalidateElements();

  return true;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolOwners& lhs, PoolOwners& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
