/**
 * @file AssetNameMap.h
 *
 * @brief Signed asset quantities keyed by AssetName.
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

#include <Cometa/Assets/AssetNameList.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_asset_name_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A map of AssetName keys to signed 64-bit quantities.
 */
class AssetNameMap
{
  public:

    /**
     * @brief Creates an empty map.
     */
    AssetNameMap();

    /**
     * @brief Creates a map from entries; later duplicate keys replace earlier amounts.
     *
     * @param entries The keys and quantities to copy.
     */
    AssetNameMap(std::initializer_list<std::pair<AssetName, std::int64_t>> entries);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The map to copy.
     */
    AssetNameMap(const AssetNameMap& other);

    /**
     * @brief Moves another map.
     *
     * @param other The source map.
     */
    AssetNameMap(AssetNameMap&& other) noexcept;

    /**
     * @brief Destroys the map.
     */
    ~AssetNameMap();

  public:

    /**
     * @brief Parses a map of asset names and signed quantities.
     *
     * @param cbor Exactly one CBOR map.
     *
     * @return The parsed map.
     *
     * @throws Error The CBOR or quantity range is invalid.
     * @throws std::invalid_argument The input contains duplicate keys, malformed hex, or trailing data.
     */
    static AssetNameMap fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Parses a map of asset names and signed quantities.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The parsed map.
     *
     * @throws Error The CBOR or quantity range is invalid.
     * @throws std::invalid_argument The input contains duplicate keys, malformed hex, or trailing data.
     */
    static AssetNameMap fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of entries.
     *
     * @return The entry count.
     */
    std::size_t size() const;

    /**
     * @brief Checks whether the map has no entries.
     *
     * @return True when no keys are present.
     */
    bool empty() const;

    /**
     * @brief Looks up a quantity.
     *
     * @param key The key to look up.
     *
     * @return The stored amount, or nullopt if absent.
     */
    std::optional<std::int64_t> get(const AssetName& key) const;

    /**
     * @brief Looks up a required quantity.
     *
     * @param key The key to look up.
     *
     * @return The stored amount.
     *
     * @throws std::out_of_range The key is absent.
     */
    std::int64_t at(const AssetName& key) const;

    /**
     * @brief Inserts or replaces a quantity.
     *
     * @param key The key to copy.
     * @param amount The signed quantity; zero is retained.
     */
    void set(const AssetName& key, std::int64_t amount);

    /**
     * @brief Copies the map keys.
     *
     * @return Independent keys in serialization order.
     */
    AssetNameList keys() const;

    /**
     * @brief Copies the map entries.
     *
     * @return Independent key/quantity pairs in serialization order.
     */
    std::vector<std::pair<AssetName, std::int64_t>> entries() const;

    /**
     * @brief Encodes the map as CBOR.
     *
     * @return Encoded map bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the map as a JSON object keyed by asset-name hex.
     *
     * @return Compact JSON with quantities encoded as decimal strings.
     */
    std::string toJson() const;

    /**
     * @brief Formats entries as JSON objects containing asset_name and amount.
     *
     * @return A compact JSON array; amounts are decimal strings.
     */
    std::string toJsonEntries() const;

    /**
     * @brief Exchanges maps.
     *
     * @param other The map to exchange.
     */
    void swap(AssetNameMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source map.
     *
     * @return This map; unchanged if copying fails.
     */
    AssetNameMap& operator=(const AssetNameMap& other);

    /**
     * @brief Moves another map into this one.
     *
     * @param other The source map; self-move is safe.
     *
     * @return This map.
     */
    AssetNameMap& operator=(AssetNameMap&& other) noexcept;

    /**
     * @brief Adds corresponding quantities, removing zero results.
     *
     * @param other The right-hand map.
     *
     * @return The resulting map.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetNameMap operator+(const AssetNameMap& other) const;

    /**
     * @brief Adds quantities into this map.
     *
     * @param other The right-hand map.
     *
     * @return This map; unchanged on failure.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetNameMap& operator+=(const AssetNameMap& other);

    /**
     * @brief Subtracts corresponding quantities, removing zero results.
     *
     * @param other The right-hand map.
     *
     * @return The resulting map.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetNameMap operator-(const AssetNameMap& other) const;

    /**
     * @brief Subtracts quantities into this map.
     *
     * @param other The right-hand map.
     *
     * @return This map; unchanged on failure.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetNameMap& operator-=(const AssetNameMap& other);

    /**
     * @brief Checks equality of keys and quantities.
     *
     * @param other The map to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const AssetNameMap& other) const;

    /**
     * @brief Checks inequality of keys and quantities.
     *
     * @param other The map to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const AssetNameMap& other) const;

  private:

    /** @brief Allows MultiAsset to copy policy maps. */
    friend class MultiAsset;

    /**
     * @brief Adopts one owned native map.
     *
     * @param value The owned reference to transfer.
     */
    explicit AssetNameMap(cardano_asset_name_map_t* value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native map.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The map has been moved from.
     */
    cardano_asset_name_map_t* checked() const;

  private:

    /**
     * @brief The owned native map, or null after a move.
     */
    cardano_asset_name_map_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges maps through argument-dependent lookup.
 *
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(AssetNameMap& lhs, AssetNameMap& rhs) noexcept;
} // namespace Cometa
