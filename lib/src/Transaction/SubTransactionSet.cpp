/**
 * @file SubTransactionSet.cpp
 *
 * @brief Ordered Dijkstra sub-transactions.
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

#include <Cometa/Transaction/SubTransactionSet.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/blake2b_hash.h>
#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction_body/sub_transaction_set.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
SubTransactionSet::SubTransactionSet()
: m_value(nullptr)
{
  cardano_sub_transaction_set_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_sub_transaction_set_new(&value);
  CardanoObject<cardano_sub_transaction_set_t, cardano_sub_transaction_set_unref> owner(value);

  checkCardanoError(status, "SubTransactionSet");
  m_value = owner.release();
}

SubTransactionSet::SubTransactionSet(const SubTransactionSet& other)
: m_value(nullptr)
{
  cardano_sub_transaction_set_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_sub_transaction_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_sub_transaction_set_t, cardano_sub_transaction_set_unref> owner(value);

  checkCardanoError(status, "SubTransactionSet copy");
  m_value = owner.release();
}

SubTransactionSet::SubTransactionSet(SubTransactionSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_materialized(std::exchange(other.m_materialized, false))
{
}

SubTransactionSet::~SubTransactionSet()
{
  cardano_sub_transaction_set_unref(&m_value);
}

SubTransactionSet
SubTransactionSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return SubTransactionSet(decodeCbor<cardano_sub_transaction_set_t, cardano_sub_transaction_set_unref>(cbor, cardano_sub_transaction_set_from_cbor));
}

SubTransactionSet
SubTransactionSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
SubTransactionSet::size() const
{
  return cardano_sub_transaction_set_get_length(checked());
}

bool
SubTransactionSet::empty() const
{
  return size() == 0U;
}

void
SubTransactionSet::add(const SubTransaction& value)
{
  const SubTransaction copy(value);
  checkCardanoError(cardano_sub_transaction_set_add(checked(), checkedNativeHandle(copy)), "SubTransactionSet add");
  invalidateElements();
}

SubTransaction
SubTransactionSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Sub-transaction index is out of range");
  }

  cardano_sub_transaction_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_sub_transaction_set_get(checked(), index, &value);
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> owner(value);

  checkCardanoError(status, "SubTransactionSet at");
  const SubTransaction copied = SubTransaction::adopt(owner.release());
  return SubTransaction(copied);
}

std::optional<SubTransaction>
SubTransactionSet::findById(const Blake2bHash& id) const
{
  cardano_sub_transaction_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_sub_transaction_set_find_by_id(checked(), checkedNativeHandle(id), &value);
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "SubTransactionSet findById");
  const SubTransaction copied = SubTransaction::adopt(owner.release());
  return SubTransaction(copied);
}

bool
SubTransactionSet::useTag() const
{
  return cardano_sub_transaction_set_is_tagged(checked());
}

void
SubTransactionSet::setUseTag(bool value)
{
  checkCardanoError(cardano_sub_transaction_set_set_use_tag(checked(), value), "SubTransactionSet setUseTag");
}

std::vector<std::uint8_t>
SubTransactionSet::toCbor() const
{
  return encodeCbor(checked(), cardano_sub_transaction_set_to_cbor);
}

std::string
SubTransactionSet::toJson() const
{
  return encodeJson(checked(), cardano_sub_transaction_set_to_cip116_json);
}

void
SubTransactionSet::swap(SubTransactionSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  std::swap(m_materialized, other.m_materialized);
}

SubTransactionSet&
SubTransactionSet::operator=(const SubTransactionSet& other)
{
  if (this != &other)
  {
    SubTransactionSet copy(other);
    swap(copy);
  }
  return *this;
}

SubTransactionSet&
SubTransactionSet::operator=(SubTransactionSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_sub_transaction_set_unref(&m_value);
    m_value        = std::exchange(other.m_value, nullptr);
    m_elements     = std::move(other.m_elements);
    m_materialized = std::exchange(other.m_materialized, false);
  }
  return *this;
}

bool
SubTransactionSet::operator==(const SubTransactionSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
SubTransactionSet::operator!=(const SubTransactionSet& other) const
{
  return !(*this == other);
}

SubTransactionSet::const_iterator
SubTransactionSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

SubTransactionSet::const_iterator
SubTransactionSet::end() const
{
  materialize();

  return m_elements.cend();
}

SubTransactionSet::const_iterator
SubTransactionSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

SubTransactionSet::const_iterator
SubTransactionSet::cend() const
{
  materialize();

  return m_elements.cend();
}

SubTransactionSet::const_reverse_iterator
SubTransactionSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

SubTransactionSet::const_reverse_iterator
SubTransactionSet::rend() const
{
  materialize();

  return m_elements.crend();
}

SubTransactionSet::const_reverse_iterator
SubTransactionSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

SubTransactionSet::const_reverse_iterator
SubTransactionSet::crend() const
{
  materialize();

  return m_elements.crend();
}

SubTransactionSet::iterator
SubTransactionSet::find(const value_type& value) const
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

SubTransactionSet::size_type
SubTransactionSet::count(const value_type& value) const
{
  return find(value) == end() ? 0U : 1U;
}

bool
SubTransactionSet::contains(const value_type& value) const
{
  return find(value) != end();
}

std::pair<SubTransactionSet::iterator, bool>
SubTransactionSet::insert(const value_type& value)
{
  const iterator found = find(value);

  if (found != end())
  {
    return { found, false };
  }

  SubTransactionSet replacement(*this);
  replacement.add(value);
  const iterator inserted = replacement.find(value);
  swap(replacement);

  return { inserted, true };
}

SubTransactionSet::iterator
SubTransactionSet::insert(const_iterator hint, const value_type& value)
{
  (void)hint;

  return insert(value).first;
}

void
SubTransactionSet::clear()
{
  SubTransactionSet replacement;
  replacement.setUseTag(useTag());

  swap(replacement);
}

SubTransactionSet::iterator
SubTransactionSet::erase(const_iterator position)
{
  return erase(position, std::next(position));
}

SubTransactionSet::iterator
SubTransactionSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  const size_type start  = static_cast<size_type>(std::distance(cbegin(), first));
  const size_type finish = static_cast<size_type>(std::distance(cbegin(), last));

  if (start == finish)
  {
    return first;
  }

  SubTransactionSet replacement;
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

SubTransactionSet::size_type
SubTransactionSet::erase(const value_type& value)
{
  const iterator position = find(value);

  if (position == end())
  {
    return 0;
  }

  erase(position);

  return 1;
}

SubTransactionSet
SubTransactionSet::adopt(cardano_sub_transaction_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null SubTransactionSet");
  }

  return SubTransactionSet(value);
}

cardano_sub_transaction_set_t*
SubTransactionSet::nativeHandle() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return m_value;
}

const cardano_sub_transaction_set_t*
SubTransactionSet::nativeHandle() const noexcept
{
  return m_value;
}

cardano_sub_transaction_set_t*
SubTransactionSet::release() noexcept
{
  m_elements.clear();
  m_materialized = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

SubTransactionSet::SubTransactionSet(cardano_sub_transaction_set_t* value) noexcept
: m_value(value)
{
}

cardano_sub_transaction_set_t*
SubTransactionSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from SubTransactionSet");
  }
  return m_value;
}

void
SubTransactionSet::materialize() const
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
SubTransactionSet::invalidateElements() noexcept
{
  m_elements.clear();
  m_materialized = false;
}

bool
SubTransactionSet::sameKey(const value_type& left, const value_type& right)
{
  return left.id() == right.id();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SubTransactionSet& lhs, SubTransactionSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
