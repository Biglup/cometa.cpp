/**
 * @file MultiAsset.h
 *
 * @brief Policy-indexed native-token quantities.
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

#include <Cometa/Assets/AssetId.h>
#include <Cometa/Assets/AssetNameMap.h>
#include <Cometa/Assets/PolicyIdList.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_multi_asset_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Native-token quantities grouped by 28-byte policy ID.
 */
class MultiAsset
{
  public:

    /**
     * @brief Creates an empty collection.
     */
    MultiAsset();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The collection to copy.
     */
    MultiAsset(const MultiAsset& other);

    /**
     * @brief Moves another collection.
     *
     * @param other The source collection.
     */
    MultiAsset(MultiAsset&& other) noexcept;

    /**
     * @brief Destroys the collection.
     */
    ~MultiAsset();

  public:

    /**
     * @brief Parses one policy-to-assets CBOR map.
     *
     * @param cbor CBOR bytes or hexadecimal without a prefix.
     *
     * @return The parsed quantities.
     *
     * @throws std::invalid_argument The input has duplicate policies, invalid policy lengths, or trailing data.
     * @throws Error The CBOR or amount range is invalid.
     */
    static MultiAsset fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Parses one policy-to-assets CBOR map.
     *
     * @param cborHex CBOR bytes or hexadecimal without a prefix.
     *
     * @return The parsed quantities.
     *
     * @throws std::invalid_argument The input has duplicate policies, invalid policy lengths, or trailing data.
     * @throws Error The CBOR or amount range is invalid.
     */
    static MultiAsset fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Counts policies.
     *
     * @return The number of policy entries, including empty ones.
     */
    std::size_t policyCount() const;

    /**
     * @brief Looks up the assets under one policy.
     *
     * @param policyId The 28-byte policy hash.
     *
     * @return An independent map, or nullopt if absent.
     */
    std::optional<AssetNameMap> assets(const Blake2bHash& policyId) const;

    /**
     * @brief Replaces all assets under one policy.
     *
     * @param policyId The 28-byte policy hash to copy.
     * @param assets The asset quantities to copy.
     */
    void setAssets(const Blake2bHash& policyId, const AssetNameMap& assets);

    /**
     * @brief Looks up an asset quantity.
     *
     * @param policyId The 28-byte policy hash.
     * @param name The asset name.
     *
     * @return The amount, or nullopt if absent.
     */
    std::optional<std::int64_t> get(const Blake2bHash& policyId, const AssetName& name) const;

    /**
     * @brief Looks up an asset quantity by identifier.
     *
     * @param id A native-token identifier.
     *
     * @return The amount, or nullopt if absent.
     *
     * @throws std::invalid_argument The identifier denotes lovelace.
     */
    std::optional<std::int64_t> get(const AssetId& id) const;

    /**
     * @brief Inserts or replaces one asset quantity.
     *
     * @param policyId The 28-byte policy hash to copy.
     * @param name The asset name to copy.
     * @param amount The signed quantity; zero is retained.
     */
    void set(const Blake2bHash& policyId, const AssetName& name, std::int64_t amount);

    /**
     * @brief Inserts or replaces a quantity by identifier.
     *
     * @param id A native-token identifier.
     * @param amount The signed quantity.
     *
     * @throws std::invalid_argument The identifier denotes lovelace.
     */
    void set(const AssetId& id, std::int64_t amount);

    /**
     * @brief Copies the policy IDs.
     *
     * @return Policy hashes in serialization order.
     */
    PolicyIdList policies() const;

    /**
     * @brief Copies all policy maps.
     *
     * @return Independent policy/map pairs in serialization order.
     */
    std::vector<std::pair<Blake2bHash, AssetNameMap>> entries() const;

    /**
     * @brief Selects strictly positive quantities.
     *
     * @return The selected quantities with their signs preserved.
     */
    MultiAsset positive() const;

    /**
     * @brief Selects strictly negative quantities.
     *
     * @return The selected quantities with their signs preserved.
     */
    MultiAsset negative() const;

    /**
     * @brief Encodes policy maps as CBOR.
     *
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats policy and asset-name keyed JSON objects.
     *
     * @return Compact JSON with amounts as decimal strings.
     */
    std::string toJson() const;

    /**
     * @brief Formats policy entries as a JSON array.
     *
     * @return Objects containing script_hash and assets; amounts are decimal strings.
     */
    std::string toJsonEntries() const;

    /**
     * @brief Exchanges collections.
     *
     * @param other The collection to exchange.
     */
    void swap(MultiAsset& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source collection.
     *
     * @return This collection; unchanged on failure.
     */
    MultiAsset& operator=(const MultiAsset& other);

    /**
     * @brief Moves another collection into this one.
     *
     * @param other The source collection; self-move is safe.
     *
     * @return This collection.
     */
    MultiAsset& operator=(MultiAsset&& other) noexcept;

    /**
     * @brief Adds corresponding quantities, removing zero results.
     *
     * @param other The right-hand quantities.
     *
     * @return The resulting quantities.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset operator+(const MultiAsset& other) const;

    /**
     * @brief Adds quantities into this collection.
     *
     * @param other The right-hand quantities.
     *
     * @return This collection; unchanged on failure.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset& operator+=(const MultiAsset& other);

    /**
     * @brief Subtracts corresponding quantities, removing zero results.
     *
     * @param other The right-hand quantities.
     *
     * @return The resulting quantities.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset operator-(const MultiAsset& other) const;

    /**
     * @brief Subtracts quantities into this collection.
     *
     * @param other The right-hand quantities.
     *
     * @return This collection; unchanged on failure.
     *
     * @throws std::overflow_error A quantity exceeds the signed 64-bit range.
     */
    MultiAsset& operator-=(const MultiAsset& other);

    /**
     * @brief Checks equality of policy maps.
     *
     * @param other The collection to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const MultiAsset& other) const;

    /**
     * @brief Checks inequality of policy maps.
     *
     * @param other The collection to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const MultiAsset& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /** @brief Allows Value to copy its token quantities. */
    friend class Value;

    /**
     * @brief Adopts one owned native collection.
     *
     * @param value The owned reference to transfer.
     */
    explicit MultiAsset(cardano_multi_asset_t* value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native collection.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The collection has been moved from.
     */
    cardano_multi_asset_t* checked() const;

  private:

    /**
     * @brief The owned native collection, or null after a move.
     */
    cardano_multi_asset_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges collections through argument-dependent lookup.
 *
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(MultiAsset& lhs, MultiAsset& rhs) noexcept;
} // namespace Cometa
