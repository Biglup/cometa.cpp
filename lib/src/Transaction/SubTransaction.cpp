/**
 * @file SubTransaction.cpp
 *
 * @brief Dijkstra sub-transactions and witness application.
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

#include <Cometa/Transaction/SubTransaction.h>

#include "../CardanoInterop.h"

#include <cardano/auxiliary_data/auxiliary_data.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction_body/sub_transaction_body.h>
#include <cardano/witness_set/vkey_witness_set.h>
#include <cardano/witness_set/witness_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
SubTransaction::SubTransaction(const SubTransactionBody& body, const WitnessSet& witnesses, const std::optional<AuxiliaryData>& auxiliaryData)
: m_value(nullptr)
{
  const SubTransactionBody                                                bodyCopy(body);
  const WitnessSet                                                        witnessCopy(witnesses);
  std::optional<AuxiliaryData>                                            auxiliaryCopy(auxiliaryData);
  cardano_sub_transaction_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_sub_transaction_new(bodyCopy.checked(), witnessCopy.checked(), auxiliaryCopy ? auxiliaryCopy->checked() : nullptr, &value);
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> owner(value);

  checkCardanoError(status, "SubTransaction");
  m_value = owner.release();
}

SubTransaction::SubTransaction(const SubTransaction& other)
: m_value(nullptr)
{
  cardano_sub_transaction_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_sub_transaction_deep_clone(other.checked(), &value);
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> owner(value);

  checkCardanoError(status, "SubTransaction copy");
  m_value = owner.release();
}

SubTransaction::SubTransaction(SubTransaction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

SubTransaction::~SubTransaction()
{
  cardano_sub_transaction_unref(&m_value);
}

SubTransaction
SubTransaction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return SubTransaction(decodeCbor<cardano_sub_transaction_t, cardano_sub_transaction_unref>(cbor, cardano_sub_transaction_from_cbor));
}

SubTransaction
SubTransaction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

SubTransactionBody
SubTransaction::body() const
{
  cardano_sub_transaction_body_t* value = cardano_sub_transaction_get_body(checked());
  if (!value)
  {
    throw std::logic_error("SubTransaction has no body");
  }

  CardanoObject<cardano_sub_transaction_body_t, cardano_sub_transaction_body_unref> owner(value);
  const SubTransactionBody                                                          copied(owner.release());
  return SubTransactionBody(copied);
}

void
SubTransaction::setBody(const SubTransactionBody& value)
{
  const SubTransactionBody copy(value);
  checkCardanoError(cardano_sub_transaction_set_body(checked(), copy.checked()), "SubTransaction setBody");
  cardano_sub_transaction_clear_cbor_cache(checked());
}

WitnessSet
SubTransaction::witnessSet() const
{
  cardano_witness_set_t* value = cardano_sub_transaction_get_witness_set(checked());
  if (!value)
  {
    throw std::logic_error("SubTransaction has no witness set");
  }

  CardanoObject<cardano_witness_set_t, cardano_witness_set_unref> owner(value);
  const WitnessSet                                                copied(owner.release());
  return WitnessSet(copied);
}

void
SubTransaction::setWitnessSet(const WitnessSet& value)
{
  const WitnessSet copy(value);
  checkCardanoError(cardano_sub_transaction_set_witness_set(checked(), copy.checked()), "SubTransaction setWitnessSet");
  cardano_sub_transaction_clear_cbor_cache(checked());
}

std::optional<AuxiliaryData>
SubTransaction::auxiliaryData() const
{
  cardano_auxiliary_data_t* value = cardano_sub_transaction_get_auxiliary_data(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_auxiliary_data_t, cardano_auxiliary_data_unref> owner(value);
  const AuxiliaryData                                                   copied(*owner.release());
  return AuxiliaryData(copied);
}

void
SubTransaction::setAuxiliaryData(const std::optional<AuxiliaryData>& value)
{
  if (value)
  {
    const AuxiliaryData copy(*value);
    checkCardanoError(cardano_sub_transaction_set_auxiliary_data(checked(), copy.checked()), "SubTransaction setAuxiliaryData");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_set_auxiliary_data(checked(), nullptr), "SubTransaction setAuxiliaryData");
  }

  cardano_sub_transaction_clear_cbor_cache(checked());
}

Blake2bHash
SubTransaction::id() const
{
  cardano_blake2b_hash_t* value = cardano_sub_transaction_get_id(checked());
  if (!value)
  {
    throw std::logic_error("SubTransaction identifier is unavailable");
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied(owner.release());
  return Blake2bHash(copied);
}

void
SubTransaction::applyVkeyWitnesses(const VkeyWitnessSet& witnesses)
{
  const VkeyWitnessSet copy(witnesses);
  checkCardanoError(cardano_sub_transaction_apply_vkey_witnesses(checked(), copy.checked()), "SubTransaction applyVkeyWitnesses");
  cardano_sub_transaction_clear_cbor_cache(checked());
}

std::vector<std::uint8_t>
SubTransaction::toCbor() const
{
  return encodeCbor(checked(), cardano_sub_transaction_to_cbor);
}

std::string
SubTransaction::toJson() const
{
  return encodeJson(checked(), cardano_sub_transaction_to_cip116_json);
}

void
SubTransaction::swap(SubTransaction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

SubTransaction&
SubTransaction::operator=(const SubTransaction& other)
{
  if (this != &other)
  {
    SubTransaction copy(other);
    swap(copy);
  }
  return *this;
}

SubTransaction&
SubTransaction::operator=(SubTransaction&& other) noexcept
{
  if (this != &other)
  {
    cardano_sub_transaction_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
SubTransaction::operator==(const SubTransaction& other) const
{
  return toCbor() == other.toCbor();
}

bool
SubTransaction::operator!=(const SubTransaction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

SubTransaction::SubTransaction(cardano_sub_transaction_t* value) noexcept
: m_value(value)
{
}

cardano_sub_transaction_t*
SubTransaction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from SubTransaction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SubTransaction& lhs, SubTransaction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
