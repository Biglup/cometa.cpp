/**
 * @file DRep.h
 *
 * @brief Delegated representative value.
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

#include <Cometa/Common/Credential.h>
#include <Cometa/Common/DRepType.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_drep_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A delegated representative or a predefined voting choice.
 */
class DRep
{
  public:

    /**
     * @brief Creates a DRep choice.
     * @param type The kind of DRep.
     * @param credential A credential for key or script DReps; none for predefined choices.
     */
    explicit DRep(DRepType type, std::optional<Credential> credential = std::nullopt);

    /**
     * @brief Creates a key or script DRep from a credential.
     * @param credential The representative's credential.
     */
    explicit DRep(const Credential& credential);

    /**
     * @brief Copies a DRep.
     * @param other The DRep to copy.
     */
    DRep(const DRep& other);

    /**
     * @brief Moves a DRep.
     * @param other The DRep to move.
     */
    DRep(DRep&& other) noexcept;

    /** @brief Destroys the DRep. */
    ~DRep();

  public:

    /**
     * @brief Decodes one CBOR DRep choice.
     * @param cbor The encoded choice.
     * @return The decoded DRep.
     */
    static DRep fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded DRep.
     */
    static DRep fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes a bech32 DRep identifier.
     * @param value The encoded identifier.
     * @return The decoded DRep.
     */
    static DRep fromString(std::string_view value);

  public:

    /**
     * @brief Gets the DRep kind.
     * @return The kind.
     */
    DRepType type() const;

    /**
     * @brief Gets the representative credential, if present.
     * @return An independent credential or no value.
     */
    std::optional<Credential> credential() const;

    /**
     * @brief Replaces the representative credential.
     * @param value A credential matching the DRep kind.
     */
    void setCredential(const Credential& value);

    /**
     * @brief Replaces the choice and its credential.
     * @param type The new kind.
     * @param value A credential for key or script DReps; none for predefined choices.
     */
    void setType(DRepType type, std::optional<Credential> value = std::nullopt);

    /**
     * @brief Encodes the DRep as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the DRep as a bech32 identifier.
     * @return The identifier.
     */
    std::string toString() const;

    /**
     * @brief Formats the DRep as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges DReps.
     * @param other The DRep to exchange.
     */
    void swap(DRep& other) noexcept;

  public:

    /**
     * @brief Assigns a copy.
     * @param other The DRep to copy.
     * @return This DRep.
     */
    DRep& operator=(const DRep& other);

    /**
     * @brief Assigns a moved DRep.
     * @param other The DRep to move.
     * @return This DRep.
     */
    DRep& operator=(DRep&& other) noexcept;

    /**
     * @brief Compares DReps.
     * @param other The DRep to compare.
     * @return True when equal.
     */
    bool operator==(const DRep& other) const;

    /**
     * @brief Compares DReps.
     * @param other The DRep to compare.
     * @return True when different.
     */
    bool operator!=(const DRep& other) const;

  private:

    /**
     * @brief Adopts an owned DRep.
     * @param value The owned DRep.
     */
    explicit DRep(cardano_drep_t* value) noexcept;

    /**
     * @brief Gets the stored DRep.
     * @return The borrowed DRep.
     */
    cardano_drep_t* checked() const;

    /**
     * @brief Checks consistency between type and credential.
     * @param type The DRep kind.
     * @param value The optional credential.
     */
    static void validate(DRepType type, const std::optional<Credential>& value);

    /** @brief Allows certificate values to copy DRep references. */
    friend class VoteDelegationCert;

    /** @brief Allows combined delegation certificates to copy DRep references. */
    friend class StakeVoteDelegationCert;

    /** @brief Allows vote registration certificates to copy DRep references. */
    friend class VoteRegistrationDelegationCert;

  private:

    /** @brief The owned DRep. */
    cardano_drep_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges DReps through argument-dependent lookup.
 * @param lhs The first DRep.
 * @param rhs The second DRep.
 */
void swap(DRep& lhs, DRep& rhs) noexcept;
} // namespace Cometa
