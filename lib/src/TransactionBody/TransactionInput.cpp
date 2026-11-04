/**
 * @file TransactionInput.cpp
 *
 * @brief A reference to a transaction output.
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

#include <Cometa/TransactionBody/TransactionInput.h>

#include "../CardanoInterop.h"
#include "../ValueValidation.h"

#include <cardano/transaction_body/transaction_input.h>

#include <cardano/crypto/blake2b_hash.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TransactionInput::TransactionInput(const Blake2bHash& id, std::uint64_t index)
: m_value(nullptr)
{
  Blake2bHash copy(id);

  cardano_transaction_input_t* value  = nullptr;
  const cardano_error_t        result = cardano_transaction_input_new(checkedNativeHandle(copy), index, &value);

  CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> owner(value);

  checkCardanoError(result, "TransactionInput");

  m_value = owner.release();
}

TransactionInput::TransactionInput(const TransactionInput& other)
: m_value(nullptr)
{
  cardano_transaction_input_t* value  = nullptr;
  const cardano_error_t        result = cardano_transaction_input_deep_clone(other.checked(), &value);

  CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> owner(value);

  checkCardanoError(result, "TransactionInput");

  m_value = owner.release();
}

TransactionInput::TransactionInput(TransactionInput&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

TransactionInput::~TransactionInput()
{
  cardano_transaction_input_unref(&m_value);
}

TransactionInput
TransactionInput::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  TransactionInput value(*decodeCbor<cardano_transaction_input_t, cardano_transaction_input_unref>(cbor, cardano_transaction_input_from_cbor));

  validateValue(value);

  return value;
}

TransactionInput
TransactionInput::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

TransactionInput
TransactionInput::fromHex(std::string_view idHex, std::uint64_t index)
{
  return TransactionInput(Blake2bHash::fromHex(idHex), index);
}

Blake2bHash
TransactionInput::id() const
{
  cardano_blake2b_hash_t*                                           value = cardano_transaction_input_get_id(checked());
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  const Blake2bHash component = Blake2bHash::adopt(owner.release());

  return Blake2bHash(component);
}

void
TransactionInput::setId(const Blake2bHash& value)
{
  Blake2bHash copy(value);

  if (copy.toBytes().size() != 32U)
  {
    throw std::invalid_argument("Transaction ID must be 32 bytes");
  }

  checkCardanoError(cardano_transaction_input_set_id(checked(), checkedNativeHandle(copy)), "TransactionInput setId");
}

std::uint64_t
TransactionInput::index() const
{
  return cardano_transaction_input_get_index(checked());
}

void
TransactionInput::setIndex(std::uint64_t index)
{
  checkCardanoError(cardano_transaction_input_set_index(checked(), index), "TransactionInput setIndex");
}

bool
TransactionInput::operator<(const TransactionInput& other) const
{
  return cardano_transaction_input_compare(checked(), other.checked()) < 0;
}

bool
TransactionInput::operator<=(const TransactionInput& other) const
{
  return cardano_transaction_input_compare(checked(), other.checked()) <= 0;
}

bool
TransactionInput::operator>(const TransactionInput& other) const
{
  return cardano_transaction_input_compare(checked(), other.checked()) > 0;
}

bool
TransactionInput::operator>=(const TransactionInput& other) const
{
  return cardano_transaction_input_compare(checked(), other.checked()) >= 0;
}

std::vector<std::uint8_t>
TransactionInput::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_input_to_cbor);
}

std::string
TransactionInput::toJson() const
{
  return encodeJson(checked(), cardano_transaction_input_to_cip116_json);
}

void
TransactionInput::swap(TransactionInput& other) noexcept
{
  std::swap(m_value, other.m_value);
}

TransactionInput&
TransactionInput::operator=(const TransactionInput& other)
{
  if (this != &other)
  {
    TransactionInput copy(other);

    swap(copy);
  }

  return *this;
}

TransactionInput&
TransactionInput::operator=(TransactionInput&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_input_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
TransactionInput::operator==(const TransactionInput& other) const
{
  return cardano_transaction_input_equals(checked(), other.checked());
}

bool
TransactionInput::operator!=(const TransactionInput& other) const
{
  return !(*this == other);
}

TransactionInput
TransactionInput::adopt(cardano_transaction_input_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null TransactionInput");
  }

  return TransactionInput(*value);
}

cardano_transaction_input_t*
TransactionInput::nativeHandle() noexcept
{
  return m_value;
}

const cardano_transaction_input_t*
TransactionInput::nativeHandle() const noexcept
{
  return m_value;
}

cardano_transaction_input_t*
TransactionInput::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

TransactionInput::TransactionInput(cardano_transaction_input_t& value) noexcept
: m_value(&value)
{
}

cardano_transaction_input_t*
TransactionInput::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TransactionInput");
  }

  return m_value;
}

void
validateValue(const TransactionInput& value)
{
  if (value.id().toBytes().size() != 32U)
  {
    throw std::invalid_argument("Transaction ID must be 32 bytes");
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionInput& lhs, TransactionInput& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
