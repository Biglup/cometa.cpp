/**
 * @file TransactionInputSet.cpp
 *
 * @brief Sorted unique TransactionInput set operations.
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

#include <Cometa/TransactionBody/TransactionInputSet.h>

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"

#include <cardano/transaction_body/transaction_input_set.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TransactionInputSet::TransactionInputSet()
: m_value(nullptr)
{
  cardano_transaction_input_set_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_transaction_input_set_new(&value);
  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);

  checkCardanoError(status, "TransactionInputSet");
  m_value = owner.release();
}

TransactionInputSet::TransactionInputSet(std::initializer_list<TransactionInput> values)
: TransactionInputSet()
{
  for (const TransactionInput& value: values)
  {
    insert(value);
  }
}

TransactionInputSet::TransactionInputSet(const TransactionInputSet& other)
: m_value(nullptr)
{
  cardano_transaction_input_set_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_transaction_input_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);

  checkCardanoError(status, "TransactionInputSet copy");
  m_value = owner.release();
}

TransactionInputSet::TransactionInputSet(TransactionInputSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

TransactionInputSet::~TransactionInputSet()
{
  cardano_transaction_input_set_unref(&m_value);
}

TransactionInputSet
TransactionInputSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  const CborReader reader(cbor);

  if (reader.peekState() == CborReaderState::Tag)
  {
    (void)reader.peekTag();
  }

  TransactionInputSet                 result(*decodeCbor<cardano_transaction_input_set_t, cardano_transaction_input_set_unref>(cbor, cardano_transaction_input_set_from_cbor));
  const std::vector<TransactionInput> elements = result.values();

  for (std::size_t i = 1; i < elements.size(); ++i)
  {
    if (elements[i - 1] == elements[i])
    {
      throw std::invalid_argument("Duplicate element in CBOR set");
    }
  }

  return result;
}

TransactionInputSet
TransactionInputSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
TransactionInputSet::size() const
{
  return cardano_transaction_input_set_get_length(checked());
}

bool
TransactionInputSet::empty() const
{
  return size() == 0;
}

bool
TransactionInputSet::wasTagged() const
{
  return cardano_transaction_input_set_is_tagged(checked());
}

bool
TransactionInputSet::contains(const TransactionInput& value) const
{
  const std::size_t elementCount = size();

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_transaction_input_t*                                                element = nullptr;
    const cardano_error_t                                                       status  = cardano_transaction_input_set_get(checked(), i, &element);
    CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> owner(element);

    checkCardanoError(status, "TransactionInputSet contains");

    if (cardano_transaction_input_equals(owner.get(), checkedNativeHandle(value)))
    {
      return true;
    }
  }

  return false;
}

TransactionInput
TransactionInputSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Set index is out of range");
  }

  cardano_transaction_input_t*                                                element = nullptr;
  const cardano_error_t                                                       status  = cardano_transaction_input_set_get(checked(), index, &element);
  CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> owner(element);

  checkCardanoError(status, "TransactionInputSet at");
  const TransactionInput value = TransactionInput::adopt(owner.release());

  return TransactionInput(value);
}

std::vector<TransactionInput>
TransactionInputSet::values() const
{
  std::vector<TransactionInput> result;
  const std::size_t             elementCount = size();
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    result.push_back(at(i));
  }

  return result;
}

std::vector<std::uint8_t>
TransactionInputSet::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_input_set_to_cbor);
}

std::string
TransactionInputSet::toJson() const
{
  return encodeJson(checked(), cardano_transaction_input_set_to_cip116_json);
}

void
TransactionInputSet::swap(TransactionInputSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

TransactionInputSet&
TransactionInputSet::operator=(const TransactionInputSet& other)
{
  if (this != &other)
  {
    TransactionInputSet copy(other);
    swap(copy);
  }

  return *this;
}

TransactionInputSet&
TransactionInputSet::operator=(TransactionInputSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_input_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }

  return *this;
}

bool
TransactionInputSet::operator==(const TransactionInputSet& other) const
{
  const std::size_t elementCount = size();

  if (elementCount != other.size())
  {
    return false;
  }

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_transaction_input_t*                                                left       = nullptr;
    const cardano_error_t                                                       leftStatus = cardano_transaction_input_set_get(checked(), i, &left);
    CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> leftOwner(left);

    checkCardanoError(leftStatus, "TransactionInputSet comparison");

    cardano_transaction_input_t*                                                right       = nullptr;
    const cardano_error_t                                                       rightStatus = cardano_transaction_input_set_get(other.checked(), i, &right);
    CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> rightOwner(right);

    checkCardanoError(rightStatus, "TransactionInputSet comparison");

    if (!cardano_transaction_input_equals(leftOwner.get(), rightOwner.get()))
    {
      return false;
    }
  }

  return true;
}

bool
TransactionInputSet::operator!=(const TransactionInputSet& other) const
{
  return !(*this == other);
}

TransactionInputSet::const_iterator
TransactionInputSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

TransactionInputSet::const_iterator
TransactionInputSet::end() const
{
  materialize();

  return m_elements.cend();
}

TransactionInputSet::const_iterator
TransactionInputSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

TransactionInputSet::const_iterator
TransactionInputSet::cend() const
{
  materialize();

  return m_elements.cend();
}

TransactionInputSet::const_reverse_iterator
TransactionInputSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

TransactionInputSet::const_reverse_iterator
TransactionInputSet::rend() const
{
  materialize();

  return m_elements.crend();
}

TransactionInputSet::const_reverse_iterator
TransactionInputSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

TransactionInputSet::const_reverse_iterator
TransactionInputSet::crend() const
{
  materialize();

  return m_elements.crend();
}

TransactionInputSet::iterator
TransactionInputSet::find(const value_type& value) const
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

TransactionInputSet::size_type
TransactionInputSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

std::pair<TransactionInputSet::iterator, bool>
TransactionInputSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  TransactionInputSet replacement(*this);
  replacement.insertValue(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

TransactionInputSet::iterator
TransactionInputSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
TransactionInputSet::clear()
{
  TransactionInputSet replacement;

  swap(replacement);
}

TransactionInputSet::iterator
TransactionInputSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

TransactionInputSet::iterator
TransactionInputSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  TransactionInputSet replacement;

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

TransactionInputSet::size_type
TransactionInputSet::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
}

TransactionInputSet
TransactionInputSet::adopt(cardano_transaction_input_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null TransactionInputSet");
  }

  return TransactionInputSet(*value);
}

cardano_transaction_input_set_t*
TransactionInputSet::nativeHandle() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return m_value;
}

const cardano_transaction_input_set_t*
TransactionInputSet::nativeHandle() const noexcept
{
  return m_value;
}

cardano_transaction_input_set_t*
TransactionInputSet::release() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

TransactionInputSet::TransactionInputSet(cardano_transaction_input_set_t& value) noexcept
: m_value(&value)
{
}

cardano_transaction_input_set_t*
TransactionInputSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TransactionInputSet");
  }

  return m_value;
}

void
TransactionInputSet::materialize() const
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
TransactionInputSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
TransactionInputSet::sameKey(const value_type& left, const value_type& right)
{
  return left == right;
}

bool
TransactionInputSet::insertValue(const TransactionInput& value)
{
  if (contains(value))
  {
    return false;
  }

  const TransactionInput copy(value);

  checkCardanoError(cardano_transaction_input_set_add(checked(), checkedNativeHandle(copy)), "TransactionInputSet insert");
  invalidateElements();

  return true;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionInputSet& lhs, TransactionInputSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
