/**
 * @file UpdateCommitteeAction.cpp
 *
 * @brief Constitutional committee update action.
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

#include <Cometa/ProposalProcedures/UpdateCommitteeAction.h>

#include "../CardanoInterop.h"

#include <cardano/common/governance_action_id.h>
#include <cardano/common/unit_interval.h>
#include <cardano/proposal_procedures/committee_members_map.h>
#include <cardano/proposal_procedures/credential_set.h>
#include <cardano/proposal_procedures/update_committee_action.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
UpdateCommitteeAction::UpdateCommitteeAction(const CredentialSet& removed, const CommitteeMembersMap& added, const UnitInterval& quorum, const std::optional<GovernanceActionId>& previousAction)
: m_value(nullptr)
{
  const CredentialSet                                                                     removedCopy(removed);
  const CommitteeMembersMap                                                               addedCopy(added);
  const UnitInterval                                                                      quorumCopy(quorum);
  const std::optional<GovernanceActionId>                                                 previousCopy(previousAction);
  cardano_governance_action_id_t*                                                         previous = previousCopy ? previousCopy->checked() : nullptr;
  cardano_update_committee_action_t*                                                      value    = nullptr;
  const cardano_error_t                                                                   status   = cardano_update_committee_action_new(removedCopy.checked(), addedCopy.checked(), quorumCopy.checked(), previous, &value);
  CardanoObject<cardano_update_committee_action_t, cardano_update_committee_action_unref> owner(value);

  checkCardanoError(status, "UpdateCommitteeAction");
  m_value = owner.release();
}

UpdateCommitteeAction::UpdateCommitteeAction(const UpdateCommitteeAction& other)
: m_value(nullptr)
{
  cardano_update_committee_action_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_update_committee_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_update_committee_action_t, cardano_update_committee_action_unref> owner(value);

  checkCardanoError(status, "UpdateCommitteeAction copy");
  m_value = owner.release();
}

UpdateCommitteeAction::UpdateCommitteeAction(UpdateCommitteeAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

UpdateCommitteeAction::~UpdateCommitteeAction()
{
  cardano_update_committee_action_unref(&m_value);
}

UpdateCommitteeAction
UpdateCommitteeAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return UpdateCommitteeAction(decodeCbor<cardano_update_committee_action_t, cardano_update_committee_action_unref>(cbor, cardano_update_committee_action_from_cbor));
}

UpdateCommitteeAction
UpdateCommitteeAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

CredentialSet
UpdateCommitteeAction::removedMembers() const
{
  cardano_credential_set_t*                                             value = cardano_update_committee_action_get_members_to_be_removed(checked());
  CardanoObject<cardano_credential_set_t, cardano_credential_set_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("UpdateCommitteeAction has no removed members");
  }

  const CredentialSet found(owner.release());
  return CredentialSet(found);
}

void
UpdateCommitteeAction::setRemovedMembers(const CredentialSet& value)
{
  const CredentialSet copy(value);
  checkCardanoError(cardano_update_committee_action_set_members_to_be_removed(checked(), copy.checked()), "UpdateCommitteeAction setRemovedMembers");
}

CommitteeMembersMap
UpdateCommitteeAction::addedMembers() const
{
  cardano_committee_members_map_t*                                                    value = cardano_update_committee_action_get_members_to_be_added(checked());
  CardanoObject<cardano_committee_members_map_t, cardano_committee_members_map_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("UpdateCommitteeAction has no added members");
  }

  const CommitteeMembersMap found(owner.release());
  return CommitteeMembersMap(found);
}

void
UpdateCommitteeAction::setAddedMembers(const CommitteeMembersMap& value)
{
  const CommitteeMembersMap copy(value);
  checkCardanoError(cardano_update_committee_action_set_members_to_be_added(checked(), copy.checked()), "UpdateCommitteeAction setAddedMembers");
}

UnitInterval
UpdateCommitteeAction::quorum() const
{
  cardano_unit_interval_t*                                            value = cardano_update_committee_action_get_quorum(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("UpdateCommitteeAction has no quorum");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
UpdateCommitteeAction::setQuorum(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_update_committee_action_set_quorum(checked(), copy.checked()), "UpdateCommitteeAction setQuorum");
}

std::optional<GovernanceActionId>
UpdateCommitteeAction::previousAction() const
{
  cardano_governance_action_id_t*                                                   value = cardano_update_committee_action_get_governance_action_id(checked());
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const GovernanceActionId found(owner.release());
  return GovernanceActionId(found);
}

void
UpdateCommitteeAction::setPreviousAction(const std::optional<GovernanceActionId>& value)
{
  const std::optional<GovernanceActionId> copy(value);
  cardano_governance_action_id_t*         data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_update_committee_action_set_governance_action_id(checked(), data), "UpdateCommitteeAction setPreviousAction");
}

std::vector<std::uint8_t>
UpdateCommitteeAction::toCbor() const
{
  return encodeCbor(checked(), cardano_update_committee_action_to_cbor);
}

std::string
UpdateCommitteeAction::toJson() const
{
  return encodeJson(checked(), cardano_update_committee_action_to_cip116_json);
}

void
UpdateCommitteeAction::swap(UpdateCommitteeAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

UpdateCommitteeAction&
UpdateCommitteeAction::operator=(const UpdateCommitteeAction& other)
{
  if (this != &other)
  {
    UpdateCommitteeAction copy(other);
    swap(copy);
  }
  return *this;
}

UpdateCommitteeAction&
UpdateCommitteeAction::operator=(UpdateCommitteeAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_update_committee_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
UpdateCommitteeAction::operator==(const UpdateCommitteeAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
UpdateCommitteeAction::operator!=(const UpdateCommitteeAction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

UpdateCommitteeAction::UpdateCommitteeAction(cardano_update_committee_action_t* value) noexcept
: m_value(value)
{
}

cardano_update_committee_action_t*
UpdateCommitteeAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from UpdateCommitteeAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(UpdateCommitteeAction& lhs, UpdateCommitteeAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
