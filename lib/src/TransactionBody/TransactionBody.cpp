/**
 * @file TransactionBody.cpp
 *
 * @brief Top-level transaction body values.
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

#include <Cometa/TransactionBody/TransactionBody.h>

#include "../CardanoInterop.h"

#include <cardano/assets/multi_asset.h>
#include <cardano/certs/certificate_set.h>
#include <cardano/common/guard_set.h>
#include <cardano/common/network_id.h>
#include <cardano/common/withdrawal_map.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/crypto/blake2b_hash_set.h>
#include <cardano/proposal_procedures/proposal_procedure_set.h>
#include <cardano/protocol_params/update.h>
#include <cardano/transaction_body/account_balance_intervals_map.h>
#include <cardano/transaction_body/direct_deposit_map.h>
#include <cardano/transaction_body/required_guards_map.h>
#include <cardano/transaction_body/sub_transaction_set.h>
#include <cardano/transaction_body/transaction_body.h>
#include <cardano/transaction_body/transaction_input_set.h>
#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/transaction_output_list.h>
#include <cardano/voting_procedures/voting_procedures.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TransactionBody::TransactionBody(const TransactionInputSet& inputs, const TransactionOutputList& outputs, std::uint64_t fee, std::optional<std::uint64_t> invalidAfter)
: m_value(nullptr)
{
  const TransactionInputSet                                                 inputCopy(inputs);
  const TransactionOutputList                                               outputCopy(outputs);
  cardano_transaction_body_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_transaction_body_new(checkedNativeHandle(inputCopy), checkedNativeHandle(outputCopy), fee, invalidAfter ? &*invalidAfter : nullptr, &value);
  CardanoObject<cardano_transaction_body_t, cardano_transaction_body_unref> owner(value);

  checkCardanoError(status, "TransactionBody");
  m_value = owner.release();
}

TransactionBody::TransactionBody(const TransactionBody& other)
: m_value(nullptr)
{
  cardano_transaction_body_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_transaction_body_deep_clone(other.checked(), &value);
  CardanoObject<cardano_transaction_body_t, cardano_transaction_body_unref> owner(value);

  checkCardanoError(status, "TransactionBody copy");
  m_value = owner.release();
}

TransactionBody::TransactionBody(TransactionBody&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

TransactionBody::~TransactionBody()
{
  cardano_transaction_body_unref(&m_value);
}

TransactionBody
TransactionBody::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return TransactionBody(decodeCbor<cardano_transaction_body_t, cardano_transaction_body_unref>(cbor, cardano_transaction_body_from_cbor));
}

TransactionBody
TransactionBody::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

TransactionInputSet
TransactionBody::inputs() const
{
  cardano_transaction_input_set_t* value = cardano_transaction_body_get_inputs(checked());
  if (!value)
  {
    throw std::logic_error("TransactionBody has no inputs");
  }

  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);
  const TransactionInputSet                                                           copied = TransactionInputSet::adopt(owner.release());
  return TransactionInputSet(copied);
}

void
TransactionBody::setInputs(const TransactionInputSet& value)
{
  const TransactionInputSet copy(value);
  checkCardanoError(cardano_transaction_body_set_inputs(checked(), checkedNativeHandle(copy)), "TransactionBody setInputs");

  cardano_transaction_body_clear_cbor_cache(checked());
}

TransactionOutputList
TransactionBody::outputs() const
{
  cardano_transaction_output_list_t* value = cardano_transaction_body_get_outputs(checked());
  if (!value)
  {
    throw std::logic_error("TransactionBody has no outputs");
  }

  CardanoObject<cardano_transaction_output_list_t, cardano_transaction_output_list_unref> owner(value);
  const TransactionOutputList                                                             copied = TransactionOutputList::adopt(owner.release());
  return TransactionOutputList(copied);
}

void
TransactionBody::setOutputs(const TransactionOutputList& value)
{
  const TransactionOutputList copy(value);
  checkCardanoError(cardano_transaction_body_set_outputs(checked(), checkedNativeHandle(copy)), "TransactionBody setOutputs");

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<CertificateSet>
TransactionBody::certificates() const
{
  cardano_certificate_set_t* value = cardano_transaction_body_get_certificates(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_certificate_set_t, cardano_certificate_set_unref> owner(value);
  const CertificateSet                                                    copied = CertificateSet::adopt(owner.release());
  return CertificateSet(copied);
}

void
TransactionBody::setCertificates(const std::optional<CertificateSet>& value)
{
  if (value)
  {
    const CertificateSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_certificates(checked(), checkedNativeHandle(copy)), "TransactionBody setCertificates");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_certificates(checked(), nullptr), "TransactionBody setCertificates");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<WithdrawalMap>
TransactionBody::withdrawals() const
{
  cardano_withdrawal_map_t* value = cardano_transaction_body_get_withdrawals(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_withdrawal_map_t, cardano_withdrawal_map_unref> owner(value);
  const WithdrawalMap                                                   copied = WithdrawalMap::adopt(owner.release());
  return WithdrawalMap(copied);
}

void
TransactionBody::setWithdrawals(const std::optional<WithdrawalMap>& value)
{
  if (value)
  {
    const WithdrawalMap copy(*value);
    checkCardanoError(cardano_transaction_body_set_withdrawals(checked(), checkedNativeHandle(copy)), "TransactionBody setWithdrawals");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_withdrawals(checked(), nullptr), "TransactionBody setWithdrawals");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<Blake2bHash>
TransactionBody::auxDataHash() const
{
  cardano_blake2b_hash_t* value = cardano_transaction_body_get_aux_data_hash(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied = Blake2bHash::adopt(owner.release());
  return Blake2bHash(copied);
}

void
TransactionBody::setAuxDataHash(const std::optional<Blake2bHash>& value)
{
  if (value)
  {
    const Blake2bHash copy(*value);
    checkCardanoError(cardano_transaction_body_set_aux_data_hash(checked(), checkedNativeHandle(copy)), "TransactionBody setAuxDataHash");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_aux_data_hash(checked(), nullptr), "TransactionBody setAuxDataHash");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<MultiAsset>
TransactionBody::mint() const
{
  cardano_multi_asset_t* value = cardano_transaction_body_get_mint(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(value);
  const MultiAsset                                                copied = MultiAsset::adopt(owner.release());
  return MultiAsset(copied);
}

void
TransactionBody::setMint(const std::optional<MultiAsset>& value)
{
  if (value)
  {
    const MultiAsset copy(*value);
    checkCardanoError(cardano_transaction_body_set_mint(checked(), checkedNativeHandle(copy)), "TransactionBody setMint");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_mint(checked(), nullptr), "TransactionBody setMint");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<Blake2bHash>
TransactionBody::scriptDataHash() const
{
  cardano_blake2b_hash_t* value = cardano_transaction_body_get_script_data_hash(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied = Blake2bHash::adopt(owner.release());
  return Blake2bHash(copied);
}

void
TransactionBody::setScriptDataHash(const std::optional<Blake2bHash>& value)
{
  if (value)
  {
    const Blake2bHash copy(*value);
    checkCardanoError(cardano_transaction_body_set_script_data_hash(checked(), checkedNativeHandle(copy)), "TransactionBody setScriptDataHash");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_script_data_hash(checked(), nullptr), "TransactionBody setScriptDataHash");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<GuardSet>
TransactionBody::guards() const
{
  cardano_guard_set_t* value = cardano_transaction_body_get_guards(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_guard_set_t, cardano_guard_set_unref> owner(value);
  const GuardSet                                              copied = GuardSet::adopt(owner.release());
  return GuardSet(copied);
}

void
TransactionBody::setGuards(const std::optional<GuardSet>& value)
{
  if (value)
  {
    const GuardSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_guards(checked(), checkedNativeHandle(copy)), "TransactionBody setGuards");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_guards(checked(), nullptr), "TransactionBody setGuards");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<TransactionInputSet>
TransactionBody::referenceInputs() const
{
  cardano_transaction_input_set_t* value = cardano_transaction_body_get_reference_inputs(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);
  const TransactionInputSet                                                           copied = TransactionInputSet::adopt(owner.release());
  return TransactionInputSet(copied);
}

void
TransactionBody::setReferenceInputs(const std::optional<TransactionInputSet>& value)
{
  if (value)
  {
    const TransactionInputSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_reference_inputs(checked(), checkedNativeHandle(copy)), "TransactionBody setReferenceInputs");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_reference_inputs(checked(), nullptr), "TransactionBody setReferenceInputs");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<VotingProcedures>
TransactionBody::votingProcedures() const
{
  cardano_voting_procedures_t* value = cardano_transaction_body_get_voting_procedures(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_voting_procedures_t, cardano_voting_procedures_unref> owner(value);
  const VotingProcedures                                                      copied = VotingProcedures::adopt(owner.release());
  return VotingProcedures(copied);
}

void
TransactionBody::setVotingProcedures(const std::optional<VotingProcedures>& value)
{
  if (value)
  {
    const VotingProcedures copy(*value);
    checkCardanoError(cardano_transaction_body_set_voting_procedures(checked(), checkedNativeHandle(copy)), "TransactionBody setVotingProcedures");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_voting_procedures(checked(), nullptr), "TransactionBody setVotingProcedures");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<ProposalProcedureSet>
TransactionBody::proposalProcedures() const
{
  cardano_proposal_procedure_set_t* value = cardano_transaction_body_get_proposal_procedures(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_proposal_procedure_set_t, cardano_proposal_procedure_set_unref> owner(value);
  const ProposalProcedureSet                                                            copied = ProposalProcedureSet::adopt(owner.release());
  return ProposalProcedureSet(copied);
}

void
TransactionBody::setProposalProcedures(const std::optional<ProposalProcedureSet>& value)
{
  if (value)
  {
    const ProposalProcedureSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_proposal_procedure(checked(), checkedNativeHandle(copy)), "TransactionBody setProposalProcedures");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_proposal_procedure(checked(), nullptr), "TransactionBody setProposalProcedures");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<RequiredGuardsMap>
TransactionBody::requiredTopLevelGuards() const
{
  cardano_required_guards_map_t* value = cardano_transaction_body_get_required_top_level_guards(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_required_guards_map_t, cardano_required_guards_map_unref> owner(value);
  const RequiredGuardsMap                                                         copied = RequiredGuardsMap::adopt(owner.release());
  return RequiredGuardsMap(copied);
}

void
TransactionBody::setRequiredTopLevelGuards(const std::optional<RequiredGuardsMap>& value)
{
  if (value)
  {
    const RequiredGuardsMap copy(*value);
    checkCardanoError(cardano_transaction_body_set_required_top_level_guards(checked(), checkedNativeHandle(copy)), "TransactionBody setRequiredTopLevelGuards");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_required_top_level_guards(checked(), nullptr), "TransactionBody setRequiredTopLevelGuards");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<DirectDepositMap>
TransactionBody::directDeposits() const
{
  cardano_direct_deposit_map_t* value = cardano_transaction_body_get_direct_deposits(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_direct_deposit_map_t, cardano_direct_deposit_map_unref> owner(value);
  const DirectDepositMap                                                        copied = DirectDepositMap::adopt(owner.release());
  return DirectDepositMap(copied);
}

void
TransactionBody::setDirectDeposits(const std::optional<DirectDepositMap>& value)
{
  if (value)
  {
    const DirectDepositMap copy(*value);
    checkCardanoError(cardano_transaction_body_set_direct_deposits(checked(), checkedNativeHandle(copy)), "TransactionBody setDirectDeposits");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_direct_deposits(checked(), nullptr), "TransactionBody setDirectDeposits");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<AccountBalanceIntervalsMap>
TransactionBody::accountBalanceIntervals() const
{
  cardano_account_balance_intervals_map_t* value = cardano_transaction_body_get_account_balance_intervals(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_account_balance_intervals_map_t, cardano_account_balance_intervals_map_unref> owner(value);
  const AccountBalanceIntervalsMap                                                                    copied = AccountBalanceIntervalsMap::adopt(owner.release());
  return AccountBalanceIntervalsMap(copied);
}

void
TransactionBody::setAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value)
{
  if (value)
  {
    const AccountBalanceIntervalsMap copy(*value);
    checkCardanoError(cardano_transaction_body_set_account_balance_intervals(checked(), checkedNativeHandle(copy)), "TransactionBody setAccountBalanceIntervals");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_account_balance_intervals(checked(), nullptr), "TransactionBody setAccountBalanceIntervals");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
TransactionBody::invalidAfter() const
{
  const std::uint64_t* value = cardano_transaction_body_get_invalid_after(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
TransactionBody::setInvalidAfter(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_transaction_body_set_invalid_after(checked(), value ? &*value : nullptr), "TransactionBody setInvalidAfter");
  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
TransactionBody::invalidBefore() const
{
  const std::uint64_t* value = cardano_transaction_body_get_invalid_before(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
TransactionBody::setInvalidBefore(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_transaction_body_set_invalid_before(checked(), value ? &*value : nullptr), "TransactionBody setInvalidBefore");
  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<NetworkId>
TransactionBody::networkId() const
{
  const cardano_network_id_t* value = cardano_transaction_body_get_network_id(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return static_cast<NetworkId>(*value);
}

void
TransactionBody::setNetworkId(std::optional<NetworkId> value)
{
  const std::optional<cardano_network_id_t> native = value ? std::optional<cardano_network_id_t>(static_cast<cardano_network_id_t>(*value)) : std::nullopt;
  checkCardanoError(cardano_transaction_body_set_network_id(checked(), native ? &*native : nullptr), "TransactionBody setNetworkId");
  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
TransactionBody::treasuryValue() const
{
  const std::uint64_t* value = cardano_transaction_body_get_treasury_value(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
TransactionBody::setTreasuryValue(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_transaction_body_set_treasury_value(checked(), value ? &*value : nullptr), "TransactionBody setTreasuryValue");
  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
TransactionBody::donation() const
{
  const std::uint64_t* value = cardano_transaction_body_get_donation(checked());
  if (!value)
  {
    return std::nullopt;
  }

  return *value;
}

void
TransactionBody::setDonation(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_transaction_body_set_donation(checked(), value ? &*value : nullptr), "TransactionBody setDonation");
  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<Update>
TransactionBody::update() const
{
  cardano_update_t* value = cardano_transaction_body_get_update(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_update_t, cardano_update_unref> owner(value);
  const Update                                          copied = Update::adopt(owner.release());
  return Update(copied);
}

void
TransactionBody::setUpdate(const std::optional<Update>& value)
{
  if (value)
  {
    const Update copy(*value);
    checkCardanoError(cardano_transaction_body_set_update(checked(), checkedNativeHandle(copy)), "TransactionBody setUpdate");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_update(checked(), nullptr), "TransactionBody setUpdate");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<TransactionInputSet>
TransactionBody::collateral() const
{
  cardano_transaction_input_set_t* value = cardano_transaction_body_get_collateral(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(value);
  const TransactionInputSet                                                           copied = TransactionInputSet::adopt(owner.release());
  return TransactionInputSet(copied);
}

void
TransactionBody::setCollateral(const std::optional<TransactionInputSet>& value)
{
  if (value)
  {
    const TransactionInputSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_collateral(checked(), checkedNativeHandle(copy)), "TransactionBody setCollateral");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_collateral(checked(), nullptr), "TransactionBody setCollateral");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<Blake2bHashSet>
TransactionBody::requiredSigners() const
{
  cardano_blake2b_hash_set_t* value = cardano_transaction_body_get_required_signers(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_blake2b_hash_set_t, cardano_blake2b_hash_set_unref> owner(value);
  const Blake2bHashSet                                                      copied = Blake2bHashSet::adopt(owner.release());
  return Blake2bHashSet(copied);
}

void
TransactionBody::setRequiredSigners(const std::optional<Blake2bHashSet>& value)
{
  if (value)
  {
    const Blake2bHashSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_required_signers(checked(), checkedNativeHandle(copy)), "TransactionBody setRequiredSigners");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_required_signers(checked(), nullptr), "TransactionBody setRequiredSigners");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<TransactionOutput>
TransactionBody::collateralReturn() const
{
  cardano_transaction_output_t* value = cardano_transaction_body_get_collateral_return(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> owner(value);
  const TransactionOutput                                                       copied = TransactionOutput::adopt(owner.release());
  return TransactionOutput(copied);
}

void
TransactionBody::setCollateralReturn(const std::optional<TransactionOutput>& value)
{
  if (value)
  {
    const TransactionOutput copy(*value);
    checkCardanoError(cardano_transaction_body_set_collateral_return(checked(), checkedNativeHandle(copy)), "TransactionBody setCollateralReturn");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_collateral_return(checked(), nullptr), "TransactionBody setCollateralReturn");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<SubTransactionSet>
TransactionBody::subTransactions() const
{
  cardano_sub_transaction_set_t* value = cardano_transaction_body_get_sub_transactions(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_sub_transaction_set_t, cardano_sub_transaction_set_unref> owner(value);
  const SubTransactionSet                                                         copied = SubTransactionSet::adopt(owner.release());
  return SubTransactionSet(copied);
}

void
TransactionBody::setSubTransactions(const std::optional<SubTransactionSet>& value)
{
  if (value)
  {
    const SubTransactionSet copy(*value);
    checkCardanoError(cardano_transaction_body_set_sub_transactions(checked(), checkedNativeHandle(copy)), "TransactionBody setSubTransactions");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_sub_transactions(checked(), nullptr), "TransactionBody setSubTransactions");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<AccountBalanceIntervalsMap>
TransactionBody::startingAccountBalanceIntervals() const
{
  cardano_account_balance_intervals_map_t* value = cardano_transaction_body_get_starting_account_balance_intervals(checked());
  if (!value)
  {
    return std::nullopt;
  }

  CardanoObject<cardano_account_balance_intervals_map_t, cardano_account_balance_intervals_map_unref> owner(value);
  const AccountBalanceIntervalsMap                                                                    copied = AccountBalanceIntervalsMap::adopt(owner.release());
  return AccountBalanceIntervalsMap(copied);
}

void
TransactionBody::setStartingAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value)
{
  if (value)
  {
    const AccountBalanceIntervalsMap copy(*value);
    checkCardanoError(cardano_transaction_body_set_starting_account_balance_intervals(checked(), checkedNativeHandle(copy)), "TransactionBody setStartingAccountBalanceIntervals");
  }
  else
  {
    checkCardanoError(cardano_transaction_body_set_starting_account_balance_intervals(checked(), nullptr), "TransactionBody setStartingAccountBalanceIntervals");
  }

  cardano_transaction_body_clear_cbor_cache(checked());
}

std::uint64_t
TransactionBody::fee() const
{
  return cardano_transaction_body_get_fee(checked());
}

void
TransactionBody::setFee(std::uint64_t value)
{
  checkCardanoError(cardano_transaction_body_set_fee(checked(), value), "TransactionBody setFee");
  cardano_transaction_body_clear_cbor_cache(checked());
}

std::optional<std::uint64_t>
TransactionBody::totalCollateral() const
{
  const std::uint64_t* value = cardano_transaction_body_get_total_collateral(checked());
  return value ? std::optional<std::uint64_t>(*value) : std::nullopt;
}

void
TransactionBody::setTotalCollateral(std::optional<std::uint64_t> value)
{
  checkCardanoError(cardano_transaction_body_set_total_collateral(checked(), value ? &*value : nullptr), "TransactionBody setTotalCollateral");
  cardano_transaction_body_clear_cbor_cache(checked());
}

bool
TransactionBody::hasTaggedSets() const
{
  return cardano_transaction_body_has_tagged_sets(checked());
}

Blake2bHash
TransactionBody::hash() const
{
  cardano_blake2b_hash_t* value = cardano_transaction_body_get_hash(checked());
  if (!value)
  {
    throw std::logic_error("TransactionBody hash is unavailable");
  }

  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  const Blake2bHash                                                 copied = Blake2bHash::adopt(owner.release());
  return Blake2bHash(copied);
}

std::vector<std::uint8_t>
TransactionBody::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_body_to_cbor);
}

std::string
TransactionBody::toJson() const
{
  return encodeJson(checked(), cardano_transaction_body_to_cip116_json);
}

void
TransactionBody::swap(TransactionBody& other) noexcept
{
  std::swap(m_value, other.m_value);
}

TransactionBody&
TransactionBody::operator=(const TransactionBody& other)
{
  if (this != &other)
  {
    TransactionBody copy(other);
    swap(copy);
  }
  return *this;
}

TransactionBody&
TransactionBody::operator=(TransactionBody&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_body_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
TransactionBody::operator==(const TransactionBody& other) const
{
  return toCbor() == other.toCbor();
}

bool
TransactionBody::operator!=(const TransactionBody& other) const
{
  return !(*this == other);
}

TransactionBody
TransactionBody::adopt(cardano_transaction_body_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null TransactionBody");
  }

  return TransactionBody(value);
}

cardano_transaction_body_t*
TransactionBody::nativeHandle() noexcept
{
  return m_value;
}

const cardano_transaction_body_t*
TransactionBody::nativeHandle() const noexcept
{
  return m_value;
}

cardano_transaction_body_t*
TransactionBody::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

TransactionBody::TransactionBody(cardano_transaction_body_t* value) noexcept
: m_value(value)
{
}

cardano_transaction_body_t*
TransactionBody::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TransactionBody");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionBody& lhs, TransactionBody& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
