/**
 * @file SubTransactionSet.h
 *
 * @brief Ordered Dijkstra sub-transactions.
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

#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Transaction/SubTransaction.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_sub_transaction_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Sub-transactions in insertion order, unique by body hash. */
class SubTransactionSet
{
  public:

    /** @brief Creates an empty set. */
    SubTransactionSet();

    /**
     * @brief Copies a set.
     * @param other The set to copy.
     */
    SubTransactionSet(const SubTransactionSet& other);

    /**
     * @brief Moves a set.
     * @param other The set to move.
     */
    SubTransactionSet(SubTransactionSet&& other) noexcept;

    /** @brief Destroys the set. */
    ~SubTransactionSet();

  public:

    /**
     * @brief Decodes one CBOR set.
     * @param cbor The encoded set.
     * @return The decoded set.
     */
    static SubTransactionSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded set.
     */
    static SubTransactionSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the entry count.
     * @return The number of sub-transactions.
     */
    std::size_t size() const;

    /**
     * @brief Reports whether the set is empty.
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Adds a sub-transaction in order.
     * @param value The sub-transaction to copy.
     * @throws Error Another sub-transaction has the same identifier.
     */
    void add(const SubTransaction& value);

    /**
     * @brief Copies a sub-transaction by position.
     * @param index The zero-based index.
     * @return An independent sub-transaction.
     * @throws std::out_of_range The index is outside the set.
     */
    SubTransaction at(std::size_t index) const;

    /**
     * @brief Finds a sub-transaction by body hash.
     * @param id The sub-transaction identifier.
     * @return An independent sub-transaction, when found.
     */
    std::optional<SubTransaction> findById(const Blake2bHash& id) const;

    /**
     * @brief Reports whether the CBOR set tag is included.
     * @return True when tagged.
     */
    bool useTag() const;

    /**
     * @brief Selects whether to include the CBOR set tag.
     * @param value True to include the tag.
     */
    void setUseTag(bool value);

    /**
     * @brief Encodes the set as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the set as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges sets.
     * @param other The set to exchange.
     */
    void swap(SubTransactionSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The set to copy.
     * @return This set.
     */
    SubTransactionSet& operator=(const SubTransactionSet& other);

    /**
     * @brief Assigns a moved set.
     * @param other The set to move.
     * @return This set.
     */
    SubTransactionSet& operator=(SubTransactionSet&& other) noexcept;

    /**
     * @brief Compares sets.
     * @param other The set to compare.
     * @return True when equal.
     */
    bool operator==(const SubTransactionSet& other) const;

    /**
     * @brief Compares sets.
     * @param other The set to compare.
     * @return True when different.
     */
    bool operator!=(const SubTransactionSet& other) const;

  private:

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /**
     * @brief Adopts an owned set.
     * @param value The owned set.
     */
    explicit SubTransactionSet(cardano_sub_transaction_set_t* value) noexcept;

    /**
     * @brief Gets the stored set.
     * @return The borrowed set.
     */
    cardano_sub_transaction_set_t* checked() const;

  private:

    /** @brief The owned set. */
    cardano_sub_transaction_set_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges sets through argument-dependent lookup.
 * @param lhs The first set.
 * @param rhs The second set.
 */
void swap(SubTransactionSet& lhs, SubTransactionSet& rhs) noexcept;
} // namespace Cometa
