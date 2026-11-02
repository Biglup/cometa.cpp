/**
 * @file AssetId.h
 *
 * @brief Asset identifiers for lovelace and native tokens.
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

#include <Cometa/Assets/AssetName.h>
#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_asset_id_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Identifies lovelace or a token by its policy ID and asset name. */
class AssetId
{
  public:

    /**
     * @brief Constructs a native-token identifier.
     *
     * @param policyId The 28-byte policy ID to copy.
     * @param assetName The asset name to copy.
     *
     * @throws Error The policy ID length is invalid.
     */
    AssetId(const Blake2bHash& policyId, const AssetName& assetName);

    /**
     * @brief Parses an asset identifier.
     *
     * @param bytes Policy ID followed by up to 32 asset-name bytes; empty input denotes lovelace.
     *
     * @throws Error A nonempty identifier has an invalid length.
     */
    explicit AssetId(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    AssetId(const AssetId& other);

    /**
     * @brief Moves another identifier.
     *
     * @param other The source value.
     */
    AssetId(AssetId&& other) noexcept;

    /**
     * @brief Destroys the identifier.
     */
    ~AssetId();

  public:

    /**
     * @brief Creates the lovelace identifier.
     *
     * @return An identifier with empty byte and hex representations.
     */
    static AssetId lovelace();

    /**
     * @brief Parses hexadecimal identifier bytes.
     *
     * @param hex Even-length hexadecimal without a prefix; empty input denotes lovelace.
     *
     * @return The parsed identifier.
     *
     * @throws std::invalid_argument The hexadecimal text is malformed.
     * @throws Error The decoded identifier length is invalid.
     */
    static AssetId fromHex(std::string_view hex);

  public:

    /**
     * @brief Checks whether the identifier denotes lovelace.
     *
     * @return True for lovelace.
     */
    bool isLovelace() const;

    /**
     * @brief Returns the policy ID.
     *
     * @return An independent policy ID, or nullopt for lovelace.
     */
    std::optional<Blake2bHash> policyId() const;

    /**
     * @brief Returns the asset name.
     *
     * @return An independent name, or nullopt for lovelace.
     */
    std::optional<AssetName> assetName() const;

    /**
     * @brief Copies the identifier bytes.
     *
     * @return Policy ID followed by asset name, or an empty vector for lovelace.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Formats lowercase identifier hex.
     *
     * @return Hexadecimal without a prefix, or an empty string for lovelace.
     */
    std::string toHex() const;

    /**
     * @brief Exchanges two identifiers.
     *
     * @param other The value to exchange.
     */
    void swap(AssetId& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This value; unchanged if copying fails.
     */
    AssetId& operator=(const AssetId& other);

    /**
     * @brief Moves another identifier into this one.
     *
     * @param other The source; self-move is safe.
     *
     * @return This value.
     */
    AssetId& operator=(AssetId&& other) noexcept;

    /**
     * @brief Checks equality.
     *
     * @param other The identifier to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const AssetId& other) const;

    /**
     * @brief Checks inequality.
     *
     * @param other The identifier to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const AssetId& other) const;

    /**
     * @brief Orders identifiers lexicographically by bytes, with lovelace first.
     *
     * @param other The identifier to compare.
     *
     * @return The comparison result.
     */
    bool operator<(const AssetId& other) const;

  private:

    /**
     * @brief Checks the lifetime and gets the stored reference.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error This value has been moved from.
     */
    cardano_asset_id_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_asset_id_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges identifiers through argument-dependent lookup.
 *
 * @param lhs The first identifier.
 * @param rhs The second identifier.
 */
void swap(AssetId& lhs, AssetId& rhs) noexcept;
} // namespace Cometa
