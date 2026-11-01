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
 * @brief Represents an owning Blake2bHash value.
 *
 * @remark Copies own independent native storage. Moves transfer ownership without allocation.
 *
 * Moved-from objects can be destroyed, assigned, or swapped. Observing their value
 * throws std::logic_error. Native operation errors throw Error; native allocation
 * error codes and C++ allocation failures throw std::bad_alloc.
 */
class Blake2bHash
{
  public:

    /**
     * @brief Initializes a Blake2bHash from raw bytes.
     *
     * @param bytes The bytes to copy. The sequence must not be empty.
     *
     * @throws Error The native constructor rejects the supplied bytes.
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
     * @brief Transfers ownership from another value.
     *
     * @param other The source, which becomes moved-from.
     */
    Blake2bHash(Blake2bHash&& other) noexcept;

    /**
     * @brief Releases the owned native storage.
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
     * @throws Error The decoded bytes are rejected by the native constructor.
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
     * @brief Creates a value from exactly one CBOR item.
     *
     * @param cbor The encoded value.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Bytes remain after the decoded item.
     * @throws Error The native decoder rejects the CBOR input.
     */
    static Blake2bHash fromCbor(const std::vector<std::uint8_t>& cbor);

  public:

    /**
     * @brief Gets a copy of the raw bytes.
     *
     * @return An owned byte sequence.
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
     * @return An owned CBOR byte sequence.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return An owned, compact JSON string.
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
     * @brief Transfers ownership and releases the previous value.
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

    /**
     * @brief Adopts one owned native reference.
     *
     * @param value A non-null owned reference returned by a successful native operation.
     */
    explicit Blake2bHash(cardano_blake2b_hash_t* value) noexcept;

  private:

    /**
     * @brief Gets the native value after checking its lifetime.
     *
     * @return The borrowed native pointer.
     *
     * @throws std::logic_error This object has been moved from.
     */
    cardano_blake2b_hash_t* checked() const;

  private:

    /**
     * @brief Owns one native reference; null only after ownership has been moved.
     */
    cardano_blake2b_hash_t* m_value;
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
