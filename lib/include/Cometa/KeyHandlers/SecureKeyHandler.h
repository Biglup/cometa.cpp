/**
 * @file SecureKeyHandler.h
 *
 * @brief A signer that can be passed to native transaction operations.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

#include <Cometa/Crypto/Ed25519PrivateKey.h>
#include <Cometa/KeyHandlers/Bip32KeyCallbacks.h>
#include <Cometa/KeyHandlers/Ed25519KeyCallbacks.h>
#include <Cometa/KeyHandlers/PassphraseProvider.h>
#include <Cometa/KeyHandlers/SecureKeyHandlerType.h>

#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_secure_key_handler_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A BIP32 or Ed25519 signer with automatic lifetime management. */
class SecureKeyHandler
{
  public:

    /**
     * @brief Creates a BIP32 signer from C++ operations.
     * @param name A short, nonempty handler name.
     * @param callbacks The required signing and public-key operations.
     * @return The handler.
     */
    static SecureKeyHandler bip32(std::string_view name, Bip32KeyCallbacks callbacks);

    /**
     * @brief Creates an Ed25519 signer from C++ operations.
     * @param name A short, nonempty handler name.
     * @param callbacks The required signing and public-key operations.
     * @return The handler.
     */
    static SecureKeyHandler ed25519(std::string_view name, Ed25519KeyCallbacks callbacks);

    /**
     * @brief Creates an encrypted BIP32 signer from wallet entropy.
     * @param entropy The BIP39 entropy to encrypt.
     * @param passphrase The encryption passphrase.
     * @param provider Supplies the passphrase for later signing operations.
     * @return The encrypted signer.
     */
    static SecureKeyHandler softwareBip32(const std::vector<std::uint8_t>& entropy, const std::vector<std::uint8_t>& passphrase, PassphraseProvider provider);

    /**
     * @brief Creates an encrypted Ed25519 signer.
     * @param key The signing key to encrypt.
     * @param passphrase The encryption passphrase.
     * @param provider Supplies the passphrase for later signing operations.
     * @return The encrypted signer.
     */
    static SecureKeyHandler softwareEd25519(const Ed25519PrivateKey& key, const std::vector<std::uint8_t>& passphrase, PassphraseProvider provider);

    /**
     * @brief Restores a serialized encrypted software signer.
     * @param bytes The serialized signer.
     * @param provider Supplies the passphrase for signing operations.
     * @return The restored signer.
     */
    static SecureKeyHandler deserializeSoftware(const std::vector<std::uint8_t>& bytes, PassphraseProvider provider);

    /**
     * @brief Copies a handler.
     * @param other The handler to copy.
     */
    SecureKeyHandler(const SecureKeyHandler& other);

    /**
     * @brief Moves a handler.
     * @param other The handler to move.
     */
    SecureKeyHandler(SecureKeyHandler&& other) noexcept;

    /** @brief Destroys the handler. */
    ~SecureKeyHandler();

  public:

    /**
     * @brief Gets the handler name.
     * @return The configured name.
     */
    std::string name() const;

    /**
     * @brief Gets the most recent handler error message.
     * @return The message, or an empty string when none is available.
     */
    std::string lastError() const;

    /**
     * @brief Gets the key scheme.
     * @return The handler type.
     */
    SecureKeyHandlerType type() const;

    /**
     * @brief Signs a transaction with BIP32 key paths.
     * @param transaction The transaction to sign.
     * @param paths The key paths to sign with.
     * @return The produced witnesses.
     */
    VkeyWitnessSet signTransaction(const Transaction& transaction, const std::vector<DerivationPath>& paths) const;

    /**
     * @brief Signs a sub-transaction with BIP32 key paths.
     * @param transaction The sub-transaction to sign.
     * @param paths The key paths to sign with.
     * @return The produced witnesses.
     */
    VkeyWitnessSet signSubTransaction(const SubTransaction& transaction, const std::vector<DerivationPath>& paths) const;

    /**
     * @brief Signs a transaction with an Ed25519 key.
     * @param transaction The transaction to sign.
     * @return The produced witnesses.
     */
    VkeyWitnessSet signTransaction(const Transaction& transaction) const;

    /**
     * @brief Signs a sub-transaction with an Ed25519 key.
     * @param transaction The sub-transaction to sign.
     * @return The produced witnesses.
     */
    VkeyWitnessSet signSubTransaction(const SubTransaction& transaction) const;

    /**
     * @brief Gets a BIP32 extended account public key.
     * @param path The account derivation path.
     * @return The extended public key.
     */
    Bip32PublicKey accountPublicKey(const AccountDerivationPath& path) const;

    /**
     * @brief Gets the Ed25519 public key.
     * @return The public key.
     */
    Ed25519PublicKey publicKey() const;

    /**
     * @brief Serializes the handler when the operation is available.
     * @return The serialized bytes.
     */
    std::vector<std::uint8_t> serialize() const;

    /**
     * @brief Exchanges handlers.
     * @param other The handler to exchange.
     */
    void swap(SecureKeyHandler& other) noexcept;

  public:

    /**
     * @brief Assigns a handler.
     * @param other The handler to copy.
     * @return This handler.
     */
    SecureKeyHandler& operator=(const SecureKeyHandler& other);

    /**
     * @brief Assigns a moved handler.
     * @param other The handler to move.
     * @return This handler.
     */
    SecureKeyHandler& operator=(SecureKeyHandler&& other) noexcept;

  private:

    /**
     * @brief Adopts a handler.
     * @param value The owned handler.
     * @param type The key scheme.
     * @param provider The passphrase operation, when this is a software signer.
     */
    SecureKeyHandler(cardano_secure_key_handler_t* value, SecureKeyHandlerType type, std::shared_ptr<PassphraseProvider> provider = nullptr) noexcept;

    /**
     * @brief Gets the stored handler.
     * @return The borrowed handler.
     */
    cardano_secure_key_handler_t* checked() const;

  private:

    /** @brief The owned signer. */
    cardano_secure_key_handler_t* m_value;

    /** @brief Its key scheme. */
    SecureKeyHandlerType m_type;

    /** @brief The passphrase operation for an encrypted software signer. */
    std::shared_ptr<PassphraseProvider> m_passphrase;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges handlers through argument-dependent lookup.
 * @param lhs The first handler.
 * @param rhs The second handler.
 */
void swap(SecureKeyHandler& lhs, SecureKeyHandler& rhs) noexcept;
} // namespace Cometa
