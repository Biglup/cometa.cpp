/**
 * @file ProposalProcedure.cpp
 *
 * @brief Governance proposal procedures.
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

/* INCLUDES ******************************************************************/

#include <Cometa/ProposalProcedures/ProposalProcedure.h>

#include "../CardanoInterop.h"

#include <cardano/address/reward_address.h>
#include <cardano/common/anchor.h>
#include <cardano/proposal_procedures/hard_fork_initiation_action.h>
#include <cardano/proposal_procedures/info_action.h>
#include <cardano/proposal_procedures/new_constitution_action.h>
#include <cardano/proposal_procedures/no_confidence_action.h>
#include <cardano/proposal_procedures/parameter_change_action.h>
#include <cardano/proposal_procedures/proposal_procedure.h>
#include <cardano/proposal_procedures/treasury_withdrawals_action.h>
#include <cardano/proposal_procedures/update_committee_action.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const ParameterChangeAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const ParameterChangeAction                                                   actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_parameter_change_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure ParameterChangeAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const HardForkInitiationAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const HardForkInitiationAction                                                actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_hard_fork_initiation_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure HardForkInitiationAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const TreasuryWithdrawalsAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const TreasuryWithdrawalsAction                                               actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_treasury_withdrawals_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure TreasuryWithdrawalsAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const NoConfidenceAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const NoConfidenceAction                                                      actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_no_confidence_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure NoConfidenceAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const UpdateCommitteeAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const UpdateCommitteeAction                                                   actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_update_committee_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure UpdateCommitteeAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const NewConstitutionAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const NewConstitutionAction                                                   actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_constitution_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure NewConstitutionAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(std::uint64_t deposit, const RewardAddress& rewardAddress, const Anchor& anchor, const InfoAction& action)
: m_value(nullptr)
{
  const RewardAddress                                                           addressCopy(rewardAddress);
  const Anchor                                                                  anchorCopy(anchor);
  const InfoAction                                                              actionCopy(action);
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_new_info_action(deposit, checkedNativeHandle(addressCopy), checkedNativeHandle(anchorCopy), checkedNativeHandle(actionCopy), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure InfoAction");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(const ProposalProcedure& other)
: m_value(nullptr)
{
  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_deep_clone(other.checked(), &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure copy");
  m_value = owner.release();
}

ProposalProcedure::ProposalProcedure(ProposalProcedure&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ProposalProcedure::~ProposalProcedure()
{
  cardano_proposal_procedure_unref(&m_value);
}

ProposalProcedure
ProposalProcedure::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ProposalProcedure(decodeCbor<cardano_proposal_procedure_t, cardano_proposal_procedure_unref>(cbor, cardano_proposal_procedure_from_cbor));
}

ProposalProcedure
ProposalProcedure::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

GovernanceActionType
ProposalProcedure::type() const
{
  cardano_governance_action_type_t value = CARDANO_GOVERNANCE_ACTION_TYPE_PARAMETER_CHANGE;
  checkCardanoError(cardano_proposal_procedure_get_action_type(checked(), &value), "ProposalProcedure type");
  return static_cast<GovernanceActionType>(value);
}

ParameterChangeAction
ProposalProcedure::asParameterChangeAction() const
{
  if (type() != GovernanceActionType::ParameterChange)
  {
    throw std::logic_error("ProposalProcedure is not ParameterChangeAction");
  }

  cardano_parameter_change_action_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_proposal_procedure_to_parameter_change_action(checked(), &value);
  CardanoObject<cardano_parameter_change_action_t, cardano_parameter_change_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asParameterChangeAction");
  const ParameterChangeAction found = ParameterChangeAction::adopt(owner.release());
  return ParameterChangeAction(found);
}

HardForkInitiationAction
ProposalProcedure::asHardForkInitiationAction() const
{
  if (type() != GovernanceActionType::HardForkInitiation)
  {
    throw std::logic_error("ProposalProcedure is not HardForkInitiationAction");
  }

  cardano_hard_fork_initiation_action_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_proposal_procedure_to_hard_fork_initiation_action(checked(), &value);
  CardanoObject<cardano_hard_fork_initiation_action_t, cardano_hard_fork_initiation_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asHardForkInitiationAction");
  const HardForkInitiationAction found = HardForkInitiationAction::adopt(owner.release());
  return HardForkInitiationAction(found);
}

TreasuryWithdrawalsAction
ProposalProcedure::asTreasuryWithdrawalsAction() const
{
  if (type() != GovernanceActionType::TreasuryWithdrawals)
  {
    throw std::logic_error("ProposalProcedure is not TreasuryWithdrawalsAction");
  }

  cardano_treasury_withdrawals_action_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_proposal_procedure_to_treasury_withdrawals_action(checked(), &value);
  CardanoObject<cardano_treasury_withdrawals_action_t, cardano_treasury_withdrawals_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asTreasuryWithdrawalsAction");
  const TreasuryWithdrawalsAction found = TreasuryWithdrawalsAction::adopt(owner.release());
  return TreasuryWithdrawalsAction(found);
}

NoConfidenceAction
ProposalProcedure::asNoConfidenceAction() const
{
  if (type() != GovernanceActionType::NoConfidence)
  {
    throw std::logic_error("ProposalProcedure is not NoConfidenceAction");
  }

  cardano_no_confidence_action_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_proposal_procedure_to_no_confidence_action(checked(), &value);
  CardanoObject<cardano_no_confidence_action_t, cardano_no_confidence_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asNoConfidenceAction");
  const NoConfidenceAction found = NoConfidenceAction::adopt(owner.release());
  return NoConfidenceAction(found);
}

UpdateCommitteeAction
ProposalProcedure::asUpdateCommitteeAction() const
{
  if (type() != GovernanceActionType::UpdateCommittee)
  {
    throw std::logic_error("ProposalProcedure is not UpdateCommitteeAction");
  }

  cardano_update_committee_action_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_proposal_procedure_to_update_committee_action(checked(), &value);
  CardanoObject<cardano_update_committee_action_t, cardano_update_committee_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asUpdateCommitteeAction");
  const UpdateCommitteeAction found = UpdateCommitteeAction::adopt(owner.release());
  return UpdateCommitteeAction(found);
}

NewConstitutionAction
ProposalProcedure::asNewConstitutionAction() const
{
  if (type() != GovernanceActionType::NewConstitution)
  {
    throw std::logic_error("ProposalProcedure is not NewConstitutionAction");
  }

  cardano_new_constitution_action_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_proposal_procedure_to_constitution_action(checked(), &value);
  CardanoObject<cardano_new_constitution_action_t, cardano_new_constitution_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asNewConstitutionAction");
  const NewConstitutionAction found = NewConstitutionAction::adopt(owner.release());
  return NewConstitutionAction(found);
}

InfoAction
ProposalProcedure::asInfoAction() const
{
  if (type() != GovernanceActionType::Info)
  {
    throw std::logic_error("ProposalProcedure is not InfoAction");
  }

  cardano_info_action_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_proposal_procedure_to_info_action(checked(), &value);
  CardanoObject<cardano_info_action_t, cardano_info_action_unref> owner(value);

  checkCardanoError(status, "ProposalProcedure asInfoAction");
  const InfoAction found = InfoAction::adopt(owner.release());
  return InfoAction(found);
}

std::uint64_t
ProposalProcedure::deposit() const
{
  return cardano_proposal_procedure_get_deposit(checked());
}

void
ProposalProcedure::setDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_proposal_procedure_set_deposit(checked(), value), "ProposalProcedure setDeposit");
}

RewardAddress
ProposalProcedure::rewardAddress() const
{
  cardano_reward_address_t*                                             value = cardano_proposal_procedure_get_reward_address(checked());
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProposalProcedure has no reward address");
  }

  const RewardAddress found = RewardAddress::adopt(owner.release());
  return RewardAddress(found);
}

