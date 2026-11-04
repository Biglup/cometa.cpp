/**
 * @file ParameterChangeAction.h
 *
 * @brief A governance action proposing selected protocol parameter changes.
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
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/ProtocolParams/ProtocolParamUpdate.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_parameter_change_action_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A governance action proposing selected protocol parameter changes.
 *
 * Only fields present in ProtocolParamUpdate are proposed for change. The optional policy hash
 * identifies the guardrails script governing the action.
 */
class ParameterChangeAction
{
  public:

    /**
     * @brief Creates a parameter change action.
     * @param update The proposed parameter changes.
     * @param previousAction The previous action in this governance chain, if any.
     * @param policyHash The policy script hash, if required.
     */
    explicit ParameterChangeAction(
      const ProtocolParamUpdate&               update,
      const std::optional<GovernanceActionId>& previousAction = std::nullopt,
      const std::optional<Blake2bHash>&        policyHash     = std::nullopt);

    /**
     * @brief Copies an action.
     * @param other The action to copy.
     */
    ParameterChangeAction(const ParameterChangeAction& other);

    /**
     * @brief Moves an action.
     * @param other The action to move.
     */
    ParameterChangeAction(ParameterChangeAction&& other) noexcept;

    /** @brief Destroys the action. */
    ~ParameterChangeAction();

  public:

    /**
     * @brief Gets the proposed parameter changes.
     * @return An independent update.
     */
    ProtocolParamUpdate update() const;

    /**
     * @brief Sets the proposed parameter changes.
     * @param value The update to set.
     */
    void setUpdate(const ProtocolParamUpdate& value);

    /**
     * @brief Gets the policy script hash.
     * @return An independent hash, or empty when absent.
     */
    std::optional<Blake2bHash> policyHash() const;

    /**
     * @brief Sets the policy script hash.
     * @param value The policy hash, or empty to remove it.
     */
    void setPolicyHash(const std::optional<Blake2bHash>& value);

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded action.
     */
    static ParameterChangeAction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded action.
     */
    static ParameterChangeAction fromCbor(std::string_view cborHex);

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
    void swap(ParameterChangeAction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The action to copy.
     * @return This action.
     */
    ParameterChangeAction& operator=(const ParameterChangeAction& other);

    /**
     * @brief Assigns a moved action.
     * @param other The action to move.
     * @return This action.
     */
    ParameterChangeAction& operator=(ParameterChangeAction&& other) noexcept;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when equal.
     */
    bool operator==(const ParameterChangeAction& other) const;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when different.
     */
    bool operator!=(const ParameterChangeAction& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static ParameterChangeAction adopt(cardano_parameter_change_action_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_parameter_change_action_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_parameter_change_action_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_parameter_change_action_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned action.
     * @param value The owned action.
     */
    explicit ParameterChangeAction(cardano_parameter_change_action_t* value) noexcept;

    /**
     * @brief Gets the stored action.
     * @return The borrowed action.
     */
    cardano_parameter_change_action_t* checked() const;

  private:

    /** @brief The owned action. */
    cardano_parameter_change_action_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges no-confidence actions.
 * @param lhs The first action.
 * @param rhs The second action.
 */
void swap(ParameterChangeAction& lhs, ParameterChangeAction& rhs) noexcept;
} // namespace Cometa
