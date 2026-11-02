/**
 * @file Transaction.cpp
 *
 * @brief Dijkstra sub-transactions and witness application.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

#include <Cometa/Transaction/Transaction.h>

#include "../CardanoInterop.h"

#include <cardano/address/address.h>
#include <cardano/address/reward_address.h>
#include <cardano/auxiliary_data/auxiliary_data.h>
#include <cardano/certs/certificate.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/crypto/blake2b_hash_set.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_body/transaction_body.h>
#include <cardano/voting_procedures/voter.h>
#include <cardano/witness_set/redeemer_tag.h>
#include <cardano/witness_set/vkey_witness_set.h>
#include <cardano/witness_set/witness_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Transaction::Transaction(const TransactionBody& body, const WitnessSet& witnesses, const std::optional<AuxiliaryData>& auxiliaryData)
: m_value(nullptr)
{
  const TransactionBody                                           bodyCopy(body);
  const WitnessSet                                                witnessCopy(witnesses);
  std::optional<AuxiliaryData>                                    auxiliaryCopy(auxiliaryData);
  cardano_transaction_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_transaction_new(bodyCopy.checked(), witnessCopy.checked(), auxiliaryCopy ? auxiliaryCopy->checked() : nullptr, &value);
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> owner(value);

  checkCardanoError(status, "Transaction");
  m_value = owner.release();
}

Transaction::Transaction(const Transaction& other)
: m_value(nullptr)
{
  cardano_transaction_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_transaction_deep_clone(other.checked(), &value);
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> owner(value);

  checkCardanoError(status, "Transaction copy");
  m_value = owner.release();
}

Transaction::Transaction(Transaction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Transaction::~Transaction()
{
  cardano_transaction_unref(&m_value);
}

Transaction
Transaction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return Transaction(decodeCbor<cardano_transaction_t, cardano_transaction_unref>(cbor, cardano_transaction_from_cbor));
}

Transaction
Transaction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

TransactionBody
Transaction::body() const
{
  cardano_transaction_body_t* value = cardano_transaction_get_body(checked());
  if (!value)
  {
    throw std::logic_error("Transaction has no body");
  }

  CardanoObject<cardano_transaction_body_t, cardano_transaction_body_unref> owner(value);
  const TransactionBody                                                     copied(owner.release());
  return TransactionBody(copied);
}

void
Transaction::setBody(const TransactionBody& value)
{
  const TransactionBody copy(value);
  checkCardanoError(cardano_transaction_set_body(checked(), copy.checked()), "Transaction setBody");
  cardano_transaction_clear_cbor_cache(checked());
}

WitnessSet
Transaction::witnessSet() const
{
  cardano_witness_set_t* value = cardano_transaction_get_witness_set(checked());
  if (!value)
  {
    throw std::logic_error("Transaction has no witness set");
  }

  CardanoObject<cardano_witness_set_t, cardano_witness_set_unref> owner(value);
  const WitnessSet                                                copied(owner.release());
  return WitnessSet(copied);
}

void
Transaction::setWitnessSet(const WitnessSet& value)
{
  const WitnessSet copy(value);
  checkCardanoError(cardano_transaction_set_witness_set(checked(), copy.checked()), "Transaction setWitnessSet");
  cardano_transaction_clear_cbor_cache(checked());
}

std::optional<AuxiliaryData>
Transaction::auxiliaryData() const
{
  cardano_auxiliary_data_t* value = cardano_transaction_get_auxiliary_data(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_auxiliary_data_t, cardano_auxiliary_data_unref> owner(value);
  const AuxiliaryData                                                   copied(*owner.release());
  return AuxiliaryData(copied);
}

void
Transaction::setAuxiliaryData(const std::optional<AuxiliaryData>& value)
{
  if (value)
  {
    const AuxiliaryData copy(*value);
    checkCardanoError(cardano_transaction_set_auxiliary_data(checked(), copy.checked()), "Transaction setAuxiliaryData");
  }
  else
  {
    checkCardanoError(cardano_transaction_set_auxiliary_data(checked(), nullptr), "Transaction setAuxiliaryData");
  }

  cardano_transaction_clear_cbor_cache(checked());
}

Blake2bHash
Transaction::id() const
{
  cardano_blake2b_hash_t* value = cardano_transaction_get_id(checked());
  if (!value)
  {
    throw std::logic_error("Transaction identifier is unavailable");
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied(owner.release());
  return Blake2bHash(copied);
}

void
Transaction::applyVkeyWitnesses(const VkeyWitnessSet& witnesses)
{
  const VkeyWitnessSet copy(witnesses);
  checkCardanoError(cardano_transaction_apply_vkey_witnesses(checked(), copy.checked()), "Transaction applyVkeyWitnesses");
  cardano_transaction_clear_cbor_cache(checked());
}

bool
Transaction::isValid() const
{
  return cardano_transaction_get_is_valid(checked());
}

void
Transaction::setValid(bool value)
{
  checkCardanoError(cardano_transaction_set_is_valid(checked(), value), "Transaction setValid");
  cardano_transaction_clear_cbor_cache(checked());
}

bool
Transaction::hasScriptData() const
{
  return cardano_transaction_has_script_data(checked());
}

Blake2bHashSet
Transaction::uniqueSigners(const UtxoList& resolvedInputs) const
{
  cardano_utxo_list_t*                                        list       = nullptr;
  const cardano_error_t                                       listStatus = cardano_utxo_list_new(&list);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> listOwner(list);
  checkCardanoError(listStatus, "Transaction uniqueSigners");

  for (const Utxo& input: resolvedInputs)
  {
    const Utxo copy(input);
    checkCardanoError(cardano_utxo_list_add(list, copy.checked()), "Transaction uniqueSigners");
  }

  cardano_blake2b_hash_set_t*                                               hashes = nullptr;
  const cardano_error_t                                                     status = cardano_transaction_get_unique_signers(checked(), list, &hashes);
  CardanoObject<cardano_blake2b_hash_set_t, cardano_blake2b_hash_set_unref> owner(hashes);
  checkCardanoError(status, "Transaction uniqueSigners");

  const Blake2bHashSet copied(owner.release());
  return Blake2bHashSet(copied);
}

std::optional<std::uint64_t>
Transaction::findInputIndex(const TransactionInput& input) const
{
  const Blake2bHash     id     = input.id();
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_input_index(checked(), id.checked(), input.index(), &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findInputIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findReferenceInputIndex(const TransactionInput& input) const
{
  const Blake2bHash     id     = input.id();
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_reference_input_index(checked(), id.checked(), input.index(), &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findReferenceInputIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findOutputIndex(const Address& address, std::uint64_t minLovelace) const
{
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_output_index(checked(), address.checked(), minLovelace, &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findOutputIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findMintPolicyIndex(const Blake2bHash& policyId) const
{
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_mint_policy_index(checked(), policyId.checked(), &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findMintPolicyIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findWithdrawalIndex(const RewardAddress& address) const
{
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_withdrawal_index(checked(), address.checked(), &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findWithdrawalIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findCertificateIndex(const Certificate& certificate) const
{
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_certificate_index(checked(), certificate.checked(), &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findCertificateIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findVoteIndex(const Voter& voter) const
{
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_vote_index(checked(), voter.checked(), &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findVoteIndex");
  return index;
}

std::optional<std::uint64_t>
Transaction::findRedeemerIndex(RedeemerTag tag, std::uint64_t purposeIndex) const
{
  std::uint64_t         index  = 0;
  const cardano_error_t status = cardano_transaction_find_redeemer_index(checked(), static_cast<cardano_redeemer_tag_t>(tag), purposeIndex, &index);
  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "Transaction findRedeemerIndex");
  return index;
}

std::vector<std::uint8_t>
Transaction::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_to_cbor);
}

std::string
Transaction::toJson() const
{
  return encodeJson(checked(), cardano_transaction_to_cip116_json);
}

void
Transaction::swap(Transaction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Transaction&
Transaction::operator=(const Transaction& other)
{
  if (this != &other)
  {
    Transaction copy(other);
    swap(copy);
  }
  return *this;
}

Transaction&
Transaction::operator=(Transaction&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Transaction::operator==(const Transaction& other) const
{
  return toCbor() == other.toCbor();
}

bool
Transaction::operator!=(const Transaction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Transaction::Transaction(cardano_transaction_t* value) noexcept
: m_value(value)
{
}

cardano_transaction_t*
Transaction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Transaction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Transaction& lhs, Transaction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
