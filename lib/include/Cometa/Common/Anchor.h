/**
 * @file Anchor.h
 *
 * @brief A URL and content hash identifying off-chain governance metadata.
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

struct cardano_anchor_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A URL and content hash identifying off-chain governance metadata.
 *
 * The URL locates the document; the hash commits to its contents. Constructing an anchor does not
 * fetch the document or verify that the URL serves the expected content.
 */
class Anchor
{
  public:

    /**
     * @brief Initializes the metadata anchor.
     *
     * @param url The URL, containing 1 to 128 bytes without embedded NULs.
     * @param hash The 32-byte metadata hash.
     *
     * @throws std::invalid_argument The URL or hash size is invalid.
     */
    Anchor(std::string_view url, const Blake2bHash& hash);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    Anchor(const Anchor& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    Anchor(Anchor&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Anchor();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded bytes.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static Anchor fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cborHex Hexadecimal CBOR without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static Anchor fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the metadata hash.
     *
     * @return An independent hash value.
     */
    Blake2bHash hash() const;

    /**
     * @brief Replaces the hash.
     *
     * @param hash The 32-byte hash to copy.
     *
     * @throws std::invalid_argument The hash size is not 32 bytes.
     */
    void setHash(const Blake2bHash& hash);

    /**
     * @brief Gets the metadata URL.
     *
     * @return The URL.
     */
    std::string url() const;

    /**
     * @brief Replaces the metadata URL.
     *
     * @param url The URL, containing 1 to 128 bytes without embedded NULs.
     *
     * @throws std::invalid_argument The URL length is invalid or contains an embedded NUL.
     */
    void setUrl(std::string_view url);

    /**
     * @brief Encodes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values.
     *
     * @param other The value to exchange with this instance.
     */
    void swap(Anchor& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance. If copying fails, its value is unchanged.
     */
    Anchor& operator=(const Anchor& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    Anchor& operator=(Anchor&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const Anchor& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are different.
     */
    bool operator!=(const Anchor& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Anchor adopt(cardano_anchor_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_anchor_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_anchor_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_anchor_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit Anchor(cardano_anchor_t* value) noexcept;

  private:

    /**
     * @brief Gets the stored value after checking its lifetime.
     *
     * @return The reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_anchor_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_anchor_t* m_value;
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
void swap(Anchor& lhs, Anchor& rhs) noexcept;
} // namespace Cometa
