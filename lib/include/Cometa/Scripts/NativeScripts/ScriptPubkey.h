/**
 * @file ScriptPubkey.h
 *
 * @brief A native script requiring a signature from the identified key.
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

struct cardano_script_pubkey_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A native script requiring a signature from the identified key.
 *
 * The 28-byte key hash identifies the verification key whose transaction witness is required. The
 * script contains the hash, not a public key or signature.
 */
class ScriptPubkey
{
  public:

    /**
     * @brief Initializes the script requirement.
     *
     * @param keyHash The 28-byte key hash to copy.
     */
    explicit ScriptPubkey(const Blake2bHash& keyHash);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    ScriptPubkey(const ScriptPubkey& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    ScriptPubkey(ScriptPubkey&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~ScriptPubkey();

  public:

    /**
     * @brief Decodes exactly one CBOR script.
     *
     * @param cbor The encoded script; empty input and trailing bytes are rejected.
     *
     * @return The decoded script.
     */
    static ScriptPubkey fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded script.
     */
    static ScriptPubkey fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes cardano-cli native-script JSON.
     *
     * @param json The object with type "sig" and its credential hash.
     *
     * @return The decoded script.
     *
     * @remark This input schema differs from the CIP-116 output of toJson().
     */
    static ScriptPubkey fromJson(std::string_view json);

  public:

    /**
     * @brief Gets an independent key hash.
     *
     * @return The keyHash value.
     */
    Blake2bHash keyHash() const;

    /**
     * @brief Replaces the required key hash.
     *
     * @param keyHash The replacement value to copy.
     */
    void setKeyHash(const Blake2bHash& keyHash);

    /**
     * @brief Serializes the script as CBOR.
     *
     * @return The encoded bytes; original encodings are preserved until mutation.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the script as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Rebuilds subsequent CBOR from the current fields.
     *
     * @remark Normalization may change a script hash without changing the requirement.
     */
    void normalizeEncoding();

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(ScriptPubkey& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance.
     *
     * @remark If copying fails, this instance is unchanged.
     */
    ScriptPubkey& operator=(const ScriptPubkey& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    ScriptPubkey& operator=(ScriptPubkey&& other) noexcept;

    /**
     * @brief Compares script requirements.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements are equal.
     */
    bool operator==(const ScriptPubkey& other) const;

    /**
     * @brief Compares script requirements for inequality.
     *
     * @param other The script to compare.
     *
     * @return True when the requirements differ.
     */
    bool operator!=(const ScriptPubkey& other) const;

  private:

    /** @brief Allows NativeScript to copy this script variant. */
    friend class NativeScript;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit ScriptPubkey(cardano_script_pubkey_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_script_pubkey_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_script_pubkey_t* m_value;
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
void swap(ScriptPubkey& lhs, ScriptPubkey& rhs) noexcept;
} // namespace Cometa
