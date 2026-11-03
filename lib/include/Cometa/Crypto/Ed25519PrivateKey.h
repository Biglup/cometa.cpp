/**
 * @file Ed25519PrivateKey.h
 *
 * @brief A normal or extended Ed25519 key used to sign messages and transaction hashes.
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

#include <Cometa/Crypto/Ed25519PublicKey.h>
#include <Cometa/Crypto/Ed25519Signature.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_ed25519_private_key_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A normal or extended Ed25519 key used to sign messages and transaction hashes.
 *
 * Normal keys are represented by 32-byte seeds; extended signing keys use 64 bytes. Neither form
 * includes a BIP32 chain code. Exported key bytes are sensitive.
 */
class Ed25519PrivateKey
{
  public:

    /**
     * @brief Initializes an independent copy.
     * @param other The source value.
     */
    Ed25519PrivateKey(const Ed25519PrivateKey& other);

    /**
     * @brief Moves another value.
     * @param other The source value.
     */
    Ed25519PrivateKey(Ed25519PrivateKey&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Ed25519PrivateKey();

  public:

    /**
     * @brief Copies a 32-byte Ed25519 seed.
     * @param bytes Exactly 32 bytes.
     * @return The signing key.
     * @throws Error The byte count is invalid.
     */
    static Ed25519PrivateKey fromNormalBytes(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Parses normal signing key bytes.
     * @param hex Exactly 64 hexadecimal characters without a prefix.
     * @return The signing key.
     * @throws std::invalid_argument The hexadecimal text is malformed.
     */
    static Ed25519PrivateKey fromNormalHex(std::string_view hex);

    /**
     * @brief Copies a 64-byte extended Ed25519 signing key.
     * @param bytes Exactly 64 bytes.
     * @return The signing key.
     * @throws Error The byte count is invalid.
     */
    static Ed25519PrivateKey fromExtendedBytes(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Parses extended signing key bytes.
     * @param hex Exactly 128 hexadecimal characters without a prefix.
     * @return The signing key.
     * @throws std::invalid_argument The hexadecimal text is malformed.
     */
    static Ed25519PrivateKey fromExtendedHex(std::string_view hex);

  public:

    /**
     * @brief Copies the encoded bytes.
     * @return The secret key bytes.
     */
    std::vector<std::uint8_t> toBytes() const;

    /**
     * @brief Formats the encoded bytes as lowercase hexadecimal.
     * @return The secret key without a prefix.
     */
    std::string toHex() const;

    /**
     * @brief Checks whether the key uses the extended 64-byte form.
     * @return True for an extended key; false for a normal seed.
     */
    bool isExtended() const;

    /**
     * @brief Signs a byte sequence.
     * @param message The message; may be empty.
     * @return The detached signature.
     */
    Ed25519Signature sign(const std::vector<std::uint8_t>& message) const;

    /**
     * @brief Derives the verification key.
     * @return The verification key.
     */
    Ed25519PublicKey publicKey() const;

    /**
     * @brief Exchanges two values.
     * @param other The value to exchange.
     */
    void swap(Ed25519PrivateKey& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source value.
     * @return This instance; unchanged if copying fails.
     */
    Ed25519PrivateKey& operator=(const Ed25519PrivateKey& other);

    /**
     * @brief Moves another value into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    Ed25519PrivateKey& operator=(Ed25519PrivateKey&& other) noexcept;

  private:

    /**
     * @brief Allows Bip32PrivateKey to transfer or inspect key material.
     */
    friend class Bip32PrivateKey;

    /**
     * @brief Adopts an owned native value.
     * @param value The owned reference transferred to this instance.
     */
    explicit Ed25519PrivateKey(cardano_ed25519_private_key_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_ed25519_private_key_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_ed25519_private_key_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Ed25519PrivateKey& lhs, Ed25519PrivateKey& rhs) noexcept;
} // namespace Cometa
