/**
 * @file RedeemerList.h
 *
 * @brief Ledger redeemer collections.
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

#include <Cometa/WitnessSet/Redeemer.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_redeemer_list_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A collection of ledger redeemers in purpose and index order.
 */
class RedeemerList
{
  public:

    /** @brief Creates an empty redeemer collection. */
    RedeemerList();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    RedeemerList(const RedeemerList& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    RedeemerList(RedeemerList&& other) noexcept;

    /** @brief Destroys the collection. */
    ~RedeemerList();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static RedeemerList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static RedeemerList fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of redeemers.
     * @return The number of elements.
     */
    std::size_t size() const;

    /**
     * @brief Adds a redeemer in purpose and index order.
     * @param value The redeemer to copy.
     * @throws std::invalid_argument A redeemer has the same purpose and index.
     */
    void add(const Redeemer& value);

    /**
     * @brief Gets a redeemer by index.
     * @param index The zero-based index.
     * @return An independent redeemer.
     */
    Redeemer at(std::size_t index) const;

    /**
     * @brief Updates the execution budget of a redeemer.
     * @param tag The script purpose.
     * @param index The index within that purpose.
     * @param value The new execution budget.
     */
    void setExUnits(RedeemerTag tag, std::uint64_t index, const ExUnits& value);

    /**
     * @brief Encodes the collection as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the collection as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges collections.
     * @param other The collection to exchange.
     */
    void swap(RedeemerList& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    RedeemerList& operator=(const RedeemerList& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    RedeemerList& operator=(RedeemerList&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const RedeemerList& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const RedeemerList& other) const;

  private:

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit RedeemerList(cardano_redeemer_list_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_redeemer_list_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_redeemer_list_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges redeemer collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(RedeemerList& lhs, RedeemerList& rhs) noexcept;
} // namespace Cometa
