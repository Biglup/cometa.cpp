/**
 * @file ProposalProcedure.h
 *
 * @brief A governance action together with its deposit, refund account, and metadata.
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

#include <Cometa/Address/RewardAddress.h>
#include <Cometa/Common/Anchor.h>
#include <Cometa/ProposalProcedures/GovernanceActionType.h>
#include <Cometa/ProposalProcedures/HardForkInitiationAction.h>
#include <Cometa/ProposalProcedures/InfoAction.h>
#include <Cometa/ProposalProcedures/NewConstitutionAction.h>
#include <Cometa/ProposalProcedures/NoConfidenceAction.h>
#include <Cometa/ProposalProcedures/ParameterChangeAction.h>
#include <Cometa/ProposalProcedures/TreasuryWithdrawalsAction.h>
#include <Cometa/ProposalProcedures/UpdateCommitteeAction.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_proposal_procedure_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A governance action together with its deposit, refund account, and metadata.
 *
 * The deposit is in lovelace. The reward address receives the deposit refund under the ledger's
 * governance rules; it is distinct from any treasury-withdrawal recipients.
 */
class ProposalProcedure
{
  public:

    /**
     * @brief Creates a proposal for ParameterChangeAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const ParameterChangeAction& action);

    /**
     * @brief Creates a proposal for HardForkInitiationAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const HardForkInitiationAction& action);

    /**
     * @brief Creates a proposal for TreasuryWithdrawalsAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const TreasuryWithdrawalsAction& action);

    /**
     * @brief Creates a proposal for NoConfidenceAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const NoConfidenceAction& action);

    /**
     * @brief Creates a proposal for UpdateCommitteeAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const UpdateCommitteeAction& action);

    /**
     * @brief Creates a proposal for NewConstitutionAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const NewConstitutionAction& action);

    /**
     * @brief Creates a proposal for InfoAction.
     * @param deposit The proposal deposit in lovelace.
     * @param rewardAddress The return address for the deposit.
     * @param anchor The proposal metadata anchor.
     * @param action The governance action.
     */
    ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const InfoAction& action);

    /**
     * @brief Copies a proposal.
     * @param other The proposal to copy.
     */
    ProposalProcedure(const ProposalProcedure& other);

    /**
     * @brief Moves a proposal.
     * @param other The proposal to move.
     */
    ProposalProcedure(ProposalProcedure&& other) noexcept;

    /** @brief Destroys the proposal. */
    ~ProposalProcedure();

  public:

    /**
     * @brief Decodes a proposal from CBOR.
     * @param cbor The encoded bytes.
     * @return The decoded proposal.
     */
    static ProposalProcedure fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal bytes.
     * @return The decoded proposal.
     */
    static ProposalProcedure fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the governance action kind.
     * @return The action kind.
     */
    GovernanceActionType type() const;

    /**
     * @brief Gets the ParameterChangeAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    ParameterChangeAction asParameterChangeAction() const;

    /**
     * @brief Gets the HardForkInitiationAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    HardForkInitiationAction asHardForkInitiationAction() const;

    /**
     * @brief Gets the TreasuryWithdrawalsAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    TreasuryWithdrawalsAction asTreasuryWithdrawalsAction() const;

    /**
     * @brief Gets the NoConfidenceAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    NoConfidenceAction asNoConfidenceAction() const;

    /**
     * @brief Gets the UpdateCommitteeAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    UpdateCommitteeAction asUpdateCommitteeAction() const;

    /**
     * @brief Gets the NewConstitutionAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    NewConstitutionAction asNewConstitutionAction() const;

    /**
     * @brief Gets the InfoAction action.
     * @return An independent action.
     * @throws std::logic_error The proposal contains another action kind.
     */
    InfoAction asInfoAction() const;

    /**
     * @brief Gets the deposit in lovelace.
     * @return The deposit.
     */
    std::uint64_t deposit() const;

    /**
     * @brief Sets the deposit in lovelace.
     * @param value The deposit.
     */
    void setDeposit(std::uint64_t value);

    /**
     * @brief Gets the return address for the deposit.
     * @return An independent address.
     */
    RewardAddress rewardAddress() const;

    /**
     * @brief Sets the return address for the deposit.
     * @param value The new address.
     */
    void setRewardAddress(const RewardAddress& value);

    /**
     * @brief Gets the proposal metadata anchor.
     * @return An independent anchor.
     */
    Anchor anchor() const;

    /**
     * @brief Sets the proposal metadata anchor.
     * @param value The new anchor.
     */
    void setAnchor(const Anchor& value);

    /**
     * @brief Encodes the proposal as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the proposal as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges proposals.
     * @param other The proposal to exchange.
     */
    void swap(ProposalProcedure& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The proposal to copy.
     * @return This proposal.
     */
    ProposalProcedure& operator=(const ProposalProcedure& other);

    /**
     * @brief Assigns a moved proposal.
     * @param other The proposal to move.
     * @return This proposal.
     */
    ProposalProcedure& operator=(ProposalProcedure&& other) noexcept;

    /**
     * @brief Compares proposals.
     * @param other The proposal to compare.
     * @return True when equal.
     */
    bool operator==(const ProposalProcedure& other) const;

    /**
     * @brief Compares proposals.
     * @param other The proposal to compare.
     * @return True when different.
     */
    bool operator!=(const ProposalProcedure& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static ProposalProcedure adopt(cardano_proposal_procedure_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_proposal_procedure_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_proposal_procedure_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_proposal_procedure_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned proposal.
     * @param value The owned proposal.
     */
    explicit ProposalProcedure(cardano_proposal_procedure_t* value) noexcept;

    /**
     * @brief Gets the stored proposal.
     * @return The borrowed proposal.
     */
    cardano_proposal_procedure_t* checked() const;

  private:

    /** @brief The owned proposal. */
    cardano_proposal_procedure_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges proposals.
 * @param lhs The first proposal.
 * @param rhs The second proposal.
 */
void swap(ProposalProcedure& lhs, ProposalProcedure& rhs) noexcept;
} // namespace Cometa
