/**
 * @file BlsKey.h
 *
 * @brief A BLS public key and proof of possession.
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

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_bls_key_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A BLS public key and proof of possession. */
class BlsKey
{
  public:

    /**
     * @brief Initializes a public key and its proof.
     *
     * @param publicKey Exactly 96 bytes.
     * @param possessionProof Exactly 48 bytes; cryptographic validity is not checked.
     */
    BlsKey(const std::vector<std::uint8_t>& publicKey, const std::vector<std::uint8_t>& possessionProof);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The source value.
     */
    BlsKey(const BlsKey& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    BlsKey(BlsKey&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~BlsKey();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static BlsKey fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static BlsKey fromCbor(std::string_view cborHex);

    /**
     * @brief Parses a hexadecimal public key and proof.
     *
     * @param publicKeyHex Exactly 192 hexadecimal characters.
     * @param possessionProofHex Exactly 96 hexadecimal characters.
     *
     * @return The parsed key and proof.
     */
    static BlsKey fromHex(std::string_view publicKeyHex, std::string_view possessionProofHex);

  public:

    /**
     * @brief Gets the public key.
     *
     * @return The copied 96 bytes.
     */
    std::vector<std::uint8_t> publicKey() const;

    /**
     * @brief Gets the proof of possession.
     *
     * @return The copied 48 bytes.
     */
    std::vector<std::uint8_t> possessionProof() const;

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
    void swap(BlsKey& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance; unchanged if copying fails.
     */
    BlsKey& operator=(const BlsKey& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    BlsKey& operator=(BlsKey&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when equal.
     */
    bool operator==(const BlsKey& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when different.
     */
    bool operator!=(const BlsKey& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit BlsKey(cardano_bls_key_t& value) noexcept;

  private:

    /**
     * @brief Checks and gets the stored reference.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_bls_key_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_bls_key_t* m_value;

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
void swap(BlsKey& lhs, BlsKey& rhs) noexcept;
} // namespace Cometa
