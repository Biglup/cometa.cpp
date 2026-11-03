/**
 * @file Ed25519PublicKey.h
 *
 * @brief A 32-byte Ed25519 key used to verify signatures.
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
#include <Cometa/Crypto/Ed25519Signature.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_ed25519_public_key_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A 32-byte Ed25519 key used to verify signatures.
 *
 * hash() produces the 28-byte key hash used by credentials. Verification checks the signature against
 * the exact message bytes supplied by the caller.
 */
class Ed25519PublicKey
{
  public:

    /**
     * @brief Copies encoded key bytes.
     * @param bytes Exactly 32 bytes.
     * @throws Error The byte count is invalid.
     */
    explicit Ed25519PublicKey(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     * @param other The source value.
     */
    Ed25519PublicKey(const Ed25519PublicKey& other);

    /**
     * @brief Moves another value.
     * @param other The source value.
     */
    Ed25519PublicKey(Ed25519PublicKey&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Ed25519PublicKey();

  public:

    /**
     * @brief Parses hexadecimal bytes without a prefix.
     * @param hex Exactly 64 hexadecimal characters.
     * @return The parsed value.
     * @throws std::invalid_argument The hexadecimal text is malformed.
     */
    static Ed25519PublicKey fromHex(std::string_view hex);

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
     * @brief Verifies a detached signature.
     * @param signature The signature to check.
     * @param message The signed message; may be empty.
     * @return True if the signature is valid for this key and message.
     */
    bool verify(const Ed25519Signature& signature, const std::vector<std::uint8_t>& message) const;

    /**
     * @brief Computes a 28-byte Blake2b hash of the verification key.
     * @return The digest.
     */
    Blake2bHash hash() const;

    /**
     * @brief Exchanges two values.
     * @param other The value to exchange.
     */
    void swap(Ed25519PublicKey& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source value.
     * @return This instance; unchanged if copying fails.
     */
    Ed25519PublicKey& operator=(const Ed25519PublicKey& other);

    /**
     * @brief Moves another value into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    Ed25519PublicKey& operator=(Ed25519PublicKey&& other) noexcept;

    /**
     * @brief Compares the encoded bytes.
     * @param other The value to compare.
     * @return True if the bytes match.
     */
    bool operator==(const Ed25519PublicKey& other) const;

    /**
     * @brief Compares the encoded bytes.
     * @param other The value to compare.
     * @return True if the bytes differ.
     */
    bool operator!=(const Ed25519PublicKey& other) const;

  private:

    /**
     * @brief Allows Ed25519PrivateKey to transfer or inspect key material.
     */
    /** @brief Allows key witnesses to copy signed values. */
    friend class VkeyWitness;

    /** @brief Allows bootstrap witnesses to copy signed values. */
    friend class BootstrapWitness;

    friend class Ed25519PrivateKey;

    /**
     * @brief Allows Bip32PublicKey to transfer or inspect key material.
     */
    friend class Bip32PublicKey;

    /**
     * @brief Adopts an owned native value.
     * @param value The owned reference transferred to this instance.
     */
    explicit Ed25519PublicKey(cardano_ed25519_public_key_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_ed25519_public_key_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_ed25519_public_key_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Ed25519PublicKey& lhs, Ed25519PublicKey& rhs) noexcept;
} // namespace Cometa
