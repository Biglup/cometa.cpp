/**
 * @file TransactionBody.h
 *
 * @brief The signed ledger instructions of a top-level transaction.
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

#include <Cometa/Assets/MultiAsset.h>
#include <Cometa/Certificates/CertificateSet.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Common/WithdrawalMap.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Crypto/Blake2bHashSet.h>
#include <Cometa/ProposalProcedures/ProposalProcedureSet.h>
#include <Cometa/ProtocolParams/Update.h>
#include <Cometa/Transaction/SubTransactionSet.h>
#include <Cometa/TransactionBody/AccountBalanceIntervalsMap.h>
#include <Cometa/TransactionBody/DirectDepositMap.h>
#include <Cometa/TransactionBody/GuardSet.h>
#include <Cometa/TransactionBody/RequiredGuardsMap.h>
#include <Cometa/TransactionBody/TransactionInputSet.h>
#include <Cometa/TransactionBody/TransactionOutput.h>
#include <Cometa/TransactionBody/TransactionOutputList.h>
#include <Cometa/VotingProcedures/VotingProcedures.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_transaction_body_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief The signed ledger instructions of a top-level transaction.
 *
 * Contains inputs, outputs, fees, validity limits, and optional staking, governance, and Dijkstra
 * fields. Witnesses are separate. Changing the body changes its hash and invalidates existing
 * transaction signatures.
 */
class TransactionBody
{
  public:

    /**
     * @brief Creates a transaction body.
     * @param inputs The inputs to copy.
     * @param outputs The outputs to copy.
     * @param fee The transaction fee in lovelace.
     * @param invalidAfter The optional exclusive ending slot.
     */
    TransactionBody(const TransactionInputSet& inputs, const TransactionOutputList& outputs, std::uint64_t fee, std::optional<std::uint64_t> invalidAfter = std::nullopt);

    /**
     * @brief Copies a body.
     * @param other The body to copy.
     */
    TransactionBody(const TransactionBody& other);

    /**
     * @brief Moves a body.
     * @param other The body to move.
     */
    TransactionBody(TransactionBody&& other) noexcept;

    /** @brief Destroys the body. */
    ~TransactionBody();

  public:

    /**
     * @brief Decodes one CBOR body.
     * @param cbor The encoded body.
     * @return The decoded body.
     */
    static TransactionBody fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded body.
     */
    static TransactionBody fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the outputs consumed by the transaction.
     * @return The input references, without their resolved output data.
     */
    TransactionInputSet inputs() const;

    /**
     * @brief Sets the outputs consumed by the transaction.
     * @param value The input references, without their resolved output data.
     */
    void setInputs(const TransactionInputSet& value);

    /**
     * @brief Gets the outputs created by the transaction.
     * @return The outputs in ledger order.
     */
    TransactionOutputList outputs() const;

    /**
     * @brief Sets the outputs created by the transaction.
     * @param value The outputs in ledger order.
     */
    void setOutputs(const TransactionOutputList& value);

    /**
     * @brief Gets the staking, pool, and governance certificates.
     * @return The certificates in transaction order; std::nullopt if the field is absent.
     */
    std::optional<CertificateSet> certificates() const;

    /**
     * @brief Sets the staking, pool, and governance certificates.
     * @param value The certificates in transaction order. Use std::nullopt to remove the field.
     */
    void setCertificates(const std::optional<CertificateSet>& value);

    /**
     * @brief Gets the reward-account withdrawals.
     * @return Reward accounts and withdrawal amounts in lovelace; std::nullopt if the field is absent.
     */
    std::optional<WithdrawalMap> withdrawals() const;

    /**
     * @brief Sets the reward-account withdrawals.
     * @param value Reward accounts and withdrawal amounts in lovelace. Use std::nullopt to remove the field.
     */
    void setWithdrawals(const std::optional<WithdrawalMap>& value);

    /**
     * @brief Gets the auxiliary-data commitment.
     * @return The 32-byte hash binding metadata and auxiliary scripts to the body; std::nullopt if the field is absent.
     */
    std::optional<Blake2bHash> auxDataHash() const;

    /**
     * @brief Sets the auxiliary-data commitment.
     * @param value The 32-byte hash binding metadata and auxiliary scripts to the body. Use std::nullopt to remove the field.
     */
    void setAuxDataHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Gets the native-token mint and burn quantities.
     * @return Policy-grouped token quantities; positive values mint and negative values burn; std::nullopt if the field is absent.
     */
    std::optional<MultiAsset> mint() const;

    /**
     * @brief Sets the native-token mint and burn quantities.
     * @param value Policy-grouped token quantities; positive values mint and negative values burn. Use std::nullopt to remove the field.
     */
    void setMint(const std::optional<MultiAsset>& value);

    /**
     * @brief Gets the script-data commitment.
     * @return The hash committing to redeemers, datums, and the required language views; std::nullopt if the field is absent.
     */
    std::optional<Blake2bHash> scriptDataHash() const;

