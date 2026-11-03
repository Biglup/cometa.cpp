/**
 * @file SubTransactionBuilder.cpp
 *
 * @brief Dijkstra sub-transaction construction.
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

#include <Cometa/TransactionBuilder/SubTransactionBuilder.h>

#include "../CardanoInterop.h"
#include "DeferredRedeemerAdapter.h"

#include <cardano/address/address.h>
#include <cardano/address/reward_address.h>
#include <cardano/assets/asset_id.h>
#include <cardano/assets/asset_name.h>
#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/certs/certificate.h>
#include <cardano/common/anchor.h>
#include <cardano/common/credential.h>
#include <cardano/common/datum.h>
#include <cardano/common/drep.h>
#include <cardano/common/governance_action_id.h>
#include <cardano/common/utxo.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/scripts/script.h>
#include <cardano/slot_config.h>
#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction_body/account_balance_interval.h>
#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/value.h>
#include <cardano/transaction_builder/sub_transaction_builder.h>
#include <cardano/voting_procedures/voter.h>
#include <cardano/voting_procedures/voting_procedure.h>

#include <stdexcept>
#include <string>
#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/** @brief Copies an address. @param address The address. @return An owned native address. */
cardano_address_t*
copyAddress(const Address& address)
{
  const std::vector<std::uint8_t>                         bytes  = address.toBytes();
  cardano_address_t*                                      value  = nullptr;
  const cardano_error_t                                   status = cardano_address_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);
  checkCardanoError(status, "SubTransactionBuilder address");
  return owner.release();
}

/** @brief Copies a reward address. @param address The address. @return An owned native address. */
cardano_reward_address_t*
copyRewardAddress(const RewardAddress& address)
{
  const std::vector<std::uint8_t>                                       bytes  = address.toBytes();
  cardano_reward_address_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_reward_address_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);
  checkCardanoError(status, "SubTransactionBuilder reward address");
  return owner.release();
}

/** @brief Copies a hash to native storage. @param hash The hash. @return An owned native hash. */
cardano_blake2b_hash_t*
copyHash(const Blake2bHash& hash)
{
  const std::vector<std::uint8_t>                                   bytes  = hash.toBytes();
  cardano_blake2b_hash_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_blake2b_hash_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);
  checkCardanoError(status, "SubTransactionBuilder hash");
  return owner.release();
}

/** @brief Copies an optional anchor. @param anchor The metadata anchor. @return An owned native anchor or null. */
cardano_anchor_t*
copyAnchor(const std::optional<Anchor>& anchor)
{
  if (!anchor)
  {
    return nullptr;
  }

  return decodeCbor<cardano_anchor_t, cardano_anchor_unref>(anchor->toCbor(), cardano_anchor_from_cbor);
}

/** @brief Copies optional Plutus data. @param data The data. @return An owned native value or null. */
cardano_plutus_data_t*
copyPlutusData(const std::optional<PlutusData>& data)
{
  if (!data)
  {
    return nullptr;
  }

  return decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(
    data->toCbor(), cardano_plutus_data_from_cbor);
}
} // namespace

/* IMPLEMENTATION ************************************************************/

SubTransactionBuilder::SubTransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots)
: m_value(nullptr)
{
  if (slots.slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be positive");
  }

  const cardano_slot_config_t nativeSlots { slots.zeroTime, slots.zeroSlot, slots.slotLength };
  m_value = cardano_sub_tx_builder_new(parameters.checked(), &nativeSlots);

  if (!m_value)
  {
    throw std::bad_alloc();
  }
}

