/**
 * @file UpdateCommitteeAction.h
 *
 * @brief A governance action proposing committee membership and quorum changes.
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

#include <Cometa/Common/CredentialSet.h>
#include <Cometa/Common/GovernanceActionId.h>
#include <Cometa/Common/UnitInterval.h>
#include <Cometa/ProposalProcedures/CommitteeMembersMap.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_update_committee_action_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A governance action proposing committee membership and quorum changes.
 *
 * Removals identify cold credentials; additions pair cold credentials with expiry epochs. The quorum
 * specifies the proposed voting threshold.
 */
class UpdateCommitteeAction
{
  public:

    /**
     * @brief Creates an update committee action.
     * @param removed The member credentials to remove.
     * @param added The members and expiration epochs to add.
     * @param quorum The new quorum threshold.
     * @param previousAction The optional previous committee action.
     */
    UpdateCommitteeAction(const CredentialSet& removed, const CommitteeMembersMap& added, const UnitInterval& quorum, const std::optional<GovernanceActionId>& previousAction = std::nullopt);

    /**
     * @brief Copies an action.
     * @param other The action to copy.
     */
    UpdateCommitteeAction(const UpdateCommitteeAction& other);

    /**
     * @brief Moves an action.
     * @param other The action to move.
     */
    UpdateCommitteeAction(UpdateCommitteeAction&& other) noexcept;

    /** @brief Destroys the action. */
    ~UpdateCommitteeAction();

  public:

    /**
     * @brief Decodes an action from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded action.
     */
    static UpdateCommitteeAction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded action.
     */
    static UpdateCommitteeAction fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the member credentials to remove.
     * @return An independent set.
     */
    CredentialSet removedMembers() const;

    /**
     * @brief Sets the member credentials to remove.
     * @param value The new set.
     */
    void setRemovedMembers(const CredentialSet& value);

    /**
     * @brief Gets the members to add.
     * @return An independent member map.
     */
    CommitteeMembersMap addedMembers() const;

    /**
     * @brief Sets the members to add.
     * @param value The new member map.
     */
    void setAddedMembers(const CommitteeMembersMap& value);

    /**
     * @brief Gets the new quorum threshold.
     * @return An independent interval.
     */
    UnitInterval quorum() const;

    /**
     * @brief Sets the new quorum threshold.
     * @param value The new interval.
     */
    void setQuorum(const UnitInterval& value);

    /**
     * @brief Gets the preceding committee action.
     * @return An independent ID, or empty when absent.
     */
    std::optional<GovernanceActionId> previousAction() const;

    /**
     * @brief Sets the preceding committee action.
     * @param value The ID, or empty to remove it.
     */
    void setPreviousAction(const std::optional<GovernanceActionId>& value);

    /**
     * @brief Encodes the action as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the action as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges actions.
     * @param other The action to exchange.
     */
    void swap(UpdateCommitteeAction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The action to copy.
     * @return This action.
     */
    UpdateCommitteeAction& operator=(const UpdateCommitteeAction& other);

    /**
     * @brief Assigns a moved action.
     * @param other The action to move.
     * @return This action.
     */
    UpdateCommitteeAction& operator=(UpdateCommitteeAction&& other) noexcept;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when equal.
     */
    bool operator==(const UpdateCommitteeAction& other) const;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when different.
     */
    bool operator!=(const UpdateCommitteeAction& other) const;

  private:

    /** @brief Allows proposal procedures to copy this value. */
    friend class ProposalProcedure;

    /**
     * @brief Adopts an owned action.
     * @param value The owned action.
     */
    explicit UpdateCommitteeAction(cardano_update_committee_action_t* value) noexcept;

    /**
     * @brief Gets the stored action.
     * @return The borrowed action.
     */
    cardano_update_committee_action_t* checked() const;

  private:

    /** @brief The owned action. */
    cardano_update_committee_action_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges update committee actions.
 * @param lhs The first action.
 * @param rhs The second action.
 */
void swap(UpdateCommitteeAction& lhs, UpdateCommitteeAction& rhs) noexcept;
} // namespace Cometa
