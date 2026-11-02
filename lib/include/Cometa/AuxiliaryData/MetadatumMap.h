/**
 * @file MetadatumMap.h
 *
 * @brief Declaration of the MetadatumMap value type.
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

#include <Cometa/AuxiliaryData/Metadatum.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_metadatum_map_t;

namespace Cometa
{

/* CLASS DECLARATION *********************************************************/

/** @brief An ordered sequence of metadata key-value pairs; duplicate keys are retained. */
class MetadatumMap
{
  public:

    /**
     * @brief Initializes an empty collection.
     */
    MetadatumMap();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    MetadatumMap(const MetadatumMap& other);

    /**
     * @brief Moves a value into this instance.
     *
     * @param other The value to move.
     */
    MetadatumMap(MetadatumMap&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~MetadatumMap();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static MetadatumMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static MetadatumMap fromCbor(std::string_view cborHex);

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
    std::optional<Metadatum> get(const Metadatum& key) const;

    /**
     * @brief Appends a copied key-value pair, retaining duplicate keys.
     *
     * @param key The key to copy.
     * @param value The value to copy.
     */
    void insert(const Metadatum& key, const Metadatum& value);

    /**
     * @brief Gets independent keys in insertion order.
     *
     * @return The copied keys, including duplicates.
     */
    std::vector<Metadatum> keys() const;

    /**
     * @brief Gets independent values in insertion order.
     *
     * @return The copied values, including duplicates.
     */
    std::vector<Metadatum> values() const;

    /**
     * @brief Gets independent pairs in insertion order.
     *
     * @return The copied pairs, including duplicate keys.
     */
    std::vector<std::pair<Metadatum, Metadatum>> entries() const;

    /**
     * @brief Gets an independent pair in insertion order.
     *
     * @param index The zero-based entry index.
     *
     * @return The copied key and value.
     *
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<Metadatum, Metadatum> entryAt(std::size_t index) const;

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The encoded bytes; collection length forms are retained and scalar encodings are normalized.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(MetadatumMap& other) noexcept;

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
    MetadatumMap& operator=(const MetadatumMap& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    MetadatumMap& operator=(MetadatumMap&& other) noexcept;

    /**
     * @brief Compares data values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const MetadatumMap& other) const;

    /**
     * @brief Compares data values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const MetadatumMap& other) const;

  private:

    /** @brief Allows Metadatum to copy components. */
    friend class Metadatum;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit MetadatumMap(cardano_metadatum_map_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_metadatum_map_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_metadatum_map_t* m_value;
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
void swap(MetadatumMap& lhs, MetadatumMap& rhs) noexcept;
} // namespace Cometa