SubTransactionBuilder::SubTransactionBuilder(SubTransactionBuilder&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_deferredRedeemers(std::move(other.m_deferredRedeemers))
{
}

SubTransactionBuilder::~SubTransactionBuilder()
{
  cardano_sub_tx_builder_unref(&m_value);
}

SubTransactionBuilder&
SubTransactionBuilder::setNetworkId(NetworkId network)
{
  cardano_sub_tx_builder_set_network_id(checked(), static_cast<cardano_network_id_t>(network));
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::setInvalidBefore(std::uint64_t slot)
{
  cardano_sub_tx_builder_set_invalid_before(checked(), slot);
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::setInvalidAfter(std::uint64_t slot)
{
  cardano_sub_tx_builder_set_invalid_after(checked(), slot);
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addInput(
  const Utxo&                      utxo,
  const std::optional<PlutusData>& redeemer,
  const std::optional<PlutusData>& datum)
{
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeDatum(copyPlutusData(datum));

  cardano_sub_tx_builder_add_input(checked(), nativeUtxo.get(), nativeRedeemer.get(), nativeDatum.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addInputWithDeferredRedeemer(
  const Utxo&                      utxo,
  DeferredRedeemer                 callback,
  const std::optional<PlutusData>& datum)
{
  if (!callback)
  {
    throw std::invalid_argument("Deferred redeemer callback is required");
  }

  CardanoObject<cardano_utxo_t, cardano_utxo_unref> nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeDatum(copyPlutusData(datum));

  std::unique_ptr<DeferredRedeemerContext> context(new DeferredRedeemerContext { std::move(callback), nullptr });
  m_deferredRedeemers.push_back(std::move(context));
  cardano_sub_tx_builder_add_input_with_deferred_redeemer(
    checked(), nativeUtxo.get(), &invokeDeferredRedeemer, m_deferredRedeemers.back().get(), nativeDatum.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addOutput(const TransactionOutput& output)
{
  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> native(
    decodeCbor<cardano_transaction_output_t, cardano_transaction_output_unref>(
      output.toCbor(), cardano_transaction_output_from_cbor));
  cardano_sub_tx_builder_add_output(checked(), native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addReferenceInput(const Utxo& utxo)
{
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  cardano_sub_tx_builder_add_reference_input(checked(), native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::sendLovelace(const Address& address, std::uint64_t lovelace)
{
  CardanoObject<cardano_address_t, cardano_address_unref> native(copyAddress(address));
  cardano_sub_tx_builder_send_lovelace(checked(), native.get(), lovelace);
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::sendValue(const Address& address, const Value& value)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_value_t, cardano_value_unref>     nativeValue(
    decodeCbor<cardano_value_t, cardano_value_unref>(value.toCbor(), cardano_value_from_cbor));
  cardano_sub_tx_builder_send_value(checked(), nativeAddress.get(), nativeValue.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::lockLovelace(const Address& address, std::uint64_t lovelace, const Datum& datum)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_datum_t, cardano_datum_unref>     nativeDatum(
    decodeCbor<cardano_datum_t, cardano_datum_unref>(datum.toCbor(), cardano_datum_from_cbor));
  cardano_sub_tx_builder_lock_lovelace(checked(), nativeAddress.get(), lovelace, nativeDatum.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::lockValue(const Address& address, const Value& value, const Datum& datum)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_value_t, cardano_value_unref>     nativeValue(
    decodeCbor<cardano_value_t, cardano_value_unref>(value.toCbor(), cardano_value_from_cbor));
  CardanoObject<cardano_datum_t, cardano_datum_unref> nativeDatum(
    decodeCbor<cardano_datum_t, cardano_datum_unref>(datum.toCbor(), cardano_datum_from_cbor));
  cardano_sub_tx_builder_lock_value(checked(), nativeAddress.get(), nativeValue.get(), nativeDatum.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::mintToken(
  const AssetId&                   asset,
  std::int64_t                     amount,
  const std::optional<PlutusData>& redeemer)
{
  if (asset.isLovelace())
  {
    throw std::invalid_argument("Cannot mint lovelace");
  }

  const std::vector<std::uint8_t>                           bytes  = asset.toBytes();
  cardano_asset_id_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_asset_id_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> nativeAsset(value);
  checkCardanoError(status, "SubTransactionBuilder asset");

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_mint_token_with_id(checked(), nativeAsset.get(), amount, nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::mintTokenWithDeferredRedeemer(const AssetId& asset, std::int64_t amount, DeferredRedeemer callback)
{
  if (asset.isLovelace() || !callback)
  {
    throw std::invalid_argument("Minting requires a native asset and a deferred redeemer callback");
  }

  const std::vector<std::uint8_t>                                   policyBytes  = asset.policyId()->toBytes();
  cardano_blake2b_hash_t*                                           policyValue  = nullptr;
  const cardano_error_t                                             policyStatus = cardano_blake2b_hash_from_bytes(policyBytes.data(), policyBytes.size(), &policyValue);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> nativePolicy(policyValue);
  checkCardanoError(policyStatus, "SubTransactionBuilder policy");

  const std::vector<std::uint8_t>                               nameBytes  = asset.assetName()->toBytes();
  cardano_asset_name_t*                                         nameValue  = nullptr;
  const cardano_error_t                                         nameStatus = cardano_asset_name_from_bytes(byteData(nameBytes), nameBytes.size(), &nameValue);
  CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> nativeName(nameValue);
  checkCardanoError(nameStatus, "SubTransactionBuilder asset name");

  std::unique_ptr<DeferredRedeemerContext> context(new DeferredRedeemerContext { std::move(callback), nullptr });
  m_deferredRedeemers.push_back(std::move(context));
  cardano_sub_tx_builder_mint_token_with_deferred_redeemer(
    checked(), nativePolicy.get(), nativeName.get(), amount, &invokeDeferredRedeemer, m_deferredRedeemers.back().get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addSigner(const Blake2bHash& keyHash)
{
  const std::vector<std::uint8_t>                                   bytes  = keyHash.toBytes();
  cardano_blake2b_hash_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_blake2b_hash_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> native(value);
  checkCardanoError(status, "SubTransactionBuilder signer");

  cardano_sub_tx_builder_add_signer(checked(), native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addDatum(const PlutusData& datum)
{
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> native(
    decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(datum.toCbor(), cardano_plutus_data_from_cbor));
  cardano_sub_tx_builder_add_datum(checked(), native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addScript(const Script& script)
{
  CardanoObject<cardano_script_t, cardano_script_unref> native(
    decodeCbor<cardano_script_t, cardano_script_unref>(script.toCbor(), cardano_script_from_cbor));
  cardano_sub_tx_builder_add_script(checked(), native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::setMetadata(std::uint64_t tag, const Metadatum& value)
{
  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> native(
    decodeCbor<cardano_metadatum_t, cardano_metadatum_unref>(value.toCbor(), cardano_metadatum_from_cbor));
  cardano_sub_tx_builder_set_metadata(checked(), tag, native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addDirectDeposit(const RewardAddress& address, std::uint64_t lovelace)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> native(copyRewardAddress(address));
  cardano_sub_tx_builder_add_direct_deposit(checked(), native.get(), lovelace);
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addAccountBalanceInterval(const RewardAddress& address, const AccountBalanceInterval& interval)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref>                     nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> nativeInterval(
    decodeCbor<cardano_account_balance_interval_t, cardano_account_balance_interval_unref>(
      interval.toCbor(), cardano_account_balance_interval_from_cbor));
  cardano_sub_tx_builder_add_account_balance_interval(checked(), nativeAddress.get(), nativeInterval.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::withdrawRewards(
  const RewardAddress&             address,
  std::int64_t                     lovelace,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref>       nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_withdraw_rewards(checked(), nativeAddress.get(), lovelace, nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::withdrawRewardsWithDeferredRedeemer(
  const RewardAddress& address,
  std::int64_t         lovelace,
  DeferredRedeemer     callback)
{
  if (!callback)
  {
    throw std::invalid_argument("Deferred redeemer callback is required");
  }

  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeAddress(copyRewardAddress(address));
  std::unique_ptr<DeferredRedeemerContext>                              context(new DeferredRedeemerContext { std::move(callback), nullptr });
  m_deferredRedeemers.push_back(std::move(context));
  cardano_sub_tx_builder_withdraw_rewards_with_deferred_redeemer(
    checked(), nativeAddress.get(), lovelace, &invokeDeferredRedeemer, m_deferredRedeemers.back().get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addCertificate(
  const Certificate&               certificate,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> nativeCertificate(
    decodeCbor<cardano_certificate_t, cardano_certificate_unref>(certificate.toCbor(), cardano_certificate_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_add_certificate(checked(), nativeCertificate.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addCertificateWithDeferredRedeemer(const Certificate& certificate, DeferredRedeemer callback)
{
  if (!callback)
  {
    throw std::invalid_argument("Deferred redeemer callback is required");
  }

  CardanoObject<cardano_certificate_t, cardano_certificate_unref> nativeCertificate(
    decodeCbor<cardano_certificate_t, cardano_certificate_unref>(certificate.toCbor(), cardano_certificate_from_cbor));
  std::unique_ptr<DeferredRedeemerContext> context(new DeferredRedeemerContext { std::move(callback), nullptr });
  m_deferredRedeemers.push_back(std::move(context));
  cardano_sub_tx_builder_add_certificate_with_deferred_redeemer(
    checked(), nativeCertificate.get(), &invokeDeferredRedeemer, m_deferredRedeemers.back().get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::registerRewardAddress(
  const RewardAddress&             address,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref>       nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_register_reward_address(checked(), nativeAddress.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::deregisterRewardAddress(
  const RewardAddress&             address,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref>       nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_deregister_reward_address(checked(), nativeAddress.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::delegateStake(
  const RewardAddress&             address,
  const Blake2bHash&               poolId,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref>     nativePool(copyHash(poolId));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref>       nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_delegate_stake(checked(), nativeAddress.get(), nativePool.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::delegateVotingPower(
  const RewardAddress&             address,
  const DRep&                      drep,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_drep_t, cardano_drep_unref>                     nativeDRep(
    decodeCbor<cardano_drep_t, cardano_drep_unref>(drep.toCbor(), cardano_drep_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_delegate_voting_power(checked(), nativeAddress.get(), nativeDRep.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::registerDRep(
  const DRep&                      drep,
  const std::optional<Anchor>&     anchor,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_drep_t, cardano_drep_unref> nativeDRep(
    decodeCbor<cardano_drep_t, cardano_drep_unref>(drep.toCbor(), cardano_drep_from_cbor));
  CardanoObject<cardano_anchor_t, cardano_anchor_unref>           nativeAnchor(copyAnchor(anchor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_register_drep(checked(), nativeDRep.get(), nativeAnchor.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::updateDRep(
  const DRep&                      drep,
  const std::optional<Anchor>&     anchor,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_drep_t, cardano_drep_unref> nativeDRep(
    decodeCbor<cardano_drep_t, cardano_drep_unref>(drep.toCbor(), cardano_drep_from_cbor));
  CardanoObject<cardano_anchor_t, cardano_anchor_unref>           nativeAnchor(copyAnchor(anchor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_update_drep(checked(), nativeDRep.get(), nativeAnchor.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::deregisterDRep(
  const DRep&                      drep,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_drep_t, cardano_drep_unref> nativeDRep(
    decodeCbor<cardano_drep_t, cardano_drep_unref>(drep.toCbor(), cardano_drep_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_deregister_drep(checked(), nativeDRep.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::vote(
  const Voter&                     voter,
  const GovernanceActionId&        action,
  const VotingProcedure&           vote,
  const std::optional<PlutusData>& redeemer)
{
  CardanoObject<cardano_voter_t, cardano_voter_unref> nativeVoter(
    decodeCbor<cardano_voter_t, cardano_voter_unref>(voter.toCbor(), cardano_voter_from_cbor));
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> nativeAction(
    decodeCbor<cardano_governance_action_id_t, cardano_governance_action_id_unref>(
      action.toCbor(), cardano_governance_action_id_from_cbor));
  CardanoObject<cardano_voting_procedure_t, cardano_voting_procedure_unref> nativeVote(
    decodeCbor<cardano_voting_procedure_t, cardano_voting_procedure_unref>(
      vote.toCbor(), cardano_voting_procedure_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_sub_tx_builder_vote(checked(), nativeVoter.get(), nativeAction.get(), nativeVote.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::voteWithDeferredRedeemer(
  const Voter&              voter,
  const GovernanceActionId& action,
  const VotingProcedure&    vote,
  DeferredRedeemer          callback)
{
  if (!callback)
  {
    throw std::invalid_argument("Deferred redeemer callback is required");
  }

  CardanoObject<cardano_voter_t, cardano_voter_unref> nativeVoter(
    decodeCbor<cardano_voter_t, cardano_voter_unref>(voter.toCbor(), cardano_voter_from_cbor));
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> nativeAction(
    decodeCbor<cardano_governance_action_id_t, cardano_governance_action_id_unref>(
      action.toCbor(), cardano_governance_action_id_from_cbor));
  CardanoObject<cardano_voting_procedure_t, cardano_voting_procedure_unref> nativeVote(
    decodeCbor<cardano_voting_procedure_t, cardano_voting_procedure_unref>(
      vote.toCbor(), cardano_voting_procedure_from_cbor));
  std::unique_ptr<DeferredRedeemerContext> context(new DeferredRedeemerContext { std::move(callback), nullptr });
  m_deferredRedeemers.push_back(std::move(context));
  cardano_sub_tx_builder_vote_with_deferred_redeemer(
    checked(), nativeVoter.get(), nativeAction.get(), nativeVote.get(), &invokeDeferredRedeemer, m_deferredRedeemers.back().get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addGuard(const Credential& guard)
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> native(
    decodeCbor<cardano_credential_t, cardano_credential_unref>(guard.toCbor(), cardano_credential_from_cbor));
  cardano_sub_tx_builder_add_guard(checked(), native.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::addGuard(const Credential& guard, const PlutusData& redeemer)
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> nativeGuard(
    decodeCbor<cardano_credential_t, cardano_credential_unref>(guard.toCbor(), cardano_credential_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(
    decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(redeemer.toCbor(), cardano_plutus_data_from_cbor));
  cardano_sub_tx_builder_add_guard_with_redeemer(checked(), nativeGuard.get(), nativeRedeemer.get());
  return *this;
}

SubTransactionBuilder&
SubTransactionBuilder::requireTopLevelGuard(
  const Credential&                guard,
  const std::optional<PlutusData>& datum)
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> nativeGuard(
    decodeCbor<cardano_credential_t, cardano_credential_unref>(guard.toCbor(), cardano_credential_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeDatum(copyPlutusData(datum));
  cardano_sub_tx_builder_require_top_level_guard(checked(), nativeGuard.get(), nativeDatum.get());
  return *this;
}

SubTransaction
SubTransactionBuilder::build()
{
  for (const std::unique_ptr<DeferredRedeemerContext>& context: m_deferredRedeemers)
  {
    context->error = nullptr;
  }

  cardano_sub_transaction_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_sub_tx_builder_build(checked(), &value);
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> owner(value);

  if (status != CARDANO_SUCCESS)
  {
    for (const std::unique_ptr<DeferredRedeemerContext>& context: m_deferredRedeemers)
    {
      if (context->error)
      {
        std::rethrow_exception(context->error);
      }
    }

    const std::string message = std::string("SubTransactionBuilder build: ") + cardano_sub_tx_builder_get_last_error(checked());
    throw Error(static_cast<ErrorCode>(status), message.c_str());
  }

  return SubTransaction::fromCbor(encodeCbor(owner.get(), cardano_sub_transaction_to_cbor));
}

SubTransactionBuilder&
SubTransactionBuilder::operator=(SubTransactionBuilder&& other) noexcept
{
  if (this != &other)
  {
    cardano_sub_tx_builder_unref(&m_value);
    m_value             = std::exchange(other.m_value, nullptr);
    m_deferredRedeemers = std::move(other.m_deferredRedeemers);
  }

  return *this;
}

cardano_sub_tx_builder_t*
SubTransactionBuilder::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("SubTransactionBuilder has no value");
  }

  return m_value;
}
} // namespace Cometa
