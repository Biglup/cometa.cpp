/**
 * @file SubTransactionBuilder.h
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
#include <Cometa/Common/Utxo.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/Scripts/Script.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/Transaction/SubTransaction.h>
#include <Cometa/TransactionBody/AccountBalanceInterval.h>
#include <Cometa/TransactionBody/TransactionOutput.h>
#include <Cometa/TransactionBody/Value.h>
#include <Cometa/TransactionBuilder/DeferredRedeemer.h>
#include <Cometa/VotingProcedures/Voter.h>
#include <Cometa/VotingProcedures/VotingProcedure.h>

#include <cstdint>
#include <memory>
#include <optional>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_sub_tx_builder_t;

namespace Cometa
{
struct DeferredRedeemerContext;
} // namespace Cometa

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Builds one Dijkstra sub-transaction from independently owned values. */
class SubTransactionBuilder
{
  public:

    /** @brief Creates a builder. @param parameters Current protocol parameters. @param slots The network slot timeline. */
    SubTransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots);

    /** @brief Moves a builder. @param other The source builder. */
    SubTransactionBuilder(SubTransactionBuilder&& other) noexcept;

    /** @brief Releases builder resources. */
    ~SubTransactionBuilder();

  public:

    /** @brief Sets the transaction network ID. @param network The network ID. @return This builder. */
    SubTransactionBuilder& setNetworkId(NetworkId network);

    /** @brief Sets the earliest valid slot. @param slot The slot. @return This builder. */
    SubTransactionBuilder& setInvalidBefore(std::uint64_t slot);

    /** @brief Sets the first invalid slot. @param slot The slot. @return This builder. */
    SubTransactionBuilder& setInvalidAfter(std::uint64_t slot);

    /**
     * @brief Adds a spendable input.
     * @param utxo The output to spend.
     * @param redeemer The script redeemer, when needed.
     * @param datum The datum, when it is not in the output.
     * @return This builder.
     */
    SubTransactionBuilder& addInput(
      const Utxo&                      utxo,
      const std::optional<PlutusData>& redeemer = std::nullopt,
      const std::optional<PlutusData>& datum    = std::nullopt);

    /** @brief Adds an input with a redeemer produced from the final layout. @param utxo The output to spend. @param callback The deterministic producer. @param datum The datum, when needed. @return This builder. */
    SubTransactionBuilder& addInputWithDeferredRedeemer(
      const Utxo&                      utxo,
      DeferredRedeemer                 callback,
      const std::optional<PlutusData>& datum = std::nullopt);

    /** @brief Adds an output. @param output The output to add. @return This builder. */
    SubTransactionBuilder& addOutput(const TransactionOutput& output);

    /** @brief Adds a reference input. @param utxo The referenced output. @return This builder. */
    SubTransactionBuilder& addReferenceInput(const Utxo& utxo);

    /** @brief Adds a payment in lovelace. @param address The recipient. @param lovelace The amount. @return This builder. */
    SubTransactionBuilder& sendLovelace(const Address& address, std::uint64_t lovelace);

    /** @brief Adds a multi-asset payment. @param address The recipient. @param value The amount. @return This builder. */
    SubTransactionBuilder& sendValue(const Address& address, const Value& value);

    /** @brief Locks lovelace with a datum. @param address The script address. @param lovelace The amount. @param datum The output datum. @return This builder. */
    SubTransactionBuilder& lockLovelace(const Address& address, std::uint64_t lovelace, const Datum& datum);

    /** @brief Locks a value with a datum. @param address The script address. @param value The amount. @param datum The output datum. @return This builder. */
    SubTransactionBuilder& lockValue(const Address& address, const Value& value, const Datum& datum);

    /** @brief Mints or burns an asset. @param asset The asset identifier. @param amount The signed quantity. @param redeemer The script redeemer, when needed. @return This builder. */
    SubTransactionBuilder& mintToken(
      const AssetId&                   asset,
      std::int64_t                     amount,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Adds a required signer. @param keyHash The signer key hash. @return This builder. */
    SubTransactionBuilder& addSigner(const Blake2bHash& keyHash);

    /** @brief Adds a datum to the witness set. @param datum The datum. @return This builder. */
    SubTransactionBuilder& addDatum(const PlutusData& datum);

    /** @brief Adds a native or Plutus script. @param script The script. @return This builder. */
    SubTransactionBuilder& addScript(const Script& script);

    /** @brief Adds transaction metadata. @param tag The metadata label. @param value The metadata value. @return This builder. */
    SubTransactionBuilder& setMetadata(std::uint64_t tag, const Metadatum& value);

    /** @brief Adds a direct reward-account deposit. @param address The reward account. @param lovelace The amount. @return This builder. */
    SubTransactionBuilder& addDirectDeposit(const RewardAddress& address, std::uint64_t lovelace);

    /** @brief Adds an account balance bound. @param address The reward account. @param interval The allowed balance. @return This builder. */
    SubTransactionBuilder& addAccountBalanceInterval(const RewardAddress& address, const AccountBalanceInterval& interval);

    /** @brief Withdraws rewards from a reward account. @param address The reward account. @param lovelace The withdrawal amount. @param redeemer The script redeemer, when needed. @return This builder. */
    SubTransactionBuilder& withdrawRewards(
      const RewardAddress&             address,
      std::int64_t                     lovelace,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Withdraws rewards with a redeemer produced after balancing. @param address The reward account. @param lovelace The withdrawal amount. @param callback The deterministic producer. @return This builder. */
    SubTransactionBuilder& withdrawRewardsWithDeferredRedeemer(
      const RewardAddress& address,
      std::int64_t         lovelace,
      DeferredRedeemer     callback);

    /** @brief Adds a certificate. @param certificate The certificate. @param redeemer The script redeemer, when needed. @return This builder. */
    SubTransactionBuilder& addCertificate(
      const Certificate&               certificate,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Adds a certificate with a redeemer produced after balancing. @param certificate The certificate. @param callback The deterministic producer. @return This builder. */
    SubTransactionBuilder& addCertificateWithDeferredRedeemer(const Certificate& certificate, DeferredRedeemer callback);

    /** @brief Casts a governance vote. @param voter The voter. @param action The governance action. @param vote The choice and metadata. @param redeemer The script redeemer, when needed. @return This builder. */
    SubTransactionBuilder& vote(
      const Voter&                     voter,
      const GovernanceActionId&        action,
      const VotingProcedure&           vote,
      const std::optional<PlutusData>& redeemer = std::nullopt);

    /** @brief Casts a vote with a redeemer produced after balancing. @param voter The voter. @param action The governance action. @param vote The choice and metadata. @param callback The deterministic producer. @return This builder. */
    SubTransactionBuilder& voteWithDeferredRedeemer(
      const Voter&              voter,
      const GovernanceActionId& action,
      const VotingProcedure&    vote,
      DeferredRedeemer          callback);

    /** @brief Adds a guard. @param guard The required credential. @return This builder. */
    SubTransactionBuilder& addGuard(const Credential& guard);

    /** @brief Adds a script guard with its redeemer. @param guard The guard credential. @param redeemer The guard redeemer. @return This builder. */
    SubTransactionBuilder& addGuard(const Credential& guard, const PlutusData& redeemer);

    /**
     * @brief Requires a guard in the top-level transaction.
     * @param guard The required credential.
     * @param datum Datum for a script guard, when needed.
     * @return This builder.
     */
    SubTransactionBuilder& requireTopLevelGuard(
      const Credential&                guard,
      const std::optional<PlutusData>& datum = std::nullopt);

    /** @brief Builds one sub-transaction. @return The sub-transaction. */
    SubTransaction build();

  public:

    /** @brief Moves another builder into this instance. @param other The source. @return This builder. */
    SubTransactionBuilder& operator=(SubTransactionBuilder&& other) noexcept;

    /** @brief A builder cannot be copied. */
    SubTransactionBuilder(const SubTransactionBuilder&) = delete;

    /** @brief A builder cannot be copy assigned. */
    SubTransactionBuilder& operator=(const SubTransactionBuilder&) = delete;

  private:

    /** @brief Gets the native builder or throws after a move. @return The native builder. */
    cardano_sub_tx_builder_t* checked() const;

  private:

    /** @brief Owned native builder. */
    cardano_sub_tx_builder_t* m_value;

    /** @brief Retained redeemer callbacks and their failure state. */
    std::vector<std::unique_ptr<DeferredRedeemerContext>> m_deferredRedeemers;
};
} // namespace Cometa
