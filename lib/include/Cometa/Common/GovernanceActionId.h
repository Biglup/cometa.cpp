/**
 * @file GovernanceActionId.h
 *
 * @brief A transaction hash and governance action index.
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

struct cardano_governance_action_id_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A transaction hash and governance action index.
 */
class GovernanceActionId
{
  public:

    /**
     * @brief Initializes the governance action identifier.
     *
     * @param hash The 32-byte transaction hash.
     * @param index The action index in the transaction.
     *
     * @throws std::invalid_argument The hash size is invalid.
     */
    GovernanceActionId(const Blake2bHash& hash, std::uint64_t index);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    GovernanceActionId(const GovernanceActionId& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    GovernanceActionId(GovernanceActionId&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~GovernanceActionId();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded bytes.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static GovernanceActionId fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cborHex Hexadecimal CBOR without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static GovernanceActionId fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes a CIP-129 Bech32 identifier.
     *
     * @param bech32 The identifier with the gov_action prefix.
     *
     * @return The decoded identifier.
     *
     * @throws Error The identifier is invalid.
     */
    static GovernanceActionId fromBech32(std::string_view bech32);

  public:

    /**
     * @brief Gets the transaction hash.
     *
     * @return An independent hash value.
     */
    Blake2bHash hash() const;

    /**
     * @brief Replaces the hash.
     *
     * @param hash The 32-byte hash to copy.
     *
     * @throws std::invalid_argument The hash size is not 32 bytes.
     */
    void setHash(const Blake2bHash& hash);

    /**
     * @brief Gets the action index.
     *
     * @return The index in the transaction.
     */
    std::uint64_t index() const;

    /**
     * @brief Replaces the action index.
     *
     * @param index The new index.
     */
    void setIndex(std::uint64_t index);

    /**
     * @brief Encodes the identifier using CIP-129 Bech32.
     *
     * @return The identifier with the gov_action prefix.
     *
     * @throws std::overflow_error The index is greater than 255.
     */
    std::string toBech32() const;

    /**
     * @brief Encodes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values.
     *
     * @param other The value to exchange with this instance.
     */
    void swap(GovernanceActionId& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance. If copying fails, its value is unchanged.
     */
    GovernanceActionId& operator=(const GovernanceActionId& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    GovernanceActionId& operator=(GovernanceActionId&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const GovernanceActionId& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are different.
     */
    bool operator!=(const GovernanceActionId& other) const;

  private:

    /** @brief Allows voting maps to transfer independent values. */
    friend class VotingProcedures;

    /** @brief Allows governance actions to copy identifiers. */
    friend class NoConfidenceAction;

    /** @brief Allows hard fork actions to copy identifiers. */
    friend class HardForkInitiationAction;

    /** @brief Allows parameter change actions to copy identifiers. */
    friend class ParameterChangeAction;

    /** @brief Allows constitution actions to copy identifiers. */
    friend class NewConstitutionAction;

    /** @brief Allows committee updates to copy previous action IDs. */
    friend class UpdateCommitteeAction;

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit GovernanceActionId(cardano_governance_action_id_t* value) noexcept;

  private:

    /**
     * @brief Gets the stored value after checking its lifetime.
     *
     * @return The reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_governance_action_id_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_governance_action_id_t* m_value;
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
void swap(GovernanceActionId& lhs, GovernanceActionId& rhs) noexcept;
} // namespace Cometa
