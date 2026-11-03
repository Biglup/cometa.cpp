/**
 * @file NewConstitutionAction.h
 *
 * @brief A governance action proposing a replacement constitution.
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
#include <Cometa/ProposalProcedures/Constitution.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_new_constitution_action_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A governance action proposing a replacement constitution.
 *
 * Carries the new Constitution and an optional predecessor identifier. Submission creates a proposal;
 * adoption depends on governance approval.
 */
class NewConstitutionAction
{
  public:

    /**
     * @brief Creates a new constitution action.
     * @param constitution The proposed constitution.
     * @param previousAction The previous action in this governance chain, if any.
     */
    explicit NewConstitutionAction(const Constitution& constitution, const std::optional<GovernanceActionId>& previousAction = std::nullopt);

    /**
     * @brief Copies an action.
     * @param other The action to copy.
     */
    NewConstitutionAction(const NewConstitutionAction& other);

    /**
     * @brief Moves an action.
     * @param other The action to move.
     */
    NewConstitutionAction(NewConstitutionAction&& other) noexcept;

    /** @brief Destroys the action. */
    ~NewConstitutionAction();

  public:

    /**
     * @brief Gets the proposed constitution.
     * @return An independent constitution.
     */
    Constitution constitution() const;

    /**
     * @brief Sets the proposed constitution.
     * @param value The constitution to set.
     */
    void setConstitution(const Constitution& value);

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded action.
     */
    static NewConstitutionAction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded action.
     */
    static NewConstitutionAction fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the previous governance action.
     * @return An independent identifier, or empty when absent.
     */
    std::optional<GovernanceActionId> previousAction() const;

    /**
     * @brief Sets the previous governance action.
     * @param value The previous action, or empty to remove it.
     */
    void setPreviousAction(const std::optional<GovernanceActionId>& value);

    /**
     * @brief Encodes CBOR.
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
    void swap(NewConstitutionAction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The action to copy.
     * @return This action.
     */
    NewConstitutionAction& operator=(const NewConstitutionAction& other);

    /**
     * @brief Assigns a moved action.
     * @param other The action to move.
     * @return This action.
     */
    NewConstitutionAction& operator=(NewConstitutionAction&& other) noexcept;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when equal.
     */
    bool operator==(const NewConstitutionAction& other) const;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when different.
     */
    bool operator!=(const NewConstitutionAction& other) const;

  private:

    /** @brief Allows proposal procedures to copy this value. */
    friend class ProposalProcedure;

    /**
     * @brief Adopts an owned action.
     * @param value The owned action.
     */
    explicit NewConstitutionAction(cardano_new_constitution_action_t* value) noexcept;

    /**
     * @brief Gets the stored action.
     * @return The borrowed action.
     */
    cardano_new_constitution_action_t* checked() const;

  private:

    /** @brief The owned action. */
    cardano_new_constitution_action_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges no-confidence actions.
 * @param lhs The first action.
 * @param rhs The second action.
 */
void swap(NewConstitutionAction& lhs, NewConstitutionAction& rhs) noexcept;
} // namespace Cometa
