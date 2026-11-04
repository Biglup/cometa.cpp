/**
 * @file SubTransactionBody.cpp
 *
 * @brief Dijkstra sub-transaction body values.
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

#include <Cometa/TransactionBody/SubTransactionBody.h>

#include "../CardanoInterop.h"

#include <cardano/assets/multi_asset.h>
#include <cardano/certs/certificate_set.h>
#include <cardano/common/guard_set.h>
#include <cardano/common/network_id.h>
#include <cardano/common/withdrawal_map.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/proposal_procedures/proposal_procedure_set.h>
#include <cardano/transaction_body/account_balance_intervals_map.h>
#include <cardano/transaction_body/direct_deposit_map.h>
#include <cardano/transaction_body/required_guards_map.h>
#include <cardano/transaction_body/sub_transaction_body.h>
#include <cardano/transaction_body/transaction_input_set.h>
#include <cardano/transaction_body/transaction_output_list.h>
#include <cardano/voting_procedures/voting_procedures.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
SubTransactionBody::SubTransactionBody(const TransactionInputSet& inputs, const TransactionOutputList& outputs, std::optional<std::uint64_t> invalidAfter)
: m_value(nullptr)
{
  const TransactionInputSet                                                         inputCopy(inputs);
  const TransactionOutputList                                                       outputCopy(outputs);
  cardano_sub_transaction_body_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_sub_transaction_body_new(checkedNativeHandle(inputCopy), checkedNativeHandle(outputCopy), invalidAfter ? &*invalidAfter : nullptr, &value);
  CardanoObject<cardano_sub_transaction_body_t, cardano_sub_transaction_body_unref> owner(value);

  checkCardanoError(status, "SubTransactionBody");
  m_value = owner.release();
}

SubTransactionBody::SubTransactionBody(const SubTransactionBody& other)
: m_value(nullptr)
{
  cardano_sub_transaction_body_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_sub_transaction_body_deep_clone(other.checked(), &value);
  CardanoObject<cardano_sub_transaction_body_t, cardano_sub_transaction_body_unref> owner(value);

  checkCardanoError(status, "SubTransactionBody copy");
  m_value = owner.release();
}

SubTransactionBody::SubTransactionBody(SubTransactionBody&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

SubTransactionBody::~SubTransactionBody()
{
  cardano_sub_transaction_body_unref(&m_value);
}

SubTransactionBody
SubTransactionBody::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return SubTransactionBody(decodeCbor<cardano_sub_transaction_body_t, cardano_sub_transaction_body_unref>(cbor, cardano_sub_transaction_body_from_cbor));
}

SubTransactionBody
SubTransactionBody::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

TransactionInputSet
SubTransactionBody::inputs() const
{
  cardano_transaction_input_set_t* value = cardano_sub_transaction_body_get_inputs(checked());
  if (!value)
  {
    throw std::logic_error("SubTransactionBody has no inputs");
  }

  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);
  const TransactionInputSet                                                           copied = TransactionInputSet::adopt(owner.release());
  return TransactionInputSet(copied);
}

void
SubTransactionBody::setInputs(const TransactionInputSet& value)
{
  const TransactionInputSet copy(value);
  checkCardanoError(cardano_sub_transaction_body_set_inputs(checked(), checkedNativeHandle(copy)), "SubTransactionBody setInputs");

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

TransactionOutputList
SubTransactionBody::outputs() const
{
  cardano_transaction_output_list_t* value = cardano_sub_transaction_body_get_outputs(checked());
  if (!value)
  {
    throw std::logic_error("SubTransactionBody has no outputs");
  }

  CardanoObject<cardano_transaction_output_list_t, cardano_transaction_output_list_unref> owner(value);
  const TransactionOutputList                                                             copied = TransactionOutputList::adopt(owner.release());
  return TransactionOutputList(copied);
}

void
SubTransactionBody::setOutputs(const TransactionOutputList& value)
{
  const TransactionOutputList copy(value);
  checkCardanoError(cardano_sub_transaction_body_set_outputs(checked(), checkedNativeHandle(copy)), "SubTransactionBody setOutputs");

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<CertificateSet>
SubTransactionBody::certificates() const
{
  cardano_certificate_set_t* value = cardano_sub_transaction_body_get_certificates(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_certificate_set_t, cardano_certificate_set_unref> owner(value);
  const CertificateSet                                                    copied = CertificateSet::adopt(owner.release());
  return CertificateSet(copied);
}

void
SubTransactionBody::setCertificates(const std::optional<CertificateSet>& value)
{
  if (value)
  {
    const CertificateSet copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_certificates(checked(), checkedNativeHandle(copy)), "SubTransactionBody setCertificates");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_certificates(checked(), nullptr), "SubTransactionBody setCertificates");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<WithdrawalMap>
SubTransactionBody::withdrawals() const
{
  cardano_withdrawal_map_t* value = cardano_sub_transaction_body_get_withdrawals(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_withdrawal_map_t, cardano_withdrawal_map_unref> owner(value);
  const WithdrawalMap                                                   copied = WithdrawalMap::adopt(owner.release());
  return WithdrawalMap(copied);
}

void
SubTransactionBody::setWithdrawals(const std::optional<WithdrawalMap>& value)
{
  if (value)
  {
    const WithdrawalMap copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_withdrawals(checked(), checkedNativeHandle(copy)), "SubTransactionBody setWithdrawals");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_withdrawals(checked(), nullptr), "SubTransactionBody setWithdrawals");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<Blake2bHash>
SubTransactionBody::auxDataHash() const
{
  cardano_blake2b_hash_t* value = cardano_sub_transaction_body_get_aux_data_hash(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied = Blake2bHash::adopt(owner.release());
  return Blake2bHash(copied);
}

void
SubTransactionBody::setAuxDataHash(const std::optional<Blake2bHash>& value)
{
  if (value)
  {
    const Blake2bHash copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_aux_data_hash(checked(), checkedNativeHandle(copy)), "SubTransactionBody setAuxDataHash");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_aux_data_hash(checked(), nullptr), "SubTransactionBody setAuxDataHash");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<MultiAsset>
SubTransactionBody::mint() const
{
  cardano_multi_asset_t* value = cardano_sub_transaction_body_get_mint(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(value);
  const MultiAsset                                                copied = MultiAsset::adopt(owner.release());
  return MultiAsset(copied);
}

void
SubTransactionBody::setMint(const std::optional<MultiAsset>& value)
{
  if (value)
  {
    const MultiAsset copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_mint(checked(), checkedNativeHandle(copy)), "SubTransactionBody setMint");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_mint(checked(), nullptr), "SubTransactionBody setMint");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<Blake2bHash>
SubTransactionBody::scriptDataHash() const
{
  cardano_blake2b_hash_t* value = cardano_sub_transaction_body_get_script_data_hash(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied = Blake2bHash::adopt(owner.release());
  return Blake2bHash(copied);
}

void
SubTransactionBody::setScriptDataHash(const std::optional<Blake2bHash>& value)
{
  if (value)
  {
    const Blake2bHash copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_script_data_hash(checked(), checkedNativeHandle(copy)), "SubTransactionBody setScriptDataHash");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_script_data_hash(checked(), nullptr), "SubTransactionBody setScriptDataHash");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<GuardSet>
SubTransactionBody::guards() const
{
  cardano_guard_set_t* value = cardano_sub_transaction_body_get_guards(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_guard_set_t, cardano_guard_set_unref> owner(value);
  const GuardSet                                              copied = GuardSet::adopt(owner.release());
  return GuardSet(copied);
}

void
SubTransactionBody::setGuards(const std::optional<GuardSet>& value)
{
  if (value)
  {
    const GuardSet copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_guards(checked(), checkedNativeHandle(copy)), "SubTransactionBody setGuards");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_guards(checked(), nullptr), "SubTransactionBody setGuards");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<TransactionInputSet>
SubTransactionBody::referenceInputs() const
{
  cardano_transaction_input_set_t* value = cardano_sub_transaction_body_get_reference_inputs(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);
  const TransactionInputSet                                                           copied = TransactionInputSet::adopt(owner.release());
  return TransactionInputSet(copied);
}

void
SubTransactionBody::setReferenceInputs(const std::optional<TransactionInputSet>& value)
{
  if (value)
  {
    const TransactionInputSet copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_reference_inputs(checked(), checkedNativeHandle(copy)), "SubTransactionBody setReferenceInputs");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_reference_inputs(checked(), nullptr), "SubTransactionBody setReferenceInputs");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<VotingProcedures>
SubTransactionBody::votingProcedures() const
{
  cardano_voting_procedures_t* value = cardano_sub_transaction_body_get_voting_procedures(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_voting_procedures_t, cardano_voting_procedures_unref> owner(value);
  const VotingProcedures                                                      copied = VotingProcedures::adopt(owner.release());
  return VotingProcedures(copied);
}

void
SubTransactionBody::setVotingProcedures(const std::optional<VotingProcedures>& value)
{
  if (value)
  {
    const VotingProcedures copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_voting_procedures(checked(), checkedNativeHandle(copy)), "SubTransactionBody setVotingProcedures");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_voting_procedures(checked(), nullptr), "SubTransactionBody setVotingProcedures");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<ProposalProcedureSet>
SubTransactionBody::proposalProcedures() const
{
  cardano_proposal_procedure_set_t* value = cardano_sub_transaction_body_get_proposal_procedures(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_proposal_procedure_set_t, cardano_proposal_procedure_set_unref> owner(value);
  const ProposalProcedureSet                                                            copied = ProposalProcedureSet::adopt(owner.release());
  return ProposalProcedureSet(copied);
}

void
SubTransactionBody::setProposalProcedures(const std::optional<ProposalProcedureSet>& value)
{
  if (value)
  {
    const ProposalProcedureSet copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_proposal_procedures(checked(), checkedNativeHandle(copy)), "SubTransactionBody setProposalProcedures");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_proposal_procedures(checked(), nullptr), "SubTransactionBody setProposalProcedures");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<RequiredGuardsMap>
SubTransactionBody::requiredTopLevelGuards() const
{
  cardano_required_guards_map_t* value = cardano_sub_transaction_body_get_required_top_level_guards(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_required_guards_map_t, cardano_required_guards_map_unref> owner(value);
  const RequiredGuardsMap                                                         copied = RequiredGuardsMap::adopt(owner.release());
  return RequiredGuardsMap(copied);
}

void
SubTransactionBody::setRequiredTopLevelGuards(const std::optional<RequiredGuardsMap>& value)
{
  if (value)
  {
    const RequiredGuardsMap copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_required_top_level_guards(checked(), checkedNativeHandle(copy)), "SubTransactionBody setRequiredTopLevelGuards");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_required_top_level_guards(checked(), nullptr), "SubTransactionBody setRequiredTopLevelGuards");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<DirectDepositMap>
SubTransactionBody::directDeposits() const
{
  cardano_direct_deposit_map_t* value = cardano_sub_transaction_body_get_direct_deposits(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_direct_deposit_map_t, cardano_direct_deposit_map_unref> owner(value);
  const DirectDepositMap                                                        copied = DirectDepositMap::adopt(owner.release());
  return DirectDepositMap(copied);
}

void
SubTransactionBody::setDirectDeposits(const std::optional<DirectDepositMap>& value)
{
  if (value)
  {
    const DirectDepositMap copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_direct_deposits(checked(), checkedNativeHandle(copy)), "SubTransactionBody setDirectDeposits");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_direct_deposits(checked(), nullptr), "SubTransactionBody setDirectDeposits");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<AccountBalanceIntervalsMap>
SubTransactionBody::accountBalanceIntervals() const
{
  cardano_account_balance_intervals_map_t* value = cardano_sub_transaction_body_get_account_balance_intervals(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_account_balance_intervals_map_t, cardano_account_balance_intervals_map_unref> owner(value);
  const AccountBalanceIntervalsMap                                                                    copied = AccountBalanceIntervalsMap::adopt(owner.release());
  return AccountBalanceIntervalsMap(copied);
}

void
SubTransactionBody::setAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value)
{
  if (value)
  {
    const AccountBalanceIntervalsMap copy(*value);
    checkCardanoError(cardano_sub_transaction_body_set_account_balance_intervals(checked(), checkedNativeHandle(copy)), "SubTransactionBody setAccountBalanceIntervals");
  }
  else
  {
    checkCardanoError(cardano_sub_transaction_body_set_account_balance_intervals(checked(), nullptr), "SubTransactionBody setAccountBalanceIntervals");
  }

  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
SubTransactionBody::invalidAfter() const
{
  const std::uint64_t* value = cardano_sub_transaction_body_get_invalid_after(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
SubTransactionBody::setInvalidAfter(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_sub_transaction_body_set_invalid_after(checked(), value ? &*value : nullptr), "SubTransactionBody setInvalidAfter");
  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
SubTransactionBody::invalidBefore() const
{
  const std::uint64_t* value = cardano_sub_transaction_body_get_invalid_before(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
SubTransactionBody::setInvalidBefore(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_sub_transaction_body_set_invalid_before(checked(), value ? &*value : nullptr), "SubTransactionBody setInvalidBefore");
  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<NetworkId>
SubTransactionBody::networkId() const
{
  const cardano_network_id_t* value = cardano_sub_transaction_body_get_network_id(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return static_cast<NetworkId>(*value);
}

void
SubTransactionBody::setNetworkId(std::optional<NetworkId> value)
{
  const std::optional<cardano_network_id_t> native = value ? std::optional<cardano_network_id_t>(static_cast<cardano_network_id_t>(*value)) : std::nullopt;
  checkCardanoError(cardano_sub_transaction_body_set_network_id(checked(), native ? &*native : nullptr), "SubTransactionBody setNetworkId");
  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
SubTransactionBody::treasuryValue() const
{
  const std::uint64_t* value = cardano_sub_transaction_body_get_treasury_value(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
SubTransactionBody::setTreasuryValue(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_sub_transaction_body_set_treasury_value(checked(), value ? &*value : nullptr), "SubTransactionBody setTreasuryValue");
  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
SubTransactionBody::donation() const
{
  const std::uint64_t* value = cardano_sub_transaction_body_get_donation(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
SubTransactionBody::setDonation(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_sub_transaction_body_set_donation(checked(), value ? &*value : nullptr), "SubTransactionBody setDonation");
  cardano_sub_transaction_body_clear_cbor_cache(checked());
}

Blake2bHash
SubTransactionBody::hash() const
{
  cardano_blake2b_hash_t* value = cardano_sub_transaction_body_get_hash(checked());
  if (!value)
  {
    throw std::logic_error("SubTransactionBody hash is unavailable");
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied = Blake2bHash::adopt(owner.release());
  return Blake2bHash(copied);
}

std::vector<std::uint8_t>
SubTransactionBody::toCbor() const
{
  return encodeCbor(checked(), cardano_sub_transaction_body_to_cbor);
}

std::string
SubTransactionBody::toJson() const
{
  return encodeJson(checked(), cardano_sub_transaction_body_to_cip116_json);
}

void
SubTransactionBody::swap(SubTransactionBody& other) noexcept
{
  std::swap(m_value, other.m_value);
}

SubTransactionBody&
SubTransactionBody::operator=(const SubTransactionBody& other)
{
  if (this != &other)
  {
    SubTransactionBody copy(other);
    swap(copy);
  }
  return *this;
}

SubTransactionBody&
SubTransactionBody::operator=(SubTransactionBody&& other) noexcept
{
  if (this != &other)
  {
    cardano_sub_transaction_body_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
SubTransactionBody::operator==(const SubTransactionBody& other) const
{
  return toCbor() == other.toCbor();
}

bool
SubTransactionBody::operator!=(const SubTransactionBody& other) const
{
  return !(*this == other);
}

SubTransactionBody
SubTransactionBody::adopt(cardano_sub_transaction_body_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null SubTransactionBody");
  }

  return SubTransactionBody(value);
}

cardano_sub_transaction_body_t*
SubTransactionBody::nativeHandle() noexcept
{
  return m_value;
}

const cardano_sub_transaction_body_t*
SubTransactionBody::nativeHandle() const noexcept
{
  return m_value;
}

cardano_sub_transaction_body_t*
SubTransactionBody::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

SubTransactionBody::SubTransactionBody(cardano_sub_transaction_body_t* value) noexcept
: m_value(value)
{
}

cardano_sub_transaction_body_t*
SubTransactionBody::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from SubTransactionBody");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SubTransactionBody& lhs, SubTransactionBody& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
