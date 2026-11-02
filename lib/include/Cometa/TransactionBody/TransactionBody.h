/**
 * @file TransactionBody.h
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
/** @brief Fields of a top-level transaction. */
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
     * @brief Gets the inputs field.
     * @return An independent value.
     */
    TransactionInputSet inputs() const;

    /**
     * @brief Replaces the inputs field.
     * @param value The value to copy.
     */
    void setInputs(const TransactionInputSet& value);

    /**
     * @brief Gets the outputs field.
     * @return An independent value.
     */
    TransactionOutputList outputs() const;

    /**
     * @brief Replaces the outputs field.
     * @param value The value to copy.
     */
    void setOutputs(const TransactionOutputList& value);

    /**
     * @brief Gets the certificates field.
     * @return An independent value, when present.
     */
    std::optional<CertificateSet> certificates() const;

    /**
     * @brief Replaces or clears the certificates field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setCertificates(const std::optional<CertificateSet>& value);

    /**
     * @brief Gets the withdrawals field.
     * @return An independent value, when present.
     */
    std::optional<WithdrawalMap> withdrawals() const;

    /**
     * @brief Replaces or clears the withdrawals field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setWithdrawals(const std::optional<WithdrawalMap>& value);

    /**
     * @brief Gets the auxDataHash field.
     * @return An independent value, when present.
     */
    std::optional<Blake2bHash> auxDataHash() const;

    /**
     * @brief Replaces or clears the auxDataHash field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setAuxDataHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Gets the mint field.
     * @return An independent value, when present.
     */
    std::optional<MultiAsset> mint() const;

    /**
     * @brief Replaces or clears the mint field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setMint(const std::optional<MultiAsset>& value);

    /**
     * @brief Gets the scriptDataHash field.
     * @return An independent value, when present.
     */
    std::optional<Blake2bHash> scriptDataHash() const;

    /**
     * @brief Replaces or clears the scriptDataHash field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setScriptDataHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Gets the guards field.
     * @return An independent value, when present.
     */
    std::optional<GuardSet> guards() const;

    /**
     * @brief Replaces or clears the guards field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setGuards(const std::optional<GuardSet>& value);

    /**
     * @brief Gets the referenceInputs field.
     * @return An independent value, when present.
     */
    std::optional<TransactionInputSet> referenceInputs() const;

    /**
     * @brief Replaces or clears the referenceInputs field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setReferenceInputs(const std::optional<TransactionInputSet>& value);

    /**
     * @brief Gets the votingProcedures field.
     * @return An independent value, when present.
     */
    std::optional<VotingProcedures> votingProcedures() const;

    /**
     * @brief Replaces or clears the votingProcedures field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setVotingProcedures(const std::optional<VotingProcedures>& value);

    /**
     * @brief Gets the proposalProcedures field.
     * @return An independent value, when present.
     */
    std::optional<ProposalProcedureSet> proposalProcedures() const;

    /**
     * @brief Replaces or clears the proposalProcedures field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setProposalProcedures(const std::optional<ProposalProcedureSet>& value);

    /**
     * @brief Gets the requiredTopLevelGuards field.
     * @return An independent value, when present.
     */
    std::optional<RequiredGuardsMap> requiredTopLevelGuards() const;

    /**
     * @brief Replaces or clears the requiredTopLevelGuards field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setRequiredTopLevelGuards(const std::optional<RequiredGuardsMap>& value);

    /**
     * @brief Gets the directDeposits field.
     * @return An independent value, when present.
     */
    std::optional<DirectDepositMap> directDeposits() const;

    /**
     * @brief Replaces or clears the directDeposits field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setDirectDeposits(const std::optional<DirectDepositMap>& value);

    /**
     * @brief Gets the accountBalanceIntervals field.
     * @return An independent value, when present.
     */
    std::optional<AccountBalanceIntervalsMap> accountBalanceIntervals() const;

    /**
     * @brief Replaces or clears the accountBalanceIntervals field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value);

    /**
     * @brief Gets the invalidAfter field.
     * @return The value, when present.
     */
    std::optional<std::uint64_t> invalidAfter() const;

    /**
     * @brief Sets or clears the invalidAfter field.
     * @param value The value to set, or no value to clear it.
     */
    void setInvalidAfter(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the invalidBefore field.
     * @return The value, when present.
     */
    std::optional<std::uint64_t> invalidBefore() const;

    /**
     * @brief Sets or clears the invalidBefore field.
     * @param value The value to set, or no value to clear it.
     */
    void setInvalidBefore(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the networkId field.
     * @return The value, when present.
     */
    std::optional<NetworkId> networkId() const;

    /**
     * @brief Sets or clears the networkId field.
     * @param value The value to set, or no value to clear it.
     */
    void setNetworkId(std::optional<NetworkId> value);

    /**
     * @brief Gets the treasuryValue field.
     * @return The value, when present.
     */
    std::optional<std::uint64_t> treasuryValue() const;

    /**
     * @brief Sets or clears the treasuryValue field.
     * @param value The value to set, or no value to clear it.
     */
    void setTreasuryValue(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the donation field.
     * @return The value, when present.
     */
    std::optional<std::uint64_t> donation() const;

    /**
     * @brief Sets or clears the donation field.
     * @param value The value to set, or no value to clear it.
     */
    void setDonation(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the update field.
     * @return An independent value, when present.
     */
    std::optional<Update> update() const;

    /**
     * @brief Replaces or clears the update field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setUpdate(const std::optional<Update>& value);

    /**
     * @brief Gets the collateral field.
     * @return An independent value, when present.
     */
    std::optional<TransactionInputSet> collateral() const;

    /**
     * @brief Replaces or clears the collateral field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setCollateral(const std::optional<TransactionInputSet>& value);

    /**
     * @brief Gets the required signers field.
     * @return An independent value, when present.
     */
    std::optional<Blake2bHashSet> requiredSigners() const;

    /**
     * @brief Replaces or clears the required signers field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setRequiredSigners(const std::optional<Blake2bHashSet>& value);

    /**
     * @brief Gets the collateral return field.
     * @return An independent value, when present.
     */
    std::optional<TransactionOutput> collateralReturn() const;

    /**
     * @brief Replaces or clears the collateral return field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setCollateralReturn(const std::optional<TransactionOutput>& value);

    /**
     * @brief Gets the sub transactions field.
     * @return An independent value, when present.
     */
    std::optional<SubTransactionSet> subTransactions() const;

    /**
     * @brief Replaces or clears the sub transactions field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setSubTransactions(const std::optional<SubTransactionSet>& value);

    /**
     * @brief Gets the starting account balance intervals field.
     * @return An independent value, when present.
     */
    std::optional<AccountBalanceIntervalsMap> startingAccountBalanceIntervals() const;

    /**
     * @brief Replaces or clears the starting account balance intervals field.
     * @param value The value to copy, or no value to clear the field.
     */
    void setStartingAccountBalanceIntervals(const std::optional<AccountBalanceIntervalsMap>& value);

    /**
     * @brief Gets the transaction fee.
     * @return The fee in lovelace.
     */
    std::uint64_t fee() const;

    /**
     * @brief Replaces the transaction fee.
     * @param value The fee in lovelace.
     */
    void setFee(std::uint64_t value);

    /**
     * @brief Gets total collateral.
     * @return The lovelace amount, when present.
     */
    std::optional<std::uint64_t> totalCollateral() const;

    /**
     * @brief Replaces or clears total collateral.
     * @param value The lovelace amount, or no value to clear the field.
     */
    void setTotalCollateral(std::optional<std::uint64_t> value);

    /**
     * @brief Reports whether the body uses tagged sets.
     * @return True when sets use their CBOR tags.
     */
    bool hasTaggedSets() const;

    /**
     * @brief Computes the transaction body hash.
     * @return The 32-byte hash.
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
