/**
 * @file Bip32PublicKey.h
 *
 * @brief An extended Ed25519 public key with a chain code for public child derivation.
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

#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Crypto/Ed25519PublicKey.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_bip32_public_key_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief An extended Ed25519 public key with a chain code for public child derivation.
 *
 * Use it for watch-only address derivation along non-hardened paths. For a payment or staking
 * credential, hash toEd25519() rather than the extended key including its chain code.
 */
class Bip32PublicKey
{
  public:

    /**
     * @brief Copies encoded key bytes.
     * @param bytes Exactly 64 bytes.
     * @throws Error The byte count is invalid.
     */
    explicit Bip32PublicKey(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     * @param other The source value.
     */
    Bip32PublicKey(const Bip32PublicKey& other);

    /**
     * @brief Moves another value.
     * @param other The source value.
     */
    Bip32PublicKey(Bip32PublicKey&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Bip32PublicKey();

  public:

    /**
     * @brief Parses hexadecimal bytes without a prefix.
     * @param hex Exactly 128 hexadecimal characters.
     * @return The parsed value.
     * @throws std::invalid_argument The hexadecimal text is malformed.
     */
    static Bip32PublicKey fromHex(std::string_view hex);

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
     * @brief Computes a 28-byte Blake2b hash of all 64 bytes, including the chain code.
     * @return The extended-key digest; use toEd25519().hash() for a credential hash.
     */
    Blake2bHash hash() const;

    /**
     * @brief Derives a sequence of child keys.
     * @param path Derivation indices; an empty path returns an independent copy. Only non-hardened indices are accepted.
     * @return The derived key.
     * @throws Error An index or curve operation is invalid, or derivation fails.
     */
    Bip32PublicKey derive(const std::vector<std::uint32_t>& path) const;

    /**
     * @brief Extracts the Ed25519 key without its chain code.
     * @return The Ed25519 key.
     */
    Ed25519PublicKey toEd25519() const;

    /**
     * @brief Exchanges two values.
     * @param other The value to exchange.
     */
    void swap(Bip32PublicKey& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source value.
     * @return This instance; unchanged if copying fails.
     */
    Bip32PublicKey& operator=(const Bip32PublicKey& other);

    /**
     * @brief Moves another value into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    Bip32PublicKey& operator=(Bip32PublicKey&& other) noexcept;

    /**
     * @brief Compares the encoded bytes.
     * @param other The value to compare.
     * @return True if the bytes match.
     */
    bool operator==(const Bip32PublicKey& other) const;

    /**
     * @brief Compares the encoded bytes.
     * @param other The value to compare.
     * @return True if the bytes differ.
     */
    bool operator!=(const Bip32PublicKey& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Bip32PublicKey adopt(cardano_bip32_public_key_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_bip32_public_key_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_bip32_public_key_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_bip32_public_key_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned native value.
     * @param value The owned reference transferred to this instance.
     */
    explicit Bip32PublicKey(cardano_bip32_public_key_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_bip32_public_key_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_bip32_public_key_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Bip32PublicKey& lhs, Bip32PublicKey& rhs) noexcept;
} // namespace Cometa
