/**
 * @file Credential.h
 *
 * @brief Declaration of the Credential value type.
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

#include <Cometa/Common/CredentialType.h>
#include <Cometa/Crypto/Blake2bHash.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_credential_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Identifies a key or script authorized to perform an action.
 */
class Credential
{
  public:

    /**
     * @brief Initializes a credential from its type and hash.
     *
     * @param type The key-hash or script-hash discriminator.
     * @param hash The 28-byte credential hash; its bytes are copied.
     *
     * @throws std::invalid_argument The type is unknown or the hash is not 28 bytes.
     */
    Credential(CredentialType type, const Blake2bHash& hash);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    Credential(const Credential& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The value to move.
     *
     * @remark After the move, other can be destroyed, assigned, or swapped; observing it throws std::logic_error.
     */
    Credential(Credential&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Credential();

  public:

    /**
     * @brief Creates a value from exactly one CBOR item.
     *
     * @param cbor The encoded value.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument The input is empty or bytes remain after the decoded item.
     * @throws Error The CBOR input is invalid.
     */
    static Credential fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Creates a value from hexadecimal CBOR.
     *
     * @param cborHex The CBOR bytes as pairs of hexadecimal digits, without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument The hex is empty, invalid, or contains bytes after the decoded item.
     * @throws Error The CBOR input is invalid.
     */
    static Credential fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets an independent copy of the credential hash.
     *
     * @return The 28-byte hash.
     */
    Blake2bHash hash() const;

    /**
     * @brief Replaces the credential hash.
     *
     * @param hash The 28-byte hash to copy.
     *
     * @throws std::invalid_argument The hash length is not 28 bytes.
     */
    void setHash(const Blake2bHash& hash);

    /**
     * @brief Gets the credential discriminator.
     *
     * @return The key-hash or script-hash discriminator.
     */
    CredentialType type() const;

    /**
     * @brief Changes the discriminator without changing the hash.
     *
     * @param type The new discriminator.
     *
     * @throws std::invalid_argument The type is unknown.
     */
    void setType(CredentialType type);

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges values without allocating.
     *
     * @param other The value to exchange with this instance.
     *
     * @remark Moved-from values may be swapped.
     */
    void swap(Credential& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance.
     *
     * @remark Self-assignment is safe. If copying fails, this instance is unchanged.
     */
    Credential& operator=(const Credential& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source, which becomes moved-from unless this is self-assignment.
     *
     * @return This instance.
     *
     * @remark Self-move assignment is safe.
     */
    Credential& operator=(Credential&& other) noexcept;

    /**
     * @brief Compares values for equality.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const Credential& other) const;

    /**
     * @brief Compares values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True if the values differ.
     */
    bool operator!=(const Credential& other) const;

    /**
     * @brief Orders values for standard containers.
     *
     * @param other The value to compare.
     *
     * @return True if this value precedes the other value.
     */
    bool operator<(const Credential& other) const;

  private:

    /** @brief Allows ScriptRequireGuard to copy script components. */
    friend class ScriptRequireGuard;

    /** @brief Allows CredentialSet to copy set elements. */
    friend class CredentialSet;

    /** @brief Allows BaseAddress to copy credential components. */
    friend class BaseAddress;

    /** @brief Allows EnterpriseAddress to copy credential components. */
    friend class EnterpriseAddress;

    /** @brief Allows RewardAddress to copy credential components. */
    friend class RewardAddress;

    /** @brief Allows PointerAddress to copy credential components. */
    friend class PointerAddress;

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference whose ownership is transferred to this instance.
     */
    explicit Credential(cardano_credential_t* value) noexcept;

  private:

    /**
     * @brief Gets the value after verifying that it is available.
     *
     * @return The stored reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_credential_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_credential_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges two values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(Credential& lhs, Credential& rhs) noexcept;
} // namespace Cometa
