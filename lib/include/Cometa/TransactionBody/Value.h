/**
 * @file Value.h
 *
 * @brief An amount of lovelace and a bundle of native-token quantities.
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

#include <Cometa/Assets/AssetIdMap.h>
#include <Cometa/Assets/MultiAsset.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_value_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief An amount of lovelace and a bundle of native-token quantities.
 *
 * One ada is 1,000,000 lovelace. Signed token quantities support value arithmetic and mint/burn
 * accounting; a spendable output must meet ledger quantity and minimum-lovelace rules.
 */
class Value
{
  public:

    /**
     * @brief Creates a lovelace-only value.
     *
     * @param coin Lovelace; negative values are allowed for arithmetic.
     */
    explicit Value(std::int64_t coin = 0);

    /**
     * @brief Creates a value with native tokens.
     *
     * @param coin Lovelace.
     * @param assets The token quantities to copy.
     */
    Value(std::int64_t coin, const MultiAsset& assets);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    Value(const Value& other);

    /**
     * @brief Moves another value.
     *
     * @param other The source value.
     */
    Value(Value&& other) noexcept;

    /**
     * @brief Destroys the value.
     */
    ~Value();

  public:

    /**
     * @brief Creates a value from identifier-keyed quantities.
     *
     * @param assets The quantities, optionally including lovelace.
     *
     * @return The resulting value.
     */
    static Value fromAssetMap(const AssetIdMap& assets);

    /**
     * @brief Parses one encoded value.
     *
     * @param cbor CBOR bytes or hexadecimal without a prefix.
     *
     * @return The parsed value.
     *
     * @throws Error The CBOR is invalid.
     * @throws std::overflow_error Lovelace exceeds int64_t.
     * @throws std::invalid_argument The input has invalid policy hashes, duplicate keys, or trailing data.
     */
    static Value fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Parses one encoded value.
     *
     * @param cborHex CBOR bytes or hexadecimal without a prefix.
     *
     * @return The parsed value.
     *
     * @throws Error The CBOR is invalid.
     * @throws std::overflow_error Lovelace exceeds int64_t.
     * @throws std::invalid_argument The input has invalid policy hashes, duplicate keys, or trailing data.
     */
    static Value fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the lovelace quantity.
     *
     * @return The signed lovelace amount.
     */
    std::int64_t coin() const;

    /**
     * @brief Replaces the lovelace quantity.
     *
     * @param coin The new signed lovelace amount.
     */
    void setCoin(std::int64_t coin);

    /**
     * @brief Adds lovelace with range checking.
     *
     * @param amount The signed amount to add.
     *
     * @throws std::overflow_error The result exceeds int64_t; this value is unchanged.
     */
    void addCoin(std::int64_t amount);

    /**
     * @brief Subtracts lovelace with range checking.
     *
     * @param amount The signed amount to subtract.
     *
     * @throws std::overflow_error The result exceeds int64_t; this value is unchanged.
     */
    void subtractCoin(std::int64_t amount);

    /**
     * @brief Copies the native-token quantities.
     *
     * @return An independent collection, empty for a lovelace-only value.
     */
    MultiAsset assets() const;

    /**
     * @brief Replaces the native-token quantities.
     *
     * @param assets The quantities to copy.
     */
    void setAssets(const MultiAsset& assets);

    /**
     * @brief Adds native-token quantities.
     *
     * @param assets The quantities to add.
     *
     * @throws std::overflow_error A resulting amount exceeds int64_t; this value is unchanged.
     */
    void addAssets(const MultiAsset& assets);

    /**
     * @brief Subtracts native-token quantities.
     *
     * @param assets The quantities to subtract.
     *
     * @throws std::overflow_error A resulting amount exceeds int64_t; this value is unchanged.
     */
    void subtractAssets(const MultiAsset& assets);

    /**
     * @brief Adds a native-token quantity.
     *
     * @param policyId The 28-byte policy hash.
     * @param name The asset name.
     * @param quantity The signed quantity to add.
     *
     * @throws std::overflow_error The resulting amount exceeds int64_t.
     */
    void addAsset(const Blake2bHash& policyId, const AssetName& name, std::int64_t quantity);

