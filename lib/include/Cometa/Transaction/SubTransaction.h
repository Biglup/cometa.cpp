/**
 * @file SubTransaction.h
 *
 * @brief A Dijkstra transaction intent with its own body, witnesses, and optional auxiliary data.
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

#include <Cometa/AuxiliaryData/AuxiliaryData.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/TransactionBody/SubTransactionBody.h>
#include <Cometa/WitnessSet/VkeyWitnessSet.h>
#include <Cometa/WitnessSet/WitnessSet.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_sub_transaction_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A Dijkstra transaction intent with its own body, witnesses, and optional auxiliary data.
 *
 * Sign the intent independently, then include it in a top-level transaction for submission. Top-level
 * guards express authorization required from that enclosing transaction.
 */
class SubTransaction
{
  public:

    /**
     * @brief Creates a sub-transaction.
     * @param body The body to copy.
     * @param witnesses The witnesses to copy.
     * @param auxiliaryData Optional auxiliary data to copy.
     */
    SubTransaction(const SubTransactionBody& body, const WitnessSet& witnesses, const std::optional<AuxiliaryData>& auxiliaryData = std::nullopt);

    /**
     * @brief Copies a sub-transaction.
     * @param other The sub-transaction to copy.
     */
    SubTransaction(const SubTransaction& other);

    /**
     * @brief Moves a sub-transaction.
     * @param other The sub-transaction to move.
     */
    SubTransaction(SubTransaction&& other) noexcept;

    /** @brief Destroys the sub-transaction. */
    ~SubTransaction();

  public:

    /**
     * @brief Decodes one CBOR sub-transaction.
     * @param cbor The encoded sub-transaction.
     * @return The decoded sub-transaction.
     */
    static SubTransaction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded sub-transaction.
     */
    static SubTransaction fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the sub-transaction body.
     * @return An independent body.
     */
    SubTransactionBody body() const;

    /**
     * @brief Replaces the sub-transaction body.
     * @param value The body to copy.
     */
    void setBody(const SubTransactionBody& value);

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
     * @brief Gets the sub-transaction identifier.
     * @return The 32-byte identifier.
     */
    Blake2bHash id() const;

    /**
     * @brief Adds verification-key witnesses.
     * @param witnesses The witnesses to copy.
     */
    void applyVkeyWitnesses(const VkeyWitnessSet& witnesses);

    /**
     * @brief Encodes the sub-transaction as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the sub-transaction as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges sub-transactions.
     * @param other The sub-transaction to exchange.
     */
    void swap(SubTransaction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The sub-transaction to copy.
     * @return This sub-transaction.
     */
    SubTransaction& operator=(const SubTransaction& other);

    /**
     * @brief Assigns a moved sub-transaction.
     * @param other The sub-transaction to move.
     * @return This sub-transaction.
     */
    SubTransaction& operator=(SubTransaction&& other) noexcept;

    /**
     * @brief Compares sub-transactions.
     * @param other The sub-transaction to compare.
     * @return True when equal.
     */
    bool operator==(const SubTransaction& other) const;

    /**
     * @brief Compares sub-transactions.
     * @param other The sub-transaction to compare.
     * @return True when different.
     */
    bool operator!=(const SubTransaction& other) const;

  private:

    /** @brief Allows sub-transaction sets to copy this value. */
    friend class SubTransactionSet;

    /**
     * @brief Adopts an owned sub-transaction.
     * @param value The owned sub-transaction.
     */
    explicit SubTransaction(cardano_sub_transaction_t* value) noexcept;

    /**
     * @brief Gets the stored sub-transaction.
     * @return The borrowed sub-transaction.
     */
    cardano_sub_transaction_t* checked() const;

  private:

    /** @brief The owned sub-transaction. */
    cardano_sub_transaction_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges sub-transactions through argument-dependent lookup.
 * @param lhs The first sub-transaction.
 * @param rhs The second sub-transaction.
 */
void swap(SubTransaction& lhs, SubTransaction& rhs) noexcept;
} // namespace Cometa
