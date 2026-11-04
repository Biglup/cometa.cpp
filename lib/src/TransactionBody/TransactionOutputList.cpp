/**
 * @file TransactionOutputList.cpp
 *
 * @brief An ordered sequence of transaction outputs.
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

#include <Cometa/TransactionBody/TransactionOutputList.h>

#include "../CardanoInterop.h"
#include "../ValueValidation.h"
#include <Cometa/TransactionBody/TransactionOutput.h>

#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/transaction_output_list.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TransactionOutputList::TransactionOutputList()
: m_value(nullptr)
{
  cardano_transaction_output_list_t* value  = nullptr;
  const cardano_error_t              result = cardano_transaction_output_list_new(&value);

  CardanoObject<cardano_transaction_output_list_t, cardano_transaction_output_list_unref> owner(value);

  checkCardanoError(result, "TransactionOutputList");

  m_value = owner.release();
}

TransactionOutputList::TransactionOutputList(std::initializer_list<TransactionOutput> outputs)
: TransactionOutputList()
{
  for (const TransactionOutput& output: outputs)
  {
    pushBack(output);
  }
}

TransactionOutputList::TransactionOutputList(const TransactionOutputList& other)
: m_value(nullptr)
{
  cardano_transaction_output_list_t* value  = nullptr;
  const cardano_error_t              result = cardano_transaction_output_list_deep_clone(other.checked(), &value);

  CardanoObject<cardano_transaction_output_list_t, cardano_transaction_output_list_unref> owner(value);

  checkCardanoError(result, "TransactionOutputList");

  m_value = owner.release();
}

TransactionOutputList::TransactionOutputList(TransactionOutputList&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

TransactionOutputList::~TransactionOutputList()
{
  cardano_transaction_output_list_unref(&m_value);
}

TransactionOutputList
TransactionOutputList::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  TransactionOutputList value(*decodeCbor<cardano_transaction_output_list_t, cardano_transaction_output_list_unref>(cbor, cardano_transaction_output_list_from_cbor));

  validateValue(value);

  return value;
}

TransactionOutputList
TransactionOutputList::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
TransactionOutputList::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_transaction_output_list_get_length(checked());
}

bool
TransactionOutputList::empty() const
{
  return size() == 0U;
}

void
TransactionOutputList::pushBack(const TransactionOutput& output)
{
  if (m_materialized)
  {
    push_back(output);
    return;
  }

  TransactionOutput copy(output);

  checkCardanoError(cardano_transaction_output_list_add(checked(), checkedNativeHandle(copy)), "TransactionOutputList pushBack");
}

std::vector<TransactionOutput>
TransactionOutputList::values() const
{
  std::vector<TransactionOutput> result;
  const std::size_t              count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
TransactionOutputList::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_output_list_to_cbor);
}

std::string
TransactionOutputList::toJson() const
{
  return encodeJson(checked(), cardano_transaction_output_list_to_cip116_json);
}

void
TransactionOutputList::swap(TransactionOutputList& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

TransactionOutputList&
TransactionOutputList::operator=(const TransactionOutputList& other)
{
  if (this != &other)
  {
    TransactionOutputList copy(other);

    swap(copy);
  }

  return *this;
}

TransactionOutputList&
TransactionOutputList::operator=(TransactionOutputList&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_output_list_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
TransactionOutputList::operator==(const TransactionOutputList& other) const
{
  return values() == other.values();
}

bool
TransactionOutputList::operator!=(const TransactionOutputList& other) const
{
  return !(*this == other);
}

TransactionOutputList::const_reference
TransactionOutputList::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

TransactionOutputList::iterator
TransactionOutputList::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

TransactionOutputList::iterator
TransactionOutputList::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

TransactionOutputList::const_iterator
TransactionOutputList::begin() const
{
  materialize();

  return m_elements.cbegin();
}

TransactionOutputList::const_iterator
TransactionOutputList::end() const
{
  materialize();

  return m_elements.cend();
}

TransactionOutputList::const_iterator
TransactionOutputList::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

TransactionOutputList::const_iterator
TransactionOutputList::cend() const
{
  materialize();

  return m_elements.cend();
}

TransactionOutputList::reverse_iterator
TransactionOutputList::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

TransactionOutputList::reverse_iterator
TransactionOutputList::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

TransactionOutputList::const_reverse_iterator
TransactionOutputList::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

TransactionOutputList::const_reverse_iterator
TransactionOutputList::rend() const
{
  materialize();

  return m_elements.crend();
}

TransactionOutputList::const_reverse_iterator
TransactionOutputList::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

TransactionOutputList::const_reverse_iterator
TransactionOutputList::crend() const
{
  materialize();

  return m_elements.crend();
}

TransactionOutputList::reference
TransactionOutputList::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

TransactionOutputList::reference
TransactionOutputList::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

TransactionOutputList::const_reference
TransactionOutputList::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

TransactionOutputList::reference
TransactionOutputList::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

TransactionOutputList::const_reference
TransactionOutputList::front() const
{
  materialize();

  return m_elements.front();
}

TransactionOutputList::reference
TransactionOutputList::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

TransactionOutputList::const_reference
TransactionOutputList::back() const
{
  materialize();

  return m_elements.back();
}

void
TransactionOutputList::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
TransactionOutputList::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
TransactionOutputList::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
TransactionOutputList::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
TransactionOutputList::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

TransactionOutputList::size_type
TransactionOutputList::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
TransactionOutputList::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

TransactionOutputList::iterator
TransactionOutputList::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

TransactionOutputList::iterator
TransactionOutputList::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

TransactionOutputList::iterator
TransactionOutputList::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

TransactionOutputList::iterator
TransactionOutputList::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

TransactionOutputList
TransactionOutputList::adopt(cardano_transaction_output_list_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null TransactionOutputList");
  }

  return TransactionOutputList(*value);
}

cardano_transaction_output_list_t*
TransactionOutputList::nativeHandle()
{
  if (m_value)
  {
    synchronize();
  }

  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;

  return m_value;
}

const cardano_transaction_output_list_t*
TransactionOutputList::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_transaction_output_list_t*
TransactionOutputList::release()
{
  if (m_value)
  {
    synchronize();
  }

  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

TransactionOutputList::TransactionOutputList(cardano_transaction_output_list_t& value) noexcept
: m_value(&value)
{
}

cardano_transaction_output_list_t*
TransactionOutputList::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TransactionOutputList");
  }

  synchronize();

  return m_value;
}

void
validateValue(const TransactionOutputList& value)
{
  const std::size_t count = value.size();

  for (std::size_t index = 0U; index < count; ++index)
  {
    validateValue(value.at(index));
  }
}

void
TransactionOutputList::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  const size_type                        count = size();
  std::vector<value_type>                elements;
  std::vector<std::vector<std::uint8_t>> encoding;
  elements.reserve(count);
  encoding.reserve(count);

  for (size_type index = 0; index < count; ++index)
  {
    elements.push_back(copyElement(index));
    encoding.push_back(elements.back().toCbor());
  }

  m_elements.swap(elements);
  m_encoding.swap(encoding);
  m_materialized = true;
}

void
TransactionOutputList::synchronize() const
{
  if (!m_materialized || !m_mutableAccess)
  {
    return;
  }

  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size());

  std::transform(
    m_elements.cbegin(),
    m_elements.cend(),
    std::back_inserter(encoding),
    std::mem_fn(&value_type::toCbor));

  if (encoding == m_encoding)
  {
    return;
  }

  TransactionOutputList replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

TransactionOutput
TransactionOutputList::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("TransactionOutputList index");
  }

  cardano_transaction_output_t* value  = nullptr;
  const cardano_error_t         result = cardano_transaction_output_list_get(checked(), index, &value);

  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> owner(value);

  checkCardanoError(result, "TransactionOutputList at");

  const TransactionOutput output = TransactionOutput::adopt(owner.release());

  return TransactionOutput(output);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionOutputList& lhs, TransactionOutputList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
