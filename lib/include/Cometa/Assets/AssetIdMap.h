/**
 * @file AssetIdMap.h
 *
 * @brief Signed asset quantities keyed by AssetId.
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

#include <Cometa/Assets/AssetIdList.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_asset_id_map_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A map of AssetId keys to signed 64-bit quantities.
 */
class AssetIdMap
{
  public:

    /**
     * @brief Creates an empty map.
     */
    AssetIdMap();

    /**
     * @brief Creates a map from entries; later duplicate keys replace earlier amounts.
     *
     * @param entries The keys and quantities to copy.
     */
    AssetIdMap(std::initializer_list<std::pair<AssetId, std::int64_t>> entries);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The map to copy.
     */
    AssetIdMap(const AssetIdMap& other);

    /**
     * @brief Moves another map.
     *
     * @param other The source map.
     */
    AssetIdMap(AssetIdMap&& other) noexcept;

    /**
     * @brief Destroys the map.
     */
    ~AssetIdMap();

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
    std::optional<std::int64_t> get(const AssetId& key) const;

    /**
     * @brief Looks up a required quantity.
     *
     * @param key The key to look up.
     *
     * @return The stored amount.
     *
     * @throws std::out_of_range The key is absent.
     */
    std::int64_t at(const AssetId& key) const;

    /**
     * @brief Inserts or replaces a quantity.
     *
     * @param key The key to copy.
     * @param amount The signed quantity; zero is retained.
     */
    void set(const AssetId& key, std::int64_t amount);

    /**
     * @brief Copies the map keys.
     *
     * @return Independent keys in serialization order.
     */
    AssetIdList keys() const;

    /**
     * @brief Copies the map entries.
     *
     * @return Independent key/quantity pairs in serialization order.
     */
    std::vector<std::pair<AssetId, std::int64_t>> entries() const;

    /**
     * @brief Exchanges maps.
     *
     * @param other The map to exchange.
     */
    void swap(AssetIdMap& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source map.
     *
     * @return This map; unchanged if copying fails.
     */
    AssetIdMap& operator=(const AssetIdMap& other);

    /**
     * @brief Moves another map into this one.
     *
     * @param other The source map; self-move is safe.
     *
     * @return This map.
     */
    AssetIdMap& operator=(AssetIdMap&& other) noexcept;

    /**
     * @brief Adds corresponding quantities, removing zero results.
     *
     * @param other The right-hand map.
     *
     * @return The resulting map.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap operator+(const AssetIdMap& other) const;

    /**
     * @brief Adds quantities into this map.
     *
     * @param other The right-hand map.
     *
     * @return This map; unchanged on failure.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap& operator+=(const AssetIdMap& other);

    /**
     * @brief Subtracts corresponding quantities, removing zero results.
     *
     * @param other The right-hand map.
     *
     * @return The resulting map.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap operator-(const AssetIdMap& other) const;

    /**
     * @brief Subtracts quantities into this map.
     *
     * @param other The right-hand map.
     *
     * @return This map; unchanged on failure.
     *
     * @throws std::overflow_error A resulting quantity exceeds the signed 64-bit range.
     */
    AssetIdMap& operator-=(const AssetIdMap& other);

    /**
     * @brief Checks equality of keys and quantities.
     *
     * @param other The map to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const AssetIdMap& other) const;

    /**
     * @brief Checks inequality of keys and quantities.
     *
     * @param other The map to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const AssetIdMap& other) const;

  private:

    /**
     * @brief Adopts one owned native map.
     *
     * @param value The owned reference to transfer.
     */
    explicit AssetIdMap(cardano_asset_id_map_t* value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native map.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The map has been moved from.
     */
    cardano_asset_id_map_t* checked() const;

  private:

    /**
     * @brief The owned native map, or null after a move.
     */
    cardano_asset_id_map_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges maps through argument-dependent lookup.
 *
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(AssetIdMap& lhs, AssetIdMap& rhs) noexcept;
} // namespace Cometa
