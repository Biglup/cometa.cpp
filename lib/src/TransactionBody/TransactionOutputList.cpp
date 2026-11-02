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
#include <Cometa/TransactionBody/TransactionOutput.h>

#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/transaction_output_list.h>

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

  value.validate();

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
  return cardano_transaction_output_list_get_length(checked());
}

bool
TransactionOutputList::empty() const
{
  return size() == 0U;
}

TransactionOutput
TransactionOutputList::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("TransactionOutputList index");
  }

  cardano_transaction_output_t* value  = nullptr;
  const cardano_error_t         result = cardano_transaction_output_list_get(checked(), index, &value);

  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> owner(value);

  checkCardanoError(result, "TransactionOutputList at");

  const TransactionOutput output(*owner.release());

  return TransactionOutput(output);
}

void
TransactionOutputList::pushBack(const TransactionOutput& output)
{
  TransactionOutput copy(output);

  checkCardanoError(cardano_transaction_output_list_add(checked(), copy.checked()), "TransactionOutputList pushBack");
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
    m_value = std::exchange(other.m_value, nullptr);
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

  return m_value;
}

void
TransactionOutputList::validate() const
{
  const std::size_t count = size();

  for (std::size_t index = 0U; index < count; ++index)
  {
    at(index).validate();
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionOutputList& lhs, TransactionOutputList& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
