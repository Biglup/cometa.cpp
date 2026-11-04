/**
 * @file SubTransactionBody.h
 *
 * @brief The signed instructions of a Dijkstra sub-transaction.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Assets/MultiAsset.h>
#include <Cometa/Certificates/CertificateSet.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Common/WithdrawalMap.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/ProposalProcedures/ProposalProcedureSet.h>
#include <Cometa/TransactionBody/AccountBalanceIntervalsMap.h>
#include <Cometa/TransactionBody/DirectDepositMap.h>
#include <Cometa/TransactionBody/GuardSet.h>
#include <Cometa/TransactionBody/RequiredGuardsMap.h>
#include <Cometa/TransactionBody/TransactionInputSet.h>
#include <Cometa/TransactionBody/TransactionOutputList.h>
#include <Cometa/VotingProcedures/VotingProcedures.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_sub_transaction_body_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief The signed instructions of a Dijkstra sub-transaction.
 *
 * Describes the intent's inputs, outputs, and other requirements. The enclosing transaction supplies
 * the top-level fee and batching context; required top-level guards constrain that context.
 */
class SubTransactionBody
{
  public:

    /**
     * @brief Creates a sub-transaction body.
     * @param inputs The inputs to copy.
     * @param outputs The outputs to copy.
     * @param invalidAfter The optional exclusive ending slot.
     */
    SubTransactionBody(const TransactionInputSet& inputs, const TransactionOutputList& outputs, std::optional<std::uint64_t> invalidAfter = std::nullopt);

    /**
     * @brief Copies a body.
     * @param other The body to copy.
     */
    SubTransactionBody(const SubTransactionBody& other);

    /**
     * @brief Moves a body.
     * @param other The body to move.
     */
    SubTransactionBody(SubTransactionBody&& other) noexcept;

    /** @brief Destroys the body. */
    ~SubTransactionBody();

  public:

    /**
     * @brief Decodes one CBOR body.
     * @param cbor The encoded body.
     * @return The decoded body.
     */
    static SubTransactionBody fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded body.
     */
    static SubTransactionBody fromCbor(std::string_view cborHex);

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
    void swap(SubTransactionBody& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The body to copy.
     * @return This body.
     */
    SubTransactionBody& operator=(const SubTransactionBody& other);

    /**
     * @brief Assigns a moved body.
     * @param other The body to move.
     * @return This body.
     */
    SubTransactionBody& operator=(SubTransactionBody&& other) noexcept;

    /**
     * @brief Compares bodies.
     * @param other The body to compare.
     * @return True when equal.
     */
    bool operator==(const SubTransactionBody& other) const;

    /**
     * @brief Compares bodies.
     * @param other The body to compare.
     * @return True when different.
     */
    bool operator!=(const SubTransactionBody& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static SubTransactionBody adopt(cardano_sub_transaction_body_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_sub_transaction_body_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_sub_transaction_body_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_sub_transaction_body_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned body.
     * @param value The owned body.
     */
    explicit SubTransactionBody(cardano_sub_transaction_body_t* value) noexcept;

    /**
     * @brief Gets the stored body.
     * @return The borrowed body.
     */
    cardano_sub_transaction_body_t* checked() const;

  private:

    /** @brief The owned body. */
    cardano_sub_transaction_body_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges bodies through argument-dependent lookup.
 * @param lhs The first body.
 * @param rhs The second body.
 */
void swap(SubTransactionBody& lhs, SubTransactionBody& rhs) noexcept;
} // namespace Cometa
