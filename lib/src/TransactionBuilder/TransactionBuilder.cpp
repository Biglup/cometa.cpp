/**
 * @file TransactionBuilder.cpp
 *
 * @brief Balanced transaction construction.
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

#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include "../CardanoInterop.h"
#include "CoinSelectorAdapter.h"
#include "DeferredRedeemerAdapter.h"
#include "TransactionEvaluatorAdapter.h"

#include <cardano/address/address.h>
#include <cardano/address/reward_address.h>
#include <cardano/assets/asset_id.h>
#include <cardano/assets/asset_name.h>
#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/common/credential.h>
#include <cardano/common/datum.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/scripts/script.h>
#include <cardano/slot_config.h>
#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_body/account_balance_interval.h>
#include <cardano/transaction_body/transaction_output.h>
#include <cardano/transaction_body/value.h>
#include <cardano/transaction_builder/coin_selection/coin_selector.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator.h>
#include <cardano/transaction_builder/transaction_builder.h>

#include <stdexcept>
#include <string>
#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Copies a payment address to native storage.
 * @param address The address to copy.
 * @return An owned native address.
 */
cardano_address_t*
copyAddress(const Address& address)
{
  const std::vector<std::uint8_t>                         bytes  = address.toBytes();
  cardano_address_t*                                      value  = nullptr;
  const cardano_error_t                                   status = cardano_address_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);
  checkCardanoError(status, "TransactionBuilder address");
  return owner.release();
}

/**
 * @brief Copies a reward address to native storage.
 * @param address The address to copy.
 * @return An owned native reward address.
 */
cardano_reward_address_t*
copyRewardAddress(const RewardAddress& address)
{
  const std::vector<std::uint8_t>                                       bytes  = address.toBytes();
  cardano_reward_address_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_reward_address_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);
  checkCardanoError(status, "TransactionBuilder reward address");
  return owner.release();
}

/**
 * @brief Copies unspent outputs to native storage.
 * @param utxos The outputs to copy.
 * @return An owned native list.
 */
cardano_utxo_list_t*
copyUtxos(const UtxoList& utxos)
{
  cardano_utxo_list_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_utxo_list_new(&value);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> owner(value);
  checkCardanoError(status, "TransactionBuilder UtxoList");

  for (const Utxo& utxo: utxos)
  {
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
      decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
    checkCardanoError(cardano_utxo_list_add(owner.get(), native.get()), "TransactionBuilder UtxoList add");
  }

  return owner.release();
}

/**
 * @brief Copies optional Plutus data to native storage.
 * @param value The data to copy, or no value.
 * @return An owned native datum or null.
 */
cardano_plutus_data_t*
copyPlutusData(const std::optional<PlutusData>& value)
{
  if (!value)
  {
    return nullptr;
  }

  return decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(
    value->toCbor(), cardano_plutus_data_from_cbor);
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TransactionBuilder::TransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots)
: m_value(nullptr)
{
  if (slots.slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be positive");
  }

  const cardano_slot_config_t nativeSlots { slots.zeroTime, slots.zeroSlot, slots.slotLength };
  m_value = cardano_tx_builder_new(parameters.checked(), &nativeSlots);

  if (!m_value)
  {
    throw std::bad_alloc();
  }
}

