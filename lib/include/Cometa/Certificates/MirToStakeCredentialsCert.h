/**
 * @file MirToStakeCredentialsCert.h
 *
 * @brief A historical allocation of instantaneous rewards to staking credentials.
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

#include <Cometa/Certificates/MirPotType.h>
#include <Cometa/Common/Credential.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_mir_to_stake_creds_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A historical allocation of instantaneous rewards to staking credentials.
 *
 * The source is the reserves or treasury pot. Entries describe reward adjustments for credentials, not
 * transaction outputs.
 */
class MirToStakeCredentialsCert
{
  public:

    /** @brief Creates an empty distribution from the selected pot.
     * @param pot The source pot.
     */
    explicit MirToStakeCredentialsCert(MirPotType pot);

    /** @brief Copies a distribution.
     * @param other The distribution to copy.
     */
    MirToStakeCredentialsCert(const MirToStakeCredentialsCert& other);

    /** @brief Moves a distribution.
     * @param other The distribution to move.
     */
    MirToStakeCredentialsCert(MirToStakeCredentialsCert&& other) noexcept;

    /** @brief Destroys the distribution. */
    ~MirToStakeCredentialsCert();

  public:

    /** @brief Decodes one CBOR distribution.
     * @param cbor The encoded distribution.
     * @return The decoded value.
     */
    static MirToStakeCredentialsCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /** @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded value.
     */
    static MirToStakeCredentialsCert fromCbor(std::string_view cborHex);

  public:

    /** @brief Gets the source pot.
     * @return The selected pot.
     */
    MirPotType pot() const;

    /** @brief Replaces the source pot.
     * @param value The new pot.
     */
    void setPot(MirPotType value);

    /** @brief Gets the number of credential entries.
     * @return The number of entries.
     */
    std::size_t size() const;

    /** @brief Adds a reward for one credential.
     * @param credential The recipient credential.
     * @param amount The reward in lovelace.
     */
    void insert(const Credential& credential, std::uint64_t amount);

    /** @brief Gets the credential and amount at an index.
     * @param index The zero-based entry index.
     * @return An independent credential and its reward in lovelace.
     */
    std::pair<Credential, std::uint64_t> at(std::size_t index) const;

    /** @brief Encodes the distribution as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /** @brief Formats the distribution as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /** @brief Exchanges distributions.
     * @param other The distribution to exchange.
     */
    void swap(MirToStakeCredentialsCert& other) noexcept;

  public:

    /** @brief Assigns a copy.
     * @param other The distribution to copy.
     * @return This distribution.
     */
    MirToStakeCredentialsCert& operator=(const MirToStakeCredentialsCert& other);

    /** @brief Assigns a moved distribution.
     * @param other The distribution to move.
     * @return This distribution.
     */
    MirToStakeCredentialsCert& operator=(MirToStakeCredentialsCert&& other) noexcept;

    /** @brief Compares distributions.
     * @param other The distribution to compare.
     * @return True when equal.
     */
    bool operator==(const MirToStakeCredentialsCert& other) const;

    /** @brief Compares distributions.
     * @param other The distribution to compare.
     * @return True when different.
     */
    bool operator!=(const MirToStakeCredentialsCert& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static MirToStakeCredentialsCert adopt(cardano_mir_to_stake_creds_cert_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_mir_to_stake_creds_cert_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_mir_to_stake_creds_cert_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_mir_to_stake_creds_cert_t* release() noexcept;

  private:

    /** @brief Adopts an owned distribution.
     * @param value The owned distribution.
     */
    explicit MirToStakeCredentialsCert(cardano_mir_to_stake_creds_cert_t* value) noexcept;

    /** @brief Gets the stored distribution.
     * @return The borrowed distribution.
     */
    cardano_mir_to_stake_creds_cert_t* checked() const;

    /** @brief Checks a pot discriminator.
     * @param value The pot to validate.
     */
    static void validatePot(MirPotType value);

  private:

    /** @brief The owned certificate. */
    cardano_mir_to_stake_creds_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges distributions through argument-dependent lookup.
 *
 * @param lhs The first distribution.
 * @param rhs The second distribution.
 */
void swap(MirToStakeCredentialsCert& lhs, MirToStakeCredentialsCert& rhs) noexcept;
} // namespace Cometa
