/**
 * @file BootstrapWitness.h
 *
 * @brief A Byron spending witness containing a verification key, signature, chain code, and address attributes.
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

#include <Cometa/Crypto/Ed25519PublicKey.h>
#include <Cometa/Crypto/Ed25519Signature.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_bootstrap_witness_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A Byron spending witness containing a verification key, signature, chain code, and address attributes.
 *
 * The additional chain code and attributes are needed to reconstruct the legacy address identity. Use
 * VkeyWitness for ordinary Shelley key witnesses.
 */
class BootstrapWitness
{
  public:

    /**
     * @brief Creates a key witness.
     * @param publicKey The verification key.
     * @param signature The transaction signature.
     * @param chainCode The 32-byte chain code.
     * @param attributes The bootstrap address attributes.
     * @throws std::invalid_argument The chain code is not 32 bytes.
     */
    BootstrapWitness(const Ed25519PublicKey& publicKey, const Ed25519Signature& signature, const std::vector<std::uint8_t>& chainCode, const std::vector<std::uint8_t>& attributes);

    /**
     * @brief Copies a key witness.
     * @param other The witness to copy.
     */
    BootstrapWitness(const BootstrapWitness& other);

    /**
     * @brief Moves a key witness.
     * @param other The witness to move.
     */
    BootstrapWitness(BootstrapWitness&& other) noexcept;

    /** @brief Destroys the witness. */
    ~BootstrapWitness();

  public:

    /**
     * @brief Decodes a key witness from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded witness.
     */
    static BootstrapWitness fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded witness.
     */
    static BootstrapWitness fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the verification key.
     * @return An independent verification key.
     */
    Ed25519PublicKey publicKey() const;

    /**
     * @brief Sets the verification key.
     * @param value The new verification key.
     */
    void setPublicKey(const Ed25519PublicKey& value);

    /**
     * @brief Gets the transaction signature.
     * @return An independent signature.
     */
    Ed25519Signature signature() const;

    /**
     * @brief Sets the transaction signature.
     * @param value The new signature.
     */
    void setSignature(const Ed25519Signature& value);

    /**
     * @brief Gets the chain code.
     * @return Independent chain code bytes.
     */
    std::vector<std::uint8_t> chainCode() const;

    /**
     * @brief Sets the chain code.
     * @param value The new chain code bytes.
     * @throws std::invalid_argument The chain code is not 32 bytes.
     */
    void setChainCode(const std::vector<std::uint8_t>& value);

    /**
     * @brief Gets the bootstrap address attributes.
     * @return Independent attribute bytes.
     */
    std::vector<std::uint8_t> attributes() const;

    /**
     * @brief Sets the bootstrap address attributes.
     * @param value The new attribute bytes.
     */
    void setAttributes(const std::vector<std::uint8_t>& value);

    /**
     * @brief Encodes the witness as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the witness as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges witnesses.
     * @param other The witness to exchange.
     */
    void swap(BootstrapWitness& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The witness to copy.
     * @return This witness.
     */
    BootstrapWitness& operator=(const BootstrapWitness& other);

    /**
     * @brief Assigns a moved witness.
     * @param other The witness to move.
     * @return This witness.
     */
    BootstrapWitness& operator=(BootstrapWitness&& other) noexcept;

    /**
     * @brief Compares witnesses.
     * @param other The witness to compare.
     * @return True when equal.
     */
    bool operator==(const BootstrapWitness& other) const;

    /**
     * @brief Compares witnesses.
     * @param other The witness to compare.
     * @return True when different.
     */
    bool operator!=(const BootstrapWitness& other) const;

  private:

    /** @brief Allows witness sets to copy their elements. */
    friend class BootstrapWitnessSet;

    /**
     * @brief Adopts an owned witness.
     * @param value The owned witness.
     */
    explicit BootstrapWitness(cardano_bootstrap_witness_t* value) noexcept;

    /**
     * @brief Gets the stored witness.
     * @return The borrowed witness.
     */
    cardano_bootstrap_witness_t* checked() const;

  private:

    /** @brief The owned witness. */
    cardano_bootstrap_witness_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges key witnesses.
 * @param lhs The first witness.
 * @param rhs The second witness.
 */
void swap(BootstrapWitness& lhs, BootstrapWitness& rhs) noexcept;
} // namespace Cometa