TransactionBuilder::TransactionBuilder(TransactionBuilder&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_coinSelector(std::move(other.m_coinSelector))
, m_evaluator(std::move(other.m_evaluator))
, m_deferredRedeemers(std::move(other.m_deferredRedeemers))
{
}

TransactionBuilder::~TransactionBuilder()
{
  cardano_tx_builder_unref(&m_value);
}

TransactionBuilder&
TransactionBuilder::setCoinSelector(std::shared_ptr<CoinSelector> selector)
{
  CardanoObject<cardano_coin_selector_t, cardano_coin_selector_unref> native(
    createCoinSelectorAdapter(selector));
  cardano_tx_builder_set_coin_selector(checked(), native.get());
  m_coinSelector = std::move(selector);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setTransactionEvaluator(std::shared_ptr<TransactionEvaluator> evaluator)
{
  CardanoObject<cardano_tx_evaluator_t, cardano_tx_evaluator_unref> native(
    createTransactionEvaluatorAdapter(evaluator));
  cardano_tx_builder_set_tx_evaluator(checked(), native.get());
  m_evaluator = std::move(evaluator);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setNetworkId(NetworkId network)
{
  cardano_tx_builder_set_network_id(checked(), static_cast<cardano_network_id_t>(network));
  return *this;
}

TransactionBuilder&
TransactionBuilder::setChangeAddress(const Address& address)
{
  CardanoObject<cardano_address_t, cardano_address_unref> native(copyAddress(address));
  cardano_tx_builder_set_change_address(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setCollateralChangeAddress(const Address& address)
{
  CardanoObject<cardano_address_t, cardano_address_unref> native(copyAddress(address));
  cardano_tx_builder_set_collateral_change_address(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setUtxos(const UtxoList& utxos)
{
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> native(copyUtxos(utxos));
  cardano_tx_builder_set_utxos(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setCollateralUtxos(const UtxoList& utxos)
{
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> native(copyUtxos(utxos));
  cardano_tx_builder_set_collateral_utxos(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setInvalidBefore(std::uint64_t slot)
{
  cardano_tx_builder_set_invalid_before(checked(), slot);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setInvalidAfter(std::uint64_t slot)
{
  cardano_tx_builder_set_invalid_after(checked(), slot);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setMinimumFee(std::uint64_t lovelace)
{
  cardano_tx_builder_set_minimum_fee(checked(), lovelace);
  return *this;
}

TransactionBuilder&
TransactionBuilder::setDonation(std::uint64_t lovelace)
{
  cardano_tx_builder_set_donation(checked(), lovelace);
  return *this;
}

TransactionBuilder&
TransactionBuilder::sendLovelace(const Address& address, std::uint64_t lovelace)
{
  CardanoObject<cardano_address_t, cardano_address_unref> native(copyAddress(address));
  cardano_tx_builder_send_lovelace(checked(), native.get(), lovelace);
  return *this;
}

TransactionBuilder&
TransactionBuilder::sendValue(const Address& address, const Value& value)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_value_t, cardano_value_unref>     nativeValue(
    decodeCbor<cardano_value_t, cardano_value_unref>(value.toCbor(), cardano_value_from_cbor));
  cardano_tx_builder_send_value(checked(), nativeAddress.get(), nativeValue.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::lockLovelace(const Address& address, std::uint64_t lovelace, const Datum& datum)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_datum_t, cardano_datum_unref>     nativeDatum(
    decodeCbor<cardano_datum_t, cardano_datum_unref>(datum.toCbor(), cardano_datum_from_cbor));
  cardano_tx_builder_lock_lovelace(checked(), nativeAddress.get(), lovelace, nativeDatum.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::lockValue(const Address& address, const Value& value, const Datum& datum)
{
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(copyAddress(address));
  CardanoObject<cardano_value_t, cardano_value_unref>     nativeValue(
    decodeCbor<cardano_value_t, cardano_value_unref>(value.toCbor(), cardano_value_from_cbor));
  CardanoObject<cardano_datum_t, cardano_datum_unref> nativeDatum(
    decodeCbor<cardano_datum_t, cardano_datum_unref>(datum.toCbor(), cardano_datum_from_cbor));
  cardano_tx_builder_lock_value(checked(), nativeAddress.get(), nativeValue.get(), nativeDatum.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addInput(
  const Utxo&                      utxo,
  const std::optional<PlutusData>& redeemer,
  const std::optional<PlutusData>& datum)
{
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> nativeUtxo(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeDatum(copyPlutusData(datum));

  cardano_tx_builder_add_input(checked(), nativeUtxo.get(), nativeRedeemer.get(), nativeDatum.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addInputWithDeferredRedeemer(
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
  cardano_tx_builder_add_input_with_deferred_redeemer(
    checked(), nativeUtxo.get(), &invokeDeferredRedeemer, m_deferredRedeemers.back().get(), nativeDatum.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addOutput(const TransactionOutput& output)
{
  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> native(
    decodeCbor<cardano_transaction_output_t, cardano_transaction_output_unref>(
      output.toCbor(), cardano_transaction_output_from_cbor));
  cardano_tx_builder_add_output(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addReferenceInput(const Utxo& utxo)
{
  CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
    decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));
  cardano_tx_builder_add_reference_input(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addSigner(const Blake2bHash& keyHash)
{
  const std::vector<std::uint8_t>                                   bytes  = keyHash.toBytes();
  cardano_blake2b_hash_t*                                           value  = nullptr;
  const cardano_error_t                                             status = cardano_blake2b_hash_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> native(value);
  checkCardanoError(status, "TransactionBuilder signer");

  cardano_tx_builder_add_signer(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addGuard(const Credential& guard)
{
  CardanoObject<cardano_credential_t, cardano_credential_unref> native(
    decodeCbor<cardano_credential_t, cardano_credential_unref>(guard.toCbor(), cardano_credential_from_cbor));
  cardano_tx_builder_add_guard(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addDatum(const PlutusData& datum)
{
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> native(
    decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(datum.toCbor(), cardano_plutus_data_from_cbor));
  cardano_tx_builder_add_datum(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addScript(const Script& script)
{
  CardanoObject<cardano_script_t, cardano_script_unref> native(
    decodeCbor<cardano_script_t, cardano_script_unref>(script.toCbor(), cardano_script_from_cbor));
  cardano_tx_builder_add_script(checked(), native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::setMetadata(std::uint64_t tag, const Metadatum& value)
{
  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> native(
    decodeCbor<cardano_metadatum_t, cardano_metadatum_unref>(value.toCbor(), cardano_metadatum_from_cbor));
  cardano_tx_builder_set_metadata(checked(), tag, native.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addDirectDeposit(const RewardAddress& address, std::uint64_t lovelace)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> native(copyRewardAddress(address));
  cardano_tx_builder_add_direct_deposit(checked(), native.get(), lovelace);
  return *this;
}

TransactionBuilder&
TransactionBuilder::addAccountBalanceInterval(const RewardAddress& address, const AccountBalanceInterval& interval)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref>                     nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> nativeInterval(
    decodeCbor<cardano_account_balance_interval_t, cardano_account_balance_interval_unref>(
      interval.toCbor(), cardano_account_balance_interval_from_cbor));
  cardano_tx_builder_add_account_balance_interval(checked(), nativeAddress.get(), nativeInterval.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addStartingAccountBalanceInterval(const RewardAddress& address, const AccountBalanceInterval& interval)
{
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref>                     nativeAddress(copyRewardAddress(address));
  CardanoObject<cardano_account_balance_interval_t, cardano_account_balance_interval_unref> nativeInterval(
    decodeCbor<cardano_account_balance_interval_t, cardano_account_balance_interval_unref>(
      interval.toCbor(), cardano_account_balance_interval_from_cbor));
  cardano_tx_builder_add_starting_account_balance_interval(checked(), nativeAddress.get(), nativeInterval.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::addSubTransaction(const SubTransaction& transaction, const UtxoList& resolvedUtxos)
{
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> nativeTransaction(
    decodeCbor<cardano_sub_transaction_t, cardano_sub_transaction_unref>(
      transaction.toCbor(), cardano_sub_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeUtxos(copyUtxos(resolvedUtxos));
  cardano_tx_builder_add_sub_transaction(checked(), nativeTransaction.get(), nativeUtxos.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::mintToken(
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
  checkCardanoError(status, "TransactionBuilder asset");

  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> nativeRedeemer(copyPlutusData(redeemer));
  cardano_tx_builder_mint_token_with_id(checked(), nativeAsset.get(), amount, nativeRedeemer.get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::mintTokenWithDeferredRedeemer(const AssetId& asset, std::int64_t amount, DeferredRedeemer callback)
{
  if (asset.isLovelace() || !callback)
  {
    throw std::invalid_argument("Minting requires a native asset and a deferred redeemer callback");
  }

  const std::vector<std::uint8_t>                                   policyBytes  = asset.policyId()->toBytes();
  cardano_blake2b_hash_t*                                           policyValue  = nullptr;
  const cardano_error_t                                             policyStatus = cardano_blake2b_hash_from_bytes(policyBytes.data(), policyBytes.size(), &policyValue);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> nativePolicy(policyValue);
  checkCardanoError(policyStatus, "TransactionBuilder policy");

  const std::vector<std::uint8_t>                               nameBytes  = asset.assetName()->toBytes();
  cardano_asset_name_t*                                         nameValue  = nullptr;
  const cardano_error_t                                         nameStatus = cardano_asset_name_from_bytes(byteData(nameBytes), nameBytes.size(), &nameValue);
  CardanoObject<cardano_asset_name_t, cardano_asset_name_unref> nativeName(nameValue);
  checkCardanoError(nameStatus, "TransactionBuilder asset name");

  std::unique_ptr<DeferredRedeemerContext> context(new DeferredRedeemerContext { std::move(callback), nullptr });
  m_deferredRedeemers.push_back(std::move(context));
  cardano_tx_builder_mint_token_with_deferred_redeemer(
    checked(), nativePolicy.get(), nativeName.get(), amount, &invokeDeferredRedeemer, m_deferredRedeemers.back().get());
  return *this;
}

TransactionBuilder&
TransactionBuilder::padSignerCount(std::size_t count)
{
  cardano_tx_builder_pad_signer_count(checked(), count);
  return *this;
}

Transaction
TransactionBuilder::build()
{
  for (const std::unique_ptr<DeferredRedeemerContext>& context: m_deferredRedeemers)
  {
    context->error = nullptr;
  }

  cardano_transaction_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_tx_builder_build(checked(), &value);
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> owner(value);

  if (status != CARDANO_SUCCESS)
  {
    for (const std::unique_ptr<DeferredRedeemerContext>& context: m_deferredRedeemers)
    {
      if (context->error)
      {
        std::rethrow_exception(context->error);
      }
    }

    const std::string message = std::string("TransactionBuilder build: ") + cardano_tx_builder_get_last_error(checked());
    throw Error(static_cast<ErrorCode>(status), message.c_str());
  }

  return Transaction::fromCbor(encodeCbor(owner.get(), cardano_transaction_to_cbor));
}

TransactionBuilder&
TransactionBuilder::operator=(TransactionBuilder&& other) noexcept
{
  if (this != &other)
  {
    cardano_tx_builder_unref(&m_value);
    m_value             = std::exchange(other.m_value, nullptr);
    m_coinSelector      = std::move(other.m_coinSelector);
    m_evaluator         = std::move(other.m_evaluator);
    m_deferredRedeemers = std::move(other.m_deferredRedeemers);
  }

  return *this;
}

cardano_tx_builder_t*
TransactionBuilder::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("TransactionBuilder has no value");
  }

  return m_value;
}
} // namespace Cometa
