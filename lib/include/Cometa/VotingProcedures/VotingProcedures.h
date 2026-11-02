/**
 * @file VotingProcedures.h
 *
 * @brief Governance votes grouped by voter and action.
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

#include <Cometa/Common/GovernanceActionId.h>
#include <Cometa/VotingProcedures/GovernanceActionIdList.h>
#include <Cometa/VotingProcedures/Voter.h>
#include <Cometa/VotingProcedures/VoterList.h>
#include <Cometa/VotingProcedures/VotingProcedure.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_voting_procedures_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Governance votes grouped by voter and action identifier. */
class VotingProcedures
{
  public:

    /** @brief Creates an empty voting map. */
    VotingProcedures();

    /**
     * @brief Copies a voting map.
     * @param other The map to copy.
     */
    VotingProcedures(const VotingProcedures& other);

    /**
     * @brief Moves a voting map.
     * @param other The map to move.
     */
    VotingProcedures(VotingProcedures&& other) noexcept;

    /** @brief Destroys the voting map. */
    ~VotingProcedures();

  public:

    /**
     * @brief Decodes a voting map from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded map.
     */
    static VotingProcedures fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded map.
     */
    static VotingProcedures fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Adds a vote for a voter and action.
     * @param voter The voter.
     * @param actionId The governance action identifier.
     * @param procedure The voting procedure.
     * @throws std::invalid_argument A vote already exists for this voter and action.
     */
    void insert(const Voter& voter, const GovernanceActionId& actionId, const VotingProcedure& procedure);

    /**
     * @brief Gets a vote for a voter and action.
     * @param voter The voter.
     * @param actionId The governance action identifier.
     * @return An independent vote, or empty when absent.
     */
    std::optional<VotingProcedure> get(const Voter& voter, const GovernanceActionId& actionId) const;

    /**
     * @brief Gets the voters in canonical order.
     * @return Independent voter values.
     */
    VoterList voters() const;

    /**
     * @brief Gets the action identifiers voted on by a voter.
     * @param voter The voter.
     * @return Independent action identifiers, or an empty list when absent.
     */
    GovernanceActionIdList actionIds(const Voter& voter) const;

    /**
     * @brief Encodes the map as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the map as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges maps.
     * @param other The map to exchange.
     */
    void swap(VotingProcedures& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The map to copy.
     * @return This map.
     */
    VotingProcedures& operator=(const VotingProcedures& other);

    /**
     * @brief Assigns a moved map.
     * @param other The map to move.
     * @return This map.
     */
    VotingProcedures& operator=(VotingProcedures&& other) noexcept;

    /**
     * @brief Compares voting maps.
     * @param other The map to compare.
     * @return True when equal.
     */
    bool operator==(const VotingProcedures& other) const;

    /**
     * @brief Compares voting maps.
     * @param other The map to compare.
     * @return True when different.
     */
    bool operator!=(const VotingProcedures& other) const;

  private:

    /**
     * @brief Adopts an owned voting map.
     * @param value The owned map.
     */
    explicit VotingProcedures(cardano_voting_procedures_t* value) noexcept;

    /**
     * @brief Gets the stored voting map.
     * @return The borrowed map.
     */
    cardano_voting_procedures_t* checked() const;

  private:

    /** @brief The owned voting map. */
    cardano_voting_procedures_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges voting maps.
 * @param lhs The first map.
 * @param rhs The second map.
 */
void swap(VotingProcedures& lhs, VotingProcedures& rhs) noexcept;
} // namespace Cometa
