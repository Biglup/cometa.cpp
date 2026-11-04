/**
 * @file Ed25519Signature.h
 *
 * @brief A detached 64-byte Ed25519 signature.
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

struct cardano_ed25519_signature_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A detached 64-byte Ed25519 signature.
 *
 * The signature does not contain the signed message or verification key. Use
 * Ed25519PublicKey::verify() with both to check authenticity.
 */
class Ed25519Signature
{
  public:

    /**
     * @brief Copies encoded signature bytes.
     * @param bytes Exactly 64 bytes.
     * @throws Error The byte count is invalid.
     */
    explicit Ed25519Signature(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     * @param other The source value.
     */
    Ed25519Signature(const Ed25519Signature& other);

    /**
     * @brief Moves another value.
     * @param other The source value.
     */
    Ed25519Signature(Ed25519Signature&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Ed25519Signature();

  public:

    /**
     * @brief Parses hexadecimal bytes without a prefix.
     * @param hex Exactly 128 hexadecimal characters.
     * @return The parsed value.
     * @throws std::invalid_argument The hexadecimal text is malformed.
     */
    static Ed25519Signature fromHex(std::string_view hex);

  public:

    /**
     * @brief Copies the encoded bytes.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Formats the encoded bytes as lowercase hexadecimal.
     * @return The encoded value without a prefix.
     */
    std::string toHex() const;

    /**
     * @brief Exchanges two values.
     * @param other The value to exchange.
     */
    void swap(Ed25519Signature& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source value.
     * @return This instance; unchanged if copying fails.
     */
    Ed25519Signature& operator=(const Ed25519Signature& other);

    /**
     * @brief Moves another value into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    Ed25519Signature& operator=(Ed25519Signature&& other) noexcept;

    /**
     * @brief Compares the encoded bytes.
     * @param other The value to compare.
     * @return True if the bytes match.
     */
    bool operator==(const Ed25519Signature& other) const;

    /**
     * @brief Compares the encoded bytes.
     * @param other The value to compare.
     * @return True if the bytes differ.
     */
    bool operator!=(const Ed25519Signature& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Ed25519Signature adopt(cardano_ed25519_signature_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_ed25519_signature_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_ed25519_signature_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_ed25519_signature_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned native value.
     * @param value The owned reference transferred to this instance.
     */
    explicit Ed25519Signature(cardano_ed25519_signature_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_ed25519_signature_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_ed25519_signature_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Ed25519Signature& lhs, Ed25519Signature& rhs) noexcept;
} // namespace Cometa