    /**
     * @brief Sets the script-data commitment.
     * @param value The hash committing to redeemers, datums, and the required language views. Use std::nullopt to remove the field.
     */
    void setScriptDataHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Gets the Dijkstra authorization guards.
     * @return The guard credentials in redeemer-index order; std::nullopt if the field is absent.
     */
    std::optional<GuardSet> guards() const;

    /**
     * @brief Sets the Dijkstra authorization guards.
     * @param value The guard credentials in redeemer-index order. Use std::nullopt to remove the field.
     */
    void setGuards(const std::optional<GuardSet>& value);

    /**
     * @brief Gets the outputs read without being spent.
     * @return The references that make output data and reference scripts available; std::nullopt if the field is absent.
     */
    std::optional<TransactionInputSet> referenceInputs() const;

    /**
     * @brief Sets the outputs read without being spent.
     * @param value The references that make output data and reference scripts available. Use std::nullopt to remove the field.
     */
    void setReferenceInputs(const std::optional<TransactionInputSet>& value);

    /**
     * @brief Gets the governance votes.
     * @return The votes grouped by voter and governance action; std::nullopt if the field is absent.
     */
    std::optional<VotingProcedures> votingProcedures() const;

    /**
     * @brief Sets the governance votes.
     * @param value The votes grouped by voter and governance action. Use std::nullopt to remove the field.
     */
    void setVotingProcedures(const std::optional<VotingProcedures>& value);

    /**
     * @brief Gets the governance proposals.
     * @return The proposals in action-index order; std::nullopt if the field is absent.
     */
    std::optional<ProposalProcedureSet> proposalProcedures() const;

    /**
     * @brief Sets the governance proposals.
     * @param value The proposals in action-index order. Use std::nullopt to remove the field.
     */
    void setProposalProcedures(const std::optional<ProposalProcedureSet>& value);

    /**
     * @brief Gets the required top-level authorizations.
     * @return Guard credentials and their optional associated data; std::nullopt if the field is absent.
     */
    std::optional<RequiredGuardsMap> requiredTopLevelGuards() const;

    /**
     * @brief Sets the required top-level authorizations.
     * @param value Guard credentials and their optional associated data. Use std::nullopt to remove the field.
     */
    void setRequiredTopLevelGuards(const std::optional<RequiredGuardsMap>& value);

    /**
     * @brief Gets the direct reward-account deposits.
     * @return Reward accounts and deposit amounts in lovelace; std::nullopt if the field is absent.
     */
    std::optional<DirectDepositMap> directDeposits() const;

    /**
     * @brief Sets the direct reward-account deposits.
     * @param value Reward accounts and deposit amounts in lovelace. Use std::nullopt to remove the field.
     */
    void setDirectDeposits(const std::optional<DirectDepositMap>& value);

    /**
     * @brief Gets the required account balance intervals.
     * @return Reward accounts and their permitted balances in lovelace; std::nullopt if the field is absent.
     */
    std::optional<AccountBalanceIntervalsMap> accountBalanceIntervals() const;