    /**
     * @brief Adds a quantity identified by policy and name hex.
     *
     * @param policyHex The 28-byte policy hash in hexadecimal.
     * @param nameHex The asset name in hexadecimal.
     * @param quantity The signed quantity to add.
     */
    void addAsset(std::string_view policyHex, std::string_view nameHex, std::int64_t quantity);

    /**
     * @brief Adds a token or lovelace quantity.
     *
     * @param id The asset identifier.
     * @param quantity The signed quantity to add.
     *
     * @throws std::overflow_error The resulting amount exceeds int64_t.
     */
    void addAsset(const AssetId& id, std::int64_t quantity);

    /**
     * @brief Adds a quantity identified by asset hex.
     *
     * @param idHex Policy and name hex; empty denotes lovelace.
     * @param quantity The signed quantity to add.
     */
    void addAsset(std::string_view idHex, std::int64_t quantity);

    /**
     * @brief Copies quantities into an identifier-keyed map.
     *
     * @return All token entries and nonzero lovelace, including negative amounts.
     */
    AssetIdMap toAssetMap() const;

    /**
     * @brief Finds shared token identifiers and positive lovelace.
     *
     * @param other The value to compare.
     *
     * @return Shared token keys regardless of quantity; lovelace is included only when positive in both values.
     */
    AssetIdList intersection(const Value& other) const;

    /**
     * @brief Counts shared identifiers.
     *
     * @param other The value to compare.
     *
     * @return The size of intersection(other).
     */
    std::size_t intersectionCount(const Value& other) const;

    /**
     * @brief Counts token entries and positive lovelace.
     *
     * @return Token entries plus one when lovelace is positive.
     */
    std::size_t assetCount() const;

    /**
     * @brief Checks for zero lovelace and no policy entries.
     *
     * @return True only when coin is zero and no policies are stored.
     */
    bool isZero() const;

    /**
     * @brief Encodes the value as CBOR.
     *
     * @return Encoded value bytes.
     *
     * @throws std::invalid_argument Lovelace is negative and cannot be encoded as an output value.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats coin and assets as JSON.
     *
     * @return Compact JSON with quantities as decimal strings.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges values.
     *
     * @param other The value to exchange.
     */
    void swap(Value& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This value; unchanged on failure.
     */
    Value& operator=(const Value& other);

    /**
     * @brief Moves another value into this one.
     *
     * @param other The source value; self-move is safe.
     *
     * @return This value.
     */
    Value& operator=(Value&& other) noexcept;

    /**
     * @brief Adds corresponding quantities.
     *
     * @param other The right-hand value.
     *
     * @return The resulting quantities.
     *
     * @throws std::overflow_error A resulting amount exceeds int64_t.
     */
    Value operator+(const Value& other) const;

    /**
     * @brief Adds quantities into this value.
     *
     * @param other The right-hand value.
     *
     * @return This value; unchanged on failure.
     *
     * @throws std::overflow_error A resulting amount exceeds int64_t.
     */
    Value& operator+=(const Value& other);

    /**
     * @brief Subtracts corresponding quantities.
     *
     * @param other The right-hand value.
     *
     * @return The resulting quantities.
     *
     * @throws std::overflow_error A resulting amount exceeds int64_t.
     */
    Value operator-(const Value& other) const;

    /**
     * @brief Subtracts quantities into this value.
     *
     * @param other The right-hand value.
     *
     * @return This value; unchanged on failure.
     *
     * @throws std::overflow_error A resulting amount exceeds int64_t.
     */
    Value& operator-=(const Value& other);

    /**
     * @brief Checks equality of quantities.
     *
     * @param other The value to compare.
     *
     * @return The comparison result.
     */
    bool operator==(const Value& other) const;

    /**
     * @brief Checks inequality of quantities.
     *
     * @param other The value to compare.
     *
     * @return The comparison result.
     */
    bool operator!=(const Value& other) const;

  private:

    /** @brief Allows TransactionOutput to copy output values. */
    friend class TransactionOutput;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The non-null owned reference.
     */
    explicit Value(cardano_value_t& value) noexcept;

  private:

    /**
     * @brief Checks lifetime and gets the native value.
     *
     * @return The borrowed native reference.
     *
     * @throws std::logic_error The value has been moved from.
     */
    cardano_value_t* checked() const;

  private:

    /**
     * @brief The owned native value, or null after a move.
     */
    cardano_value_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Value& lhs, Value& rhs) noexcept;
} // namespace Cometa
