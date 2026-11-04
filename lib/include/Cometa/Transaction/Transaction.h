/**
 * @file Transaction.h
 *
 * @brief A complete transaction containing its body, witnesses, validity flag, and optional auxiliary data.
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

#include <Cometa/WitnessSet/RedeemerTag.h>

#include <Cometa/VotingProcedures/Voter.h>

#include <Cometa/TransactionBody/TransactionInput.h>

#include <Cometa/Crypto/Blake2bHashSet.h>

#include <Cometa/Common/UtxoList.h>

#include <Cometa/Certificates/Certificate.h>

#include <Cometa/Address/RewardAddress.h>

#include <Cometa/Address/Address.h>

#include <Cometa/AuxiliaryData/AuxiliaryData.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/TransactionBody/TransactionBody.h>
#include <Cometa/WitnessSet/VkeyWitnessSet.h>
#include <Cometa/WitnessSet/WitnessSet.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_transaction_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A complete transaction containing its body, witnesses, validity flag, and optional auxiliary data.
 *
 * The transaction ID is the body hash. Add signatures after finalizing the body; changing body fields
 * invalidates existing signatures. Serialization alone does not establish ledger validity.
 */
class Transaction
{
  public:

    /**
     * @brief Creates a transaction.
     * @param body The body to copy.
     * @param witnesses The witnesses to copy.
     * @param auxiliaryData Optional auxiliary data to copy.
     */
    Transaction(const TransactionBody& body, const WitnessSet& witnesses, const std::optional<AuxiliaryData>& auxiliaryData = std::nullopt);

    /**
     * @brief Copies a transaction.
     * @param other The transaction to copy.
     */
    Transaction(const Transaction& other);

    /**
     * @brief Moves a transaction.
     * @param other The transaction to move.
     */
    Transaction(Transaction&& other) noexcept;

    /** @brief Destroys the transaction. */
    ~Transaction();

  public:

    /**
     * @brief Decodes one CBOR transaction.
     * @param cbor The encoded transaction.
     * @return The decoded transaction.
     */
    static Transaction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded transaction.
     */
    static Transaction fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the transaction body.
     * @return An independent body.
     */
    TransactionBody body() const;

    /**
     * @brief Replaces the transaction body.
     *
     * Replacing the body changes the signing payload. Existing witnesses are not automatically re-signed.
     *
     * @param value The body to copy.
     */
    void setBody(const TransactionBody& value);

    /**
     * @brief Gets the witness set.
     * @return An independent witness set.
     */
    WitnessSet witnessSet() const;

    /**
     * @brief Replaces the witness set.
     * @param value The witnesses to copy.
     */
    void setWitnessSet(const WitnessSet& value);

    /**
     * @brief Gets optional auxiliary data.
     * @return An independent value, when present.
     */
    std::optional<AuxiliaryData> auxiliaryData() const;

    /**
     * @brief Replaces or clears auxiliary data.
     *
     * The body's auxiliary-data hash must match these contents. Finalize the data and its commitment
     * before collecting signatures.
     *
     * @param value The data to copy, or no value to clear it.
     */
    void setAuxiliaryData(const std::optional<AuxiliaryData>& value);

    /**
     * @brief Gets the transaction identifier.
     * @return The 32-byte BLAKE2b hash of the body, excluding witnesses.
     */
    Blake2bHash id() const;

    /**
     * @brief Adds verification-key witnesses.
     *
     * Applies collected key witnesses without changing the transaction body. The signatures must have been
     * produced for this body hash.
     *
     * @param witnesses The witnesses to copy.
     */
    void applyVkeyWitnesses(const VkeyWitnessSet& witnesses);

    /**
     * @brief Reads the declared phase-two script-validation outcome.
     * @return True when the transaction declares script success; this method does not evaluate scripts.
     */
    bool isValid() const;

    /**
     * @brief Changes the validity flag.
     *
     * Declares the expected script outcome for ledger processing. It does not execute scripts or validate
     * the transaction. A false outcome affects collateral processing under the applicable ledger rules.
     *
     * @param value The new validity flag.
     */
    void setValid(bool value);

    /**
     * @brief Reports whether the transaction carries script data.
     * @return True when script data is present.
     */
    bool hasScriptData() const;

    /**
     * @brief Finds all required signing key hashes.
     * @param resolvedInputs Resolved inputs used to determine payment credentials.
     * @return Unique signing key hashes.
     */
    Blake2bHashSet uniqueSigners(const UtxoList& resolvedInputs = {}) const;

    /**
     * @brief Finds the input position in ledger order.
     * @param input The input to find.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findInputIndex(const TransactionInput& input) const;

    /**
     * @brief Finds the reference input position in ledger order.
     * @param input The reference input to find.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findReferenceInputIndex(const TransactionInput& input) const;

    /**
     * @brief Finds the output position in ledger order.
     * @param address The output address.
     * @param minLovelace Minimum lovelace to match.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findOutputIndex(const Address& address, std::uint64_t minLovelace) const;

    /**
     * @brief Finds the minting policy position in ledger order.
     * @param policyId The minting policy hash.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findMintPolicyIndex(const Blake2bHash& policyId) const;

    /**
     * @brief Finds the withdrawal position in ledger order.
     * @param address The reward address.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findWithdrawalIndex(const RewardAddress& address) const;

    /**
     * @brief Finds the certificate position in ledger order.
     * @param certificate The certificate to find.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findCertificateIndex(const Certificate& certificate) const;

    /**
     * @brief Finds the vote position in ledger order.
     * @param voter The voter to find.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findVoteIndex(const Voter& voter) const;

    /**
     * @brief Finds the redeemer position in ledger order.
     * @param tag The redeemer purpose.
     * @param purposeIndex The index within that purpose.
     * @return The zero-based position, or std::nullopt when no matching component exists.
     */
    std::optional<std::uint64_t> findRedeemerIndex(RedeemerTag tag, std::uint64_t purposeIndex) const;

    /**
     * @brief Encodes the transaction as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the transaction as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges transactions.
     * @param other The transaction to exchange.
     */
    void swap(Transaction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The transaction to copy.
     * @return This transaction.
     */
    Transaction& operator=(const Transaction& other);

    /**
     * @brief Assigns a moved transaction.
     * @param other The transaction to move.
     * @return This transaction.
     */
    Transaction& operator=(Transaction&& other) noexcept;

    /**
     * @brief Compares transactions.
     * @param other The transaction to compare.
     * @return True when equal.
     */
    bool operator==(const Transaction& other) const;

    /**
     * @brief Compares transactions.
     * @param other The transaction to compare.
     * @return True when different.
     */
    bool operator!=(const Transaction& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Transaction adopt(cardano_transaction_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_transaction_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_transaction_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_transaction_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned transaction.
     * @param value The owned transaction.
     */
    explicit Transaction(cardano_transaction_t* value) noexcept;

    /**
     * @brief Gets the stored transaction.
     * @return The borrowed transaction.
     */
    cardano_transaction_t* checked() const;

  private:

    /** @brief The owned transaction. */
    cardano_transaction_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges transactions through argument-dependent lookup.
 * @param lhs The first transaction.
 * @param rhs The second transaction.
 */
void swap(Transaction& lhs, Transaction& rhs) noexcept;
} // namespace Cometa