void
ProposalProcedure::setRewardAddress(const RewardAddress& value)
{
  const RewardAddress copy(value);
  checkCardanoError(cardano_proposal_procedure_set_reward_address(checked(), checkedNativeHandle(copy)), "ProposalProcedure setRewardAddress");
}

Anchor
ProposalProcedure::anchor() const
{
  cardano_anchor_t*                                     value = cardano_proposal_procedure_get_anchor(checked());
  CardanoObject<cardano_anchor_t, cardano_anchor_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProposalProcedure has no anchor");
  }

  const Anchor found = Anchor::adopt(owner.release());
  return Anchor(found);
}

void
ProposalProcedure::setAnchor(const Anchor& value)
{
  const Anchor copy(value);
  checkCardanoError(cardano_proposal_procedure_set_anchor(checked(), checkedNativeHandle(copy)), "ProposalProcedure setAnchor");
}

std::vector<std::uint8_t>
ProposalProcedure::toCbor() const
{
  return encodeCbor(checked(), cardano_proposal_procedure_to_cbor);
}

std::string
ProposalProcedure::toJson() const
{
  return encodeJson(checked(), cardano_proposal_procedure_to_cip116_json);
}

void
ProposalProcedure::swap(ProposalProcedure& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ProposalProcedure&
ProposalProcedure::operator=(const ProposalProcedure& other)
{
  if (this != &other)
  {
    ProposalProcedure copy(other);
    swap(copy);
  }
  return *this;
}

ProposalProcedure&
ProposalProcedure::operator=(ProposalProcedure&& other) noexcept
{
  if (this != &other)
  {
    cardano_proposal_procedure_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ProposalProcedure::operator==(const ProposalProcedure& other) const
{
  return toCbor() == other.toCbor();
}

bool
ProposalProcedure::operator!=(const ProposalProcedure& other) const
{
  return !(*this == other);
}

ProposalProcedure
ProposalProcedure::adopt(cardano_proposal_procedure_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ProposalProcedure");
  }

  return ProposalProcedure(value);
}

cardano_proposal_procedure_t*
ProposalProcedure::nativeHandle() noexcept
{
  return m_value;
}

const cardano_proposal_procedure_t*
ProposalProcedure::nativeHandle() const noexcept
{
  return m_value;
}

cardano_proposal_procedure_t*
ProposalProcedure::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ProposalProcedure::ProposalProcedure(cardano_proposal_procedure_t* value) noexcept
: m_value(value)
{
}

cardano_proposal_procedure_t*
ProposalProcedure::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ProposalProcedure");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ProposalProcedure& lhs, ProposalProcedure& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
