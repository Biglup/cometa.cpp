/**
 * @file VkeyWitness.h
 *
 * @brief Verification-key transaction witness.
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

struct cardano_vkey_witness_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief A verification key and its transaction signature. */
class VkeyWitness
{
  public:

    /**
     * @brief Creates a key witness.
     * @param publicKey The verification key.
     * @param signature The transaction signature.
     */
    VkeyWitness(const Ed25519PublicKey& publicKey, const Ed25519Signature& signature);

    /**
     * @brief Copies a key witness.
     * @param other The witness to copy.
     */
    VkeyWitness(const VkeyWitness& other);

    /**
     * @brief Moves a key witness.
     * @param other The witness to move.
     */
    VkeyWitness(VkeyWitness&& other) noexcept;

    /** @brief Destroys the witness. */
    ~VkeyWitness();

  public:

    /**
     * @brief Decodes a key witness from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded witness.
     */
    static VkeyWitness fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded witness.
     */
    static VkeyWitness fromCbor(std::string_view cborHex);

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
     * @brief Checks whether this witness uses a key.
     * @param value The verification key to compare.
     * @return True when the keys match.
     */
    bool hasPublicKey(const Ed25519PublicKey& value) const;

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
    void swap(VkeyWitness& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The witness to copy.
     * @return This witness.
     */
    VkeyWitness& operator=(const VkeyWitness& other);

    /**
     * @brief Assigns a moved witness.
     * @param other The witness to move.
     * @return This witness.
     */
    VkeyWitness& operator=(VkeyWitness&& other) noexcept;

    /**
     * @brief Compares witnesses.
     * @param other The witness to compare.
     * @return True when equal.
     */
    bool operator==(const VkeyWitness& other) const;

    /**
     * @brief Compares witnesses.
     * @param other The witness to compare.
     * @return True when different.
     */
    bool operator!=(const VkeyWitness& other) const;

  private:

    /** @brief Allows witness sets to copy their elements. */
    friend class VkeyWitnessSet;

    /**
     * @brief Adopts an owned witness.
     * @param value The owned witness.
     */
    explicit VkeyWitness(cardano_vkey_witness_t* value) noexcept;

    /**
     * @brief Gets the stored witness.
     * @return The borrowed witness.
     */
    cardano_vkey_witness_t* checked() const;

  private:

    /** @brief The owned witness. */
    cardano_vkey_witness_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges key witnesses.
 * @param lhs The first witness.
 * @param rhs The second witness.
 */
void swap(VkeyWitness& lhs, VkeyWitness& rhs) noexcept;
} // namespace Cometa
