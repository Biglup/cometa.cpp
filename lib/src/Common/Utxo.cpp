/**
 * @file Utxo.cpp
 *
 * @brief An unspent transaction output and its reference.
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

#include <Cometa/Common/Utxo.h>

#include "../CardanoInterop.h"

#include <cardano/common/utxo.h>

#include <cardano/transaction_body/transaction_input.h>
#include <cardano/transaction_body/transaction_output.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Utxo::Utxo(const TransactionInput& input, const TransactionOutput& output)
: m_value(nullptr)
{
  TransactionInput  inputCopy(input);
  TransactionOutput outputCopy(output);

  cardano_utxo_t*       value  = nullptr;
  const cardano_error_t result = cardano_utxo_new(inputCopy.checked(), outputCopy.checked(), &value);

  CardanoObject<cardano_utxo_t, cardano_utxo_unref> owner(value);

  checkCardanoError(result, "Utxo");

  m_value = owner.release();
}

Utxo::Utxo(const Utxo& other)
: m_value(nullptr)
{
  cardano_utxo_t*       value  = nullptr;
  const cardano_error_t result = cardano_utxo_deep_clone(other.checked(), &value);

  CardanoObject<cardano_utxo_t, cardano_utxo_unref> owner(value);

  checkCardanoError(result, "Utxo");

  m_value = owner.release();
}

Utxo::Utxo(Utxo&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Utxo::~Utxo()
{
  cardano_utxo_unref(&m_value);
}

Utxo
Utxo::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  Utxo value(*decodeCbor<cardano_utxo_t, cardano_utxo_unref>(cbor, cardano_utxo_from_cbor));

  value.validate();

  return value;
}

Utxo
Utxo::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

TransactionInput
Utxo::input() const
{
  cardano_transaction_input_t*                                                value = cardano_utxo_get_input(checked());
  CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> owner(value);

  const TransactionInput component(*owner.release());

  return TransactionInput(component);
}

void
Utxo::setInput(const TransactionInput& value)
{
  TransactionInput copy(value);

  checkCardanoError(cardano_utxo_set_input(checked(), copy.checked()), "Utxo setInput");
}

TransactionOutput
Utxo::output() const
{
  cardano_transaction_output_t*                                                 value = cardano_utxo_get_output(checked());
  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> owner(value);

  const TransactionOutput component(*owner.release());

  return TransactionOutput(component);
}

void
Utxo::setOutput(const TransactionOutput& value)
{
  TransactionOutput copy(value);

  checkCardanoError(cardano_utxo_set_output(checked(), copy.checked()), "Utxo setOutput");
}

std::vector<std::uint8_t>
Utxo::toCbor() const
{
  return encodeCbor(checked(), cardano_utxo_to_cbor);
}

std::string
Utxo::toJson() const
{
  return encodeJson(checked(), cardano_utxo_to_cip116_json);
}

void
Utxo::swap(Utxo& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Utxo&
Utxo::operator=(const Utxo& other)
{
  if (this != &other)
  {
    Utxo copy(other);

    swap(copy);
  }

  return *this;
}

Utxo&
Utxo::operator=(Utxo&& other) noexcept
{
  if (this != &other)
  {
    cardano_utxo_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
Utxo::operator==(const Utxo& other) const
{
  return cardano_utxo_equals(checked(), other.checked());
}

bool
Utxo::operator!=(const Utxo& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Utxo::Utxo(cardano_utxo_t& value) noexcept
: m_value(&value)
{
}

cardano_utxo_t*
Utxo::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Utxo");
  }

  return m_value;
}

void
Utxo::validate() const
{
  input().validate();
  output().validate();
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Utxo& lhs, Utxo& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
