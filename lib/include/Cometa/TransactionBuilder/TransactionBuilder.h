/**
 * @file TransactionBuilder.h
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Address/Address.h>
#include <Cometa/Address/RewardAddress.h>
#include <Cometa/Assets/AssetId.h>
#include <Cometa/AuxiliaryData/Metadatum.h>
#include <Cometa/Certificates/Certificate.h>
#include <Cometa/Common/Credential.h>
#include <Cometa/Common/Datum.h>
#include <Cometa/Common/GovernanceActionId.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/Scripts/Script.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/Transaction/SubTransaction.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/TransactionBody/AccountBalanceInterval.h>
#include <Cometa/TransactionBody/TransactionOutput.h>
#include <Cometa/TransactionBody/Value.h>
#include <Cometa/TransactionBuilder/CoinSelector.h>
#include <Cometa/TransactionBuilder/DeferredRedeemer.h>
#include <Cometa/TransactionBuilder/TransactionEvaluator.h>
#include <Cometa/VotingProcedures/Voter.h>
#include <Cometa/VotingProcedures/VotingProcedure.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_tx_builder_t;

namespace Cometa
{
struct DeferredRedeemerContext;
} // namespace Cometa

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Builds balanced Cardano transactions from C++ values. */
class TransactionBuilder
{
  public:

