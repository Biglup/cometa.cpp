/**
 * @file AssetName.h
 *
 * @brief Declaration of the AssetName value type.
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

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_asset_name_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Represents an asset name of up to 32 bytes.
 */
class AssetName
{
  public:

    /**
     * @brief Initializes an AssetName from raw bytes.
     *
     * @param bytes The bytes to copy. The length must be between 0 and 32 bytes.
     *
     * @throws Error The supplied byte length is invalid.
     */
    explicit AssetName(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    AssetName(const AssetName& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The value to move.
     *
     * @remark After the move, other can be destroyed, assigned, or swapped; observing it throws std::logic_error.
     */
    AssetName(AssetName&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~AssetName();

  public:

    /**
     * @brief Creates a value from hexadecimal bytes.
     *
     * @param hex Pairs of hexadecimal digits without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument The input contains invalid digits or an odd number of digits.
     * @throws Error The decoded byte length is invalid.
     */
    static AssetName fromHex(std::string_view hex);

    /**
     * @brief Creates an asset name from string bytes.
     *
     * @param text The bytes to copy, up to 32 bytes; embedded NUL bytes are retained.
     *
     * @return The asset name.
     *
     * @throws Error The input exceeds 32 bytes.
     */
    static AssetName fromString(std::string_view text);

    /**
     * @brief Creates a value from exactly one CBOR item.
     *
     * @param cbor The encoded value.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument The input is empty or bytes remain after the decoded item.
     * @throws Error The CBOR input is invalid.
     */
    static AssetName fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Creates a value from hexadecimal CBOR.
     *
     * @param cborHex The CBOR bytes as pairs of hexadecimal digits, without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument The hex is empty, invalid, or contains bytes after the decoded item.
     * @throws Error The CBOR input is invalid.
     */
    static AssetName fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets a copy of the raw bytes.
     *
     * @return The bytes.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Gets the hexadecimal representation.
     *
     * @return Lowercase hexadecimal digits without a prefix.
     */
    std::string toHex() const;

    /**
     * @brief Gets the asset bytes as a string.
     *
     * @return A string preserving embedded NUL bytes.
     *
     * @remark The returned bytes are not validated as UTF-8.
     */
    std::string toString() const;

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges values without allocating.
     *
     * @param other The value to exchange with this instance.
     *
     * @remark Moved-from values may be swapped.
     */
    void swap(AssetName& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance.
     *
     * @remark Self-assignment is safe. If copying fails, this instance is unchanged.
     */
    AssetName& operator=(const AssetName& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source, which becomes moved-from unless this is self-assignment.
     *
     * @return This instance.
     *
     * @remark Self-move assignment is safe.
     */
    AssetName& operator=(AssetName&& other) noexcept;

    /**
     * @brief Compares values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const AssetName& other) const;

    /**
     * @brief Compares values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const AssetName& other) const;

    /**
     * @brief Orders values for standard containers.
     *
     * @param other The value to compare.
     *
     * @return True if this value precedes the other value.
     */
    bool operator<(const AssetName& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference whose ownership is transferred to this instance.
     */
    explicit AssetName(cardano_asset_name_t* value) noexcept;

  private:

    /**
     * @brief Gets the value after verifying that it is available.
     *
     * @return The stored reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_asset_name_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_asset_name_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges two values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(AssetName& lhs, AssetName& rhs) noexcept;
} // namespace Cometa
