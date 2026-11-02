/**
 * @file PlutusMap.h
 *
 * @brief Declaration of the PlutusMap value type.
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
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_plutus_map_t;

namespace Cometa
{

/* CLASS DECLARATION *********************************************************/

/** @brief An ordered sequence of Plutus key-value pairs; duplicate keys are retained. */
class PlutusMap
{
  public:

    /**
     * @brief Initializes an empty collection.
     */
    PlutusMap();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    PlutusMap(const PlutusMap& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    PlutusMap(PlutusMap&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~PlutusMap();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static PlutusMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static PlutusMap fromCbor(std::string_view cborHex);

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
     * @brief Finds the first value with an equal key.
     *
     * @param key The key to find.
     *
     * @return An independent value, or nullopt if absent.
     */
    std::optional<PlutusData> get(const PlutusData& key) const;

    /**
     * @brief Appends a copied key-value pair, retaining duplicate keys.
     *
     * @param key The key to copy.
     * @param value The value to copy.
     */
    void insert(const PlutusData& key, const PlutusData& value);

    /**
     * @brief Gets independent keys in insertion order.
     *
     * @return The copied keys, including duplicates.
     */
    std::vector<PlutusData> keys() const;

    /**
     * @brief Gets independent values in insertion order.
     *
     * @return The copied values, including duplicates.
     */
    std::vector<PlutusData> values() const;

    /**
     * @brief Gets independent pairs in insertion order.
     *
     * @return The copied pairs, including duplicate keys.
     */
    std::vector<std::pair<PlutusData, PlutusData>> entries() const;

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
    void swap(PlutusMap& other) noexcept;

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
    PlutusMap& operator=(const PlutusMap& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    PlutusMap& operator=(PlutusMap&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const PlutusMap& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const PlutusMap& other) const;

  private:

    /** @brief Allows PlutusData to copy components. */
    friend class PlutusData;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit PlutusMap(cardano_plutus_map_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_plutus_map_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_plutus_map_t* m_value;
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
void swap(PlutusMap& lhs, PlutusMap& rhs) noexcept;
} // namespace Cometa