    /**
     * @brief Sets the required account balance intervals.
     * @param value Reward accounts and their permitted balances in lovelace. Use std::nullopt to remove the field.
     */
    void setAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value);

    /**
     * @brief Gets the exclusive upper validity slot.
     * @return The first slot at which the transaction is no longer valid; std::nullopt if the field is absent.
     */
    std::optional<std::uint64_t> invalidAfter() const;

    /**
     * @brief Sets the exclusive upper validity slot.
     * @param value The first slot at which the transaction is no longer valid. Use std::nullopt to remove the field.
     */
    void setInvalidAfter(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the inclusive lower validity slot.
     * @return The earliest slot at which the transaction may be valid; std::nullopt if the field is absent.
     */
    std::optional<std::uint64_t> invalidBefore() const;

    /**
     * @brief Sets the inclusive lower validity slot.
     * @param value The earliest slot at which the transaction may be valid. Use std::nullopt to remove the field.
     */
    void setInvalidBefore(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the explicitly declared address network.
     * @return The mainnet or testnet identifier; this is not network magic; std::nullopt if the field is absent.
     */
    std::optional<NetworkId> networkId() const;

    /**
     * @brief Sets the explicitly declared address network.
     * @param value The mainnet or testnet identifier; this is not network magic. Use std::nullopt to remove the field.
     */
    void setNetworkId(std::optional<NetworkId> value);

    /**
     * @brief Gets the declared current treasury value.
     * @return The treasury amount in lovelace against which the ledger checks the transaction; std::nullopt if the field is absent.
     */
    std::optional<std::uint64_t> treasuryValue() const;

    /**
     * @brief Sets the declared current treasury value.
     * @param value The treasury amount in lovelace against which the ledger checks the transaction. Use std::nullopt to remove the field.
     */
    void setTreasuryValue(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the treasury donation.
     * @return The amount donated to the treasury in lovelace; std::nullopt if the field is absent.
     */
    std::optional<std::uint64_t> donation() const;

    /**
     * @brief Sets the treasury donation.
     * @param value The amount donated to the treasury in lovelace. Use std::nullopt to remove the field.
     */
    void setDonation(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the legacy protocol parameter update.
     * @return The historical epoch-based update proposal; std::nullopt if the field is absent.
     */
    std::optional<Update> update() const;

    /**
     * @brief Sets the legacy protocol parameter update.
     * @param value The historical epoch-based update proposal. Use std::nullopt to remove the field.
     */
    void setUpdate(const std::optional<Update>& value);

    /**
     * @brief Gets the collateral input references.
     * @return The inputs available to cover script failure; std::nullopt if the field is absent.
     */
    std::optional<TransactionInputSet> collateral() const;

    /**
     * @brief Sets the collateral input references.
     * @param value The inputs available to cover script failure. Use std::nullopt to remove the field.
     */
    void setCollateral(const std::optional<TransactionInputSet>& value);

    /**
     * @brief Gets the required verification-key hashes.
     * @return The keys whose signatures must be supplied with the transaction; std::nullopt if the field is absent.
     */
    std::optional<Blake2bHashSet> requiredSigners() const;

    /**
     * @brief Sets the required verification-key hashes.
     * @param value The keys whose signatures must be supplied with the transaction. Use std::nullopt to remove the field.
     */
    void setRequiredSigners(const std::optional<Blake2bHashSet>& value);

    /**
     * @brief Gets the collateral return output.
     * @return The output returning collateral value beyond the total collateral charged; std::nullopt if the field is absent.
     */
    std::optional<TransactionOutput> collateralReturn() const;

    /**
     * @brief Sets the collateral return output.
     * @param value The output returning collateral value beyond the total collateral charged. Use std::nullopt to remove the field.
     */
    void setCollateralReturn(const std::optional<TransactionOutput>& value);

    /**
     * @brief Gets the Dijkstra sub-transaction batch.
     * @return The signed intents in batch order; std::nullopt if the field is absent.
     */
    std::optional<SubTransactionSet> subTransactions() const;

    /**
     * @brief Sets the Dijkstra sub-transaction batch.
     * @param value The signed intents in batch order. Use std::nullopt to remove the field.
     */
    void setSubTransactions(const std::optional<SubTransactionSet>& value);

    /**
     * @brief Gets the initial account balance constraints.
     * @return The account balances required at the start of the top-level transaction; std::nullopt if the field is absent.
     */
    std::optional<AccountBalanceIntervalsMap> startingAccountBalanceIntervals() const;

    /**
     * @brief Sets the initial account balance constraints.
     * @param value The account balances required at the start of the top-level transaction. Use std::nullopt to remove the field.
     */
    void setStartingAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value);

    /**
     * @brief Gets the transaction fee.
     * @return The fee in lovelace.
     */
    std::uint64_t fee() const;

    /**
     * @brief Sets the transaction fee.
     * @param value The fee in lovelace.
     */
    void setFee(std::uint64_t value);

    /**
     * @brief Gets the total collateral charged on script failure.
     * @return The collateral amount in lovelace; std::nullopt if the field is absent.
     */
    std::optional<std::uint64_t> totalCollateral() const;

    /**
     * @brief Sets the total collateral charged on script failure.
     * @param value The collateral amount in lovelace. Use std::nullopt to remove the field.
     */
    void setTotalCollateral(std::optional<std::uint64_t> value);

    /**
     * @brief Reports whether the body uses tagged sets.
     * @return True when sets use their CBOR tags.
     */
    bool hasTaggedSets() const;

    /**
     * @brief Computes the transaction body identifier.
     * @return The 32-byte BLAKE2b hash of the serialized body.
     */
    Blake2bHash hash() const;

    /**
     * @brief Encodes the body as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the body as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges bodies.
     * @param other The body to exchange.
     */
    void swap(TransactionBody& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The body to copy.
     * @return This body.
     */
    TransactionBody& operator=(const TransactionBody& other);

    /**
     * @brief Assigns a moved body.
     * @param other The body to move.
     * @return This body.
     */
    TransactionBody& operator=(TransactionBody&& other) noexcept;

    /**
     * @brief Compares bodies.
     * @param other The body to compare.
     * @return True when equal.
     */
    bool operator==(const TransactionBody& other) const;

    /**
     * @brief Compares bodies.
     * @param other The body to compare.
     * @return True when different.
     */
    bool operator!=(const TransactionBody& other) const;

  private:

    /** @brief Allows transactions to copy this value. */
    friend class Transaction;

    /**
     * @brief Adopts an owned body.
     * @param value The owned body.
     */
    explicit TransactionBody(cardano_transaction_body_t* value) noexcept;

    /**
     * @brief Gets the stored body.
     * @return The borrowed body.
     */
    cardano_transaction_body_t* checked() const;

  private:

    /** @brief The owned body. */
    cardano_transaction_body_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges bodies through argument-dependent lookup.
 * @param lhs The first body.
 * @param rhs The second body.
 */
void swap(TransactionBody& lhs, TransactionBody& rhs) noexcept;
} // namespace Cometa
