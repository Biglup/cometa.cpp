/**
 * @file Blake2bHash.h
 *
 * @brief Declaration of the Blake2bHash value type.
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

#include <Cometa/Crypto/Blake2bHashSize.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_blake2b_hash_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Represents a BLAKE2b hash.
 */
class Blake2bHash
{
  public:

    /**
     * @brief Initializes a Blake2bHash from raw bytes.
     *
     * @param bytes The bytes to copy. The sequence must not be empty.
     *
     * @throws Error The supplied byte length is invalid.
     */
    explicit Blake2bHash(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    Blake2bHash(const Blake2bHash& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The value to move.
     *
     * @remark After the move, other can be destroyed, assigned, or swapped; observing it throws std::logic_error.
     */
    Blake2bHash(Blake2bHash&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Blake2bHash();

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
    static Blake2bHash fromHex(std::string_view hex);

    /**
     * @brief Computes a BLAKE2b digest.
     *
     * @param data The nonempty input byte sequence.
     * @param digestSize The digest length in bytes, from 1 through 64.
     *
     * @return The computed digest.
     *
     * @throws std::invalid_argument The input is empty or the digest length is outside the supported range.
     */
    static Blake2bHash compute(const std::vector<std::uint8_t>& data, std::size_t digestSize = 32);

    /**
     * @brief Computes a BLAKE2b digest with a named output length.
     *
     * @param data The nonempty input bytes.
     * @param digestSize A supported digest length.
     *
     * @return The computed digest.
     *
     * @throws std::invalid_argument The input is empty or the enum value is unknown.
     */
    static Blake2bHash compute(const std::vector<std::uint8_t>& data, Blake2bHashSize digestSize);

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
    static Blake2bHash fromCbor(const std::vector<std::uint8_t>& cbor);

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
    static Blake2bHash fromCbor(std::string_view cborHex);

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
    void swap(Blake2bHash& other) noexcept;

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
    Blake2bHash& operator=(const Blake2bHash& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source, which becomes moved-from unless this is self-assignment.
     *
     * @return This instance.
     *
     * @remark Self-move assignment is safe.
     */
    Blake2bHash& operator=(Blake2bHash&& other) noexcept;

    /**
     * @brief Compares values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const Blake2bHash& other) const;

    /**
     * @brief Compares values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const Blake2bHash& other) const;

    /**
     * @brief Orders values for standard containers.
     *
     * @param other The value to compare.
     *
     * @return True if this value precedes the other value.
     */
    bool operator<(const Blake2bHash& other) const;

  private:

    /** @brief Allows GenesisKeyDelegationCert to copy its hashes. */
    friend class GenesisKeyDelegationCert;

    /** @brief Allows PoolRetirementCert to copy its pool key hash. */
    friend class PoolRetirementCert;

    /** @brief Allows StakeRegistrationDelegationCert to copy its pool key hash. */
    friend class StakeRegistrationDelegationCert;

    /** @brief Allows StakeDelegationCert to copy its pool key hash. */
    friend class StakeDelegationCert;

    /** @brief Allows combined delegation certificates to copy pool hashes. */
    friend class StakeVoteDelegationCert;

    /** @brief Allows PoolParams to copy pool components. */
    friend class PoolParams;

    /** @brief Allows TransactionInput to copy components. */
    friend class TransactionInput;

    /** @brief Allows ScriptPubkey to copy script components. */
    friend class ScriptPubkey;

    /** @brief Allows Blake2bHashSet to copy set elements. */
    friend class Blake2bHashSet;

    /** @brief Allows MultiAsset to copy policy components. */
    friend class MultiAsset;

    /** @brief Allows AssetId to copy identifier components. */
    friend class AssetId;

    /** @brief Allows ByronAddress to copy credential components. */
    friend class ByronAddress;

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference whose ownership is transferred to this instance.
     */
    explicit Blake2bHash(cardano_blake2b_hash_t* value) noexcept;

    /**
     * @brief Allows public-key hashing to adopt the returned digest.
     */
    friend class Ed25519PublicKey;

    /**
     * @brief Allows extended public-key hashing to adopt the returned digest.
     */
    friend class Bip32PublicKey;

  private:

    /**
     * @brief Gets the value after verifying that it is available.
     *
     * @return The stored reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_blake2b_hash_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_blake2b_hash_t* m_value;
    /** @brief Allows pool metadata to exchange content hashes. */
    friend class PoolMetadata;

    /** @brief Allows pool owners to copy hashes. */
    friend class PoolOwners;
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
void swap(Blake2bHash& lhs, Blake2bHash& rhs) noexcept;
} // namespace Cometa
