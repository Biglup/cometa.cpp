/**
 * @file PoolMetadata.h
 *
 * @brief A URL and content hash for a stake pool's off-chain metadata.
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

#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_pool_metadata_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A URL and content hash for a stake pool's off-chain metadata.
 *
 * The hash commits to the fetched document. This value describes the reference; it does not download
 * or validate the metadata document.
 */
class PoolMetadata
{
  public:

    /**
     * @brief Initializes a metadata location and content hash.
     *
     * @param url At most 128 bytes without embedded NULs; may be empty.
     * @param hash The 32-byte content hash to copy.
     */
    PoolMetadata(std::string_view url, const Blake2bHash& hash);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    PoolMetadata(const PoolMetadata& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    PoolMetadata(PoolMetadata&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~PoolMetadata();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static PoolMetadata fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static PoolMetadata fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the metadata URL.
     *
     * @return The URL text.
     */
    std::string url() const;

    /**
     * @brief Replaces the metadata URL.
     *
     * @param url At most 128 bytes without embedded NULs; may be empty.
     */
    void setUrl(std::string_view url);

    /**
     * @brief Gets the metadata content hash.
     *
     * @return An independent content hash.
     */
    Blake2bHash hash() const;

    /**
     * @brief Replaces the metadata content hash.
     *
     * @param hash The 32-byte hash to copy.
     */
    void setHash(const Blake2bHash& hash);

    /**
     * @brief Serializes this value as CBOR.
     *
     * @return The encoded value.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes this value as CIP-116 JSON.
     *
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges values without allocation.
     *
     * @param other The value to exchange.
     */
    void swap(PoolMetadata& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    PoolMetadata& operator=(const PoolMetadata& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    PoolMetadata& operator=(PoolMetadata&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const PoolMetadata& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const PoolMetadata& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit PoolMetadata(cardano_pool_metadata_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_pool_metadata_t* checked() const;

    /**
     * @brief Checks the URL representation.
     *
     * @param url The proposed URL.
     */
    static void validateUrl(std::string_view url);

    /**
     * @brief Checks the content hash length.
     *
     * @param hash The proposed hash.
     */
    static void validateHash(const Blake2bHash& hash);

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_pool_metadata_t* m_value;

    /** @brief Allows PoolParams to exchange component values. */
    friend class PoolParams;
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
void swap(PoolMetadata& lhs, PoolMetadata& rhs) noexcept;
} // namespace Cometa
