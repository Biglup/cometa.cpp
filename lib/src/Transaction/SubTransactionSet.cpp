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
  checkCardanoError(cardano_sub_transaction_set_add(checked(), copy.checked()), "SubTransactionSet add");
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
  const SubTransaction copied(owner.release());
  return SubTransaction(copied);
}

std::optional<SubTransaction>
SubTransactionSet::findById(const Blake2bHash& id) const
{
  cardano_sub_transaction_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_sub_transaction_set_find_by_id(checked(), id.checked(), &value);
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "SubTransactionSet findById");
  const SubTransaction copied(owner.release());
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
    m_value = std::exchange(other.m_value, nullptr);
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

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SubTransactionSet& lhs, SubTransactionSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
