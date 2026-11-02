/**
 * @file Bip32PrivateKey.h
 *
 * @brief Bip32PrivateKey operations.
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

#include <Cometa/Crypto/Bip32PublicKey.h>
#include <Cometa/Crypto/Ed25519PrivateKey.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_bip32_private_key_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A 96-byte extended Ed25519 private key with a chain code. */
class Bip32PrivateKey
{
  public:

    /**
     * @brief Copies encoded key bytes.
     * @param bytes Exactly 96 bytes.
     * @throws Error The byte count is invalid.
     */
    explicit Bip32PrivateKey(const std::vector<std::uint8_t>& bytes);

    /**
     * @brief Initializes an independent copy.
     * @param other The source value.
     */
    Bip32PrivateKey(const Bip32PrivateKey& other);

    /**
     * @brief Moves another value.
     * @param other The source value.
     */
    Bip32PrivateKey(Bip32PrivateKey&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Bip32PrivateKey();

  public:

    /**
     * @brief Parses hexadecimal bytes without a prefix.
     * @param hex Exactly 192 hexadecimal characters.
     * @return The parsed value.
     * @throws std::invalid_argument The hexadecimal text is malformed.
     */
    static Bip32PrivateKey fromHex(std::string_view hex);

    /**
     * @brief Derives a root key from entropy and password bytes.
     * @param entropy Nonempty entropy bytes.
     * @param password Password bytes, which may be empty; no Unicode normalization is applied.
     * @return The derived root key.
     * @throws Error The entropy is empty or derivation fails.
     */
    static Bip32PrivateKey fromBip39Entropy(const std::vector<std::uint8_t>& entropy, std::string_view password = {});

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
     * @brief Derives a sequence of child keys.
     * @param path Derivation indices; an empty path returns an independent copy. Indices with the high bit set select hardened derivation.
     * @return The derived key.
     * @throws Error An index or curve operation is invalid, or derivation fails.
     */
    Bip32PrivateKey derive(const std::vector<std::uint32_t>& path) const;

    /**
     * @brief Extracts the Ed25519 key without its chain code.
     * @return The Ed25519 key.
     */
    Ed25519PrivateKey toEd25519() const;

    /**
     * @brief Derives the extended public key.
     * @return The public key with its chain code.
     */
    Bip32PublicKey publicKey() const;

    /**
     * @brief Exchanges two values.
     * @param other The value to exchange.
     */
    void swap(Bip32PrivateKey& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source value.
     * @return This instance; unchanged if copying fails.
     */
    Bip32PrivateKey& operator=(const Bip32PrivateKey& other);

    /**
     * @brief Moves another value into this one.
     * @param other The source; self-move is safe.
     * @return This instance.
     */
    Bip32PrivateKey& operator=(Bip32PrivateKey&& other) noexcept;

  private:

    /**
     * @brief Adopts an owned native value.
     * @param value The owned reference transferred to this instance.
     */
    explicit Bip32PrivateKey(cardano_bip32_private_key_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     * @return The borrowed reference.
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_bip32_private_key_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_bip32_private_key_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges values through argument-dependent lookup.
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Bip32PrivateKey& lhs, Bip32PrivateKey& rhs) noexcept;
} // namespace Cometa