    /**
     * @brief Creates a builder for a network and protocol version.
     * @param parameters Current protocol parameters.
     * @param slots The network slot timeline.
     */
    TransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots);

    /** @brief Moves a builder. @param other The source builder. */
    TransactionBuilder(TransactionBuilder&& other) noexcept;

    /** @brief Releases builder resources. */
    ~TransactionBuilder();

  public:

    /** @brief Sets the coin selection strategy. @param selector The strategy to retain. @return This builder. */
    TransactionBuilder& setCoinSelector(std::shared_ptr<CoinSelector> selector);

    /** @brief Sets the script evaluator. @param evaluator The evaluator to retain. @return This builder. */
    TransactionBuilder& setTransactionEvaluator(std::shared_ptr<TransactionEvaluator> evaluator);

    /** @brief Sets the transaction network ID. @param network The network ID. @return This builder. */
    TransactionBuilder& setNetworkId(NetworkId network);

    /** @brief Sets the address for change outputs. @param address The change address. @return This builder. */
    TransactionBuilder& setChangeAddress(const Address& address);

    /** @brief Sets the collateral change address. @param address The change address. @return This builder. */
    TransactionBuilder& setCollateralChangeAddress(const Address& address);

    /** @brief Sets spendable outputs for coin selection. @param utxos The outputs. @return This builder. */
    TransactionBuilder& setUtxos(const UtxoList& utxos);

    /** @brief Sets collateral outputs for script execution. @param utxos The outputs. @return This builder. */
    TransactionBuilder& setCollateralUtxos(const UtxoList& utxos);

    /** @brief Sets the earliest valid slot. @param slot The slot. @return This builder. */
    TransactionBuilder& setInvalidBefore(std::uint64_t slot);

    /** @brief Sets the first invalid slot. @param slot The slot. @return This builder. */
    TransactionBuilder& setInvalidAfter(std::uint64_t slot);

    /** @brief Sets the minimum fee. @param lovelace The minimum lovelace. @return This builder. */
    TransactionBuilder& setMinimumFee(std::uint64_t lovelace);

    /** @brief Sets a treasury donation. @param lovelace The donation. @return This builder. */
    TransactionBuilder& setDonation(std::uint64_t lovelace);

    /** @brief Adds a payment in lovelace. @param address The recipient. @param lovelace The amount. @return This builder. */
    TransactionBuilder& sendLovelace(const Address& address, std::uint64_t lovelace);

    /** @brief Adds a multi-asset payment. @param address The recipient. @param value The amount. @return This builder. */
    TransactionBuilder& sendValue(const Address& address, const Value& value);

    /** @brief Locks lovelace with a datum. @param address The script address. @param lovelace The amount. @param datum The output datum. @return This builder. */
    TransactionBuilder& lockLovelace(const Address& address, std::uint64_t lovelace, const Datum& datum);

    /** @brief Locks a value with a datum. @param address The script address. @param value The amount. @param datum The output datum. @return This builder. */
    TransactionBuilder& lockValue(const Address& address, const Value& value, const Datum& datum);

    /**
     * @brief Adds a spendable input.
     * @param utxo The output to spend.
     * @param redeemer The script redeemer, when needed.
     * @param datum The datum, when it is not in the output.
     * @return This builder.
     */
    TransactionBuilder& addInput(
      const Utxo&                      utxo,
      const std::optional<PlutusData>& redeemer = std::nullopt,
      const std::optional<PlutusData>& datum    = std::nullopt);

    /** @brief Adds an input with a redeemer produced after balancing. @param utxo The output to spend. @param callback The deterministic producer. @param datum The datum, when needed. @return This builder. */
    TransactionBuilder& addInputWithDeferredRedeemer(
      const Utxo&                      utxo,
      DeferredRedeemer                 callback,
      const std::optional<PlutusData>& datum = std::nullopt);

    /** @brief Adds an output. @param output The output to add. @return This builder. */
    TransactionBuilder& addOutput(const TransactionOutput& output);

    /** @brief Adds a reference input. @param utxo The referenced output. @return This builder. */
    TransactionBuilder& addReferenceInput(const Utxo& utxo);

    /** @brief Adds a required signer. @param keyHash The payment key hash. @return This builder. */
    TransactionBuilder& addSigner(const Blake2bHash& keyHash);

    /** @brief Adds a Dijkstra guard. @param guard The guard credential. @return This builder. */
    TransactionBuilder& addGuard(const Credential& guard);

    /** @brief Adds a script guard with its redeemer. @param guard The guard credential. @param redeemer The guard redeemer. @return This builder. */
    TransactionBuilder& addGuard(const Credential& guard, const PlutusData& redeemer);

    /** @brief Adds a datum to the witness set. @param datum The datum. @return This builder. */
    TransactionBuilder& addDatum(const PlutusData& datum);

    /** @brief Adds a native or Plutus script. @param script The script. @return This builder. */
    TransactionBuilder& addScript(const Script& script);

    /** @brief Adds transaction metadata. @param tag The metadata label. @param value The metadata value. @return This builder. */
    TransactionBuilder& setMetadata(std::uint64_t tag, const Metadatum& value);

    /** @brief Adds a direct reward-account deposit. @param address The reward account. @param lovelace The amount. @return This builder. */
    TransactionBuilder& addDirectDeposit(const RewardAddress& address, std::uint64_t lovelace);

    /** @brief Adds an account balance bound. @param address The reward account. @param interval The allowed balance. @return This builder. */
    TransactionBuilder& addAccountBalanceInterval(const RewardAddress& address, const AccountBalanceInterval& interval);

    /** @brief Adds a starting account balance bound. @param address The reward account. @param interval The starting balance. @return This builder. */
    TransactionBuilder& addStartingAccountBalanceInterval(const RewardAddress& address, const AccountBalanceInterval& interval);

    /** @brief Adds a signed sub-transaction. @param transaction The sub-transaction. @param resolvedUtxos Its resolved inputs. @return This builder. */
    TransactionBuilder& addSubTransaction(const SubTransaction& transaction, const UtxoList& resolvedUtxos);

    /** @brief Withdraws rewards from a reward account. @param address The reward account. @param lovelace The withdrawal amount. @param redeemer The script redeemer, when needed. @return This builder. */
    TransactionBuilder& withdrawRewards(
      const RewardAddress&             address,
      std::int64_t                     lovelace,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Withdraws rewards with a redeemer produced after balancing. @param address The reward account. @param lovelace The withdrawal amount. @param callback The deterministic producer. @return This builder. */
    TransactionBuilder& withdrawRewardsWithDeferredRedeemer(
      const RewardAddress& address,
      std::int64_t         lovelace,
      DeferredRedeemer     callback);

    /** @brief Adds a certificate. @param certificate The certificate. @param redeemer The script redeemer, when needed. @return This builder. */
    TransactionBuilder& addCertificate(
      const Certificate&               certificate,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Adds a certificate with a redeemer produced after balancing. @param certificate The certificate. @param callback The deterministic producer. @return This builder. */
    TransactionBuilder& addCertificateWithDeferredRedeemer(const Certificate& certificate, DeferredRedeemer callback);

    /** @brief Casts a governance vote. @param voter The voter. @param action The governance action. @param vote The choice and metadata. @param redeemer The script redeemer, when needed. @return This builder. */
    TransactionBuilder& vote(
      const Voter&                     voter,
      const GovernanceActionId&        action,
      const VotingProcedure&           vote,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Casts a vote with a redeemer produced after balancing. @param voter The voter. @param action The governance action. @param vote The choice and metadata. @param callback The deterministic producer. @return This builder. */
    TransactionBuilder& voteWithDeferredRedeemer(
      const Voter&              voter,
      const GovernanceActionId& action,
      const VotingProcedure&    vote,
      DeferredRedeemer          callback);

    /**
     * @brief Mints or burns a native asset.
     * @param asset The asset identifier.
     * @param amount Positive to mint, negative to burn.
     * @param redeemer The script redeemer, when needed.
     * @return This builder.
     */
    TransactionBuilder& mintToken(
      const AssetId&                   asset,
      std::int64_t                     amount,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Mints an asset with a redeemer produced after balancing. @param asset The asset. @param amount The signed quantity. @param callback The deterministic producer. @return This builder. */
    TransactionBuilder& mintTokenWithDeferredRedeemer(const AssetId& asset, std::int64_t amount, DeferredRedeemer callback);

    /** @brief Reserves witness space for signing keys. @param count The number of keys. @return This builder. */
    TransactionBuilder& padSignerCount(std::size_t count);

    /** @brief Balances and builds the transaction. @return The transaction. */
    Transaction build();

  public:

    /** @brief Moves another builder into this instance. @param other The source. @return This builder. */
    TransactionBuilder& operator=(TransactionBuilder&& other) noexcept;

    /** @brief A builder cannot be copied. */
    TransactionBuilder(const TransactionBuilder&) = delete;

    /** @brief A builder cannot be copy assigned. */
    TransactionBuilder& operator=(const TransactionBuilder&) = delete;

  private:

    /** @brief Gets the native builder or throws after a move. @return The native builder. */
    cardano_tx_builder_t* checked() const;

  private:

    /** @brief Owned native builder. */
    cardano_tx_builder_t* m_value;

    /** @brief Retained selection strategy, when configured. */
    std::shared_ptr<CoinSelector> m_coinSelector;

    /** @brief Retained execution evaluator, when configured. */
    std::shared_ptr<TransactionEvaluator> m_evaluator;

    /** @brief Retained redeemer callbacks and their failure state. */
    std::vector<std::unique_ptr<DeferredRedeemerContext>> m_deferredRedeemers;
};
} // namespace Cometa
