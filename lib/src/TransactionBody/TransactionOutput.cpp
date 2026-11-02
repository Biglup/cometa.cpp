/**
 * @file TransactionOutput.cpp
 *
 * @brief An address, value, and optional output data.
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

#include <Cometa/TransactionBody/TransactionOutput.h>

#include "../CardanoInterop.h"

#include <cardano/transaction_body/transaction_output.h>

#include <cardano/address/address.h>
#include <cardano/common/datum.h>
#include <cardano/scripts/script.h>
#include <cardano/transaction_body/value.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TransactionOutput::TransactionOutput(const Address& address, const Value& amount)
: m_value(nullptr)
{
  Address addressCopy(address);
  Value   amountCopy(amount);

  if (amountCopy.coin() < 0)
  {
    throw std::invalid_argument("Output lovelace must not be negative");
  }

  cardano_transaction_output_t* value  = nullptr;
  const cardano_error_t         result = cardano_transaction_output_new(addressCopy.checked(), 0U, &value);

  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> owner(value);

  checkCardanoError(result, "TransactionOutput");
  checkCardanoError(cardano_transaction_output_set_value(value, amountCopy.checked()), "TransactionOutput amount");

  m_value = owner.release();
}

TransactionOutput::TransactionOutput(const TransactionOutput& other)
: m_value(nullptr)
{
  cardano_transaction_output_t* value  = nullptr;
  const cardano_error_t         result = cardano_transaction_output_deep_clone(other.checked(), &value);

  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> owner(value);

  checkCardanoError(result, "TransactionOutput");

  m_value = owner.release();
}

TransactionOutput::TransactionOutput(TransactionOutput&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

TransactionOutput::~TransactionOutput()
{
  cardano_transaction_output_unref(&m_value);
}

TransactionOutput
TransactionOutput::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  TransactionOutput value(*decodeCbor<cardano_transaction_output_t, cardano_transaction_output_unref>(cbor, cardano_transaction_output_from_cbor));

  value.validate();

  return value;
}

TransactionOutput
TransactionOutput::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Address
TransactionOutput::address() const
{
  cardano_address_t*                                      value = cardano_transaction_output_get_address(checked());
  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);

  const Address component(*owner.release());

  return Address(component);
}

void
TransactionOutput::setAddress(const Address& value)
{
  Address copy(value);

  checkCardanoError(cardano_transaction_output_set_address(checked(), copy.checked()), "TransactionOutput setAddress");
}

Value
TransactionOutput::amount() const
{
  cardano_value_t*                                    value = cardano_transaction_output_get_value(checked());
  CardanoObject<cardano_value_t, cardano_value_unref> owner(value);

  const Value component(*owner.release());

  return Value(component);
}

void
TransactionOutput::setAmount(const Value& value)
{
  Value copy(value);

  if (copy.coin() < 0)
  {
    throw std::invalid_argument("Output lovelace must not be negative");
  }

  checkCardanoError(cardano_transaction_output_set_value(checked(), copy.checked()), "TransactionOutput setAmount");
}

std::optional<Datum>
TransactionOutput::datum() const
{
  cardano_datum_t*                                    value = cardano_transaction_output_get_datum(checked());
  CardanoObject<cardano_datum_t, cardano_datum_unref> owner(value);

  if (value == nullptr)
  {
    return std::nullopt;
  }

  const Datum component(*owner.release());

  return Datum(component);
}

void
TransactionOutput::setDatum(const std::optional<Datum>& value)
{
  std::optional<Datum> copy(value);

  checkCardanoError(cardano_transaction_output_set_datum(checked(), copy ? copy->checked() : nullptr), "TransactionOutput setDatum");
}

std::optional<Script>
TransactionOutput::scriptRef() const
{
  cardano_script_t*                                     value = cardano_transaction_output_get_script_ref(checked());
  CardanoObject<cardano_script_t, cardano_script_unref> owner(value);

  if (value == nullptr)
  {
    return std::nullopt;
  }

  const Script component(*owner.release());

  return Script(component);
}

void
TransactionOutput::setScriptRef(const std::optional<Script>& value)
{
  std::optional<Script> copy(value);

  checkCardanoError(cardano_transaction_output_set_script_ref(checked(), copy ? copy->checked() : nullptr), "TransactionOutput setScriptRef");
}

std::vector<std::uint8_t>
TransactionOutput::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_output_to_cbor);
}

std::string
TransactionOutput::toJson() const
{
  return encodeJson(checked(), cardano_transaction_output_to_cip116_json);
}

void
TransactionOutput::swap(TransactionOutput& other) noexcept
{
  std::swap(m_value, other.m_value);
}

TransactionOutput&
TransactionOutput::operator=(const TransactionOutput& other)
{
  if (this != &other)
  {
    TransactionOutput copy(other);

    swap(copy);
  }

  return *this;
}

TransactionOutput&
TransactionOutput::operator=(TransactionOutput&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_output_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
TransactionOutput::operator==(const TransactionOutput& other) const
{
  return cardano_transaction_output_equals(checked(), other.checked());
}

bool
TransactionOutput::operator!=(const TransactionOutput& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

TransactionOutput::TransactionOutput(cardano_transaction_output_t& value) noexcept
: m_value(&value)
{
}

cardano_transaction_output_t*
TransactionOutput::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TransactionOutput");
  }

  return m_value;
}

void
TransactionOutput::validate() const
{
  if (amount().coin() < 0)
  {
    throw std::invalid_argument("Output lovelace must not be negative");
  }

  const std::optional<Script> reference = scriptRef();
  if (reference)
  {
    reference->validate();
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionOutput& lhs, TransactionOutput& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
