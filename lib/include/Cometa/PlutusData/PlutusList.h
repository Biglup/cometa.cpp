/**
 * @file PlutusList.h
 *
 * @brief Declaration of the PlutusList value type.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/PlutusData/PlutusData.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_plutus_list_t;

namespace Cometa
{

/* CLASS DECLARATION *********************************************************/

/** @brief An ordered sequence of Plutus data values. */
class PlutusList
{
  public:

    /**
     * @brief Initializes an empty collection.
     */
    PlutusList();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    PlutusList(const PlutusList& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    PlutusList(PlutusList&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~PlutusList();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static PlutusList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static PlutusList fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of entries.
     *
     * @return The entry count, including duplicates.
     */
    std::size_t size() const;

    /**
     * @brief Tests whether the collection is empty.
     *
     * @return True when there are no entries.
     */
    bool empty() const;

    /**
     * @brief Gets an independent element.
     *
     * @param index The zero-based element index.
     *
     * @return The copied element.
     *
     * @throws std::out_of_range The index is not less than size().
     */
    PlutusData at(std::size_t index) const;

    /**
     * @brief Appends a copied element.
     *
     * @param data The element to append.
     */
    void pushBack(const PlutusData& data);

    /**
     * @brief Gets independent elements in order.
     *
     * @return The copied elements.
     */
    std::vector<PlutusData> values() const;

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The encoded bytes; decoded Plutus data retains its original encoding until changed.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Rebuilds subsequent CBOR encodings from the current fields.
     *
     * @remark Recursively discards original encodings; datum hashes may change.
     */
    void normalizeEncoding();

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(PlutusList& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance.
     *
     * @remark If copying fails, this instance is unchanged.
     */
    PlutusList& operator=(const PlutusList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    PlutusList& operator=(PlutusList&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const PlutusList& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const PlutusList& other) const;

  private:

    /** @brief Allows PlutusV1Script to apply data parameters. */
    friend class PlutusV1Script;

    /** @brief Allows PlutusV2Script to apply data parameters. */
    friend class PlutusV2Script;

    /** @brief Allows PlutusV3Script to apply data parameters. */
    friend class PlutusV3Script;

    /** @brief Allows PlutusV4Script to apply data parameters. */
    friend class PlutusV4Script;

    /** @brief Allows PlutusData to copy components. */
    friend class PlutusData;

    /** @brief Allows PlutusMap to copy components. */
    friend class PlutusMap;

    /** @brief Allows ConstrPlutusData to copy components. */
    friend class ConstrPlutusData;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit PlutusList(cardano_plutus_list_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_plutus_list_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_plutus_list_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(PlutusList& lhs, PlutusList& rhs) noexcept;
} // namespace Cometa
