/**
 * @file VotingProcedures.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/VotingProcedures/VotingProcedures.h>

#include "../CardanoInterop.h"

#include <cardano/common/governance_action_id.h>
#include <cardano/voting_procedures/governance_action_id_list.h>
#include <cardano/voting_procedures/voter.h>
#include <cardano/voting_procedures/voter_list.h>
#include <cardano/voting_procedures/voting_procedure.h>
#include <cardano/voting_procedures/voting_procedures.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VotingProcedures::VotingProcedures()
: m_value(nullptr)
{
  cardano_voting_procedures_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_voting_procedures_new(&value);
  CardanoObject<cardano_voting_procedures_t, cardano_voting_procedures_unref> owner(value);

  checkCardanoError(status, "VotingProcedures");
  m_value = owner.release();
}

VotingProcedures::VotingProcedures(const VotingProcedures& other)
: m_value(nullptr)
{
  cardano_voting_procedures_t*                                                value  = nullptr;
  const cardano_error_t                                                       status = cardano_voting_procedures_deep_clone(other.checked(), &value);
  CardanoObject<cardano_voting_procedures_t, cardano_voting_procedures_unref> owner(value);

  checkCardanoError(status, "VotingProcedures copy");
  m_value = owner.release();
}

VotingProcedures::VotingProcedures(VotingProcedures&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

VotingProcedures::~VotingProcedures()
{
  cardano_voting_procedures_unref(&m_value);
}

VotingProcedures
VotingProcedures::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VotingProcedures(decodeCbor<cardano_voting_procedures_t, cardano_voting_procedures_unref>(cbor, cardano_voting_procedures_from_cbor));
}

VotingProcedures
VotingProcedures::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

void
VotingProcedures::insert(const Voter& voter, const GovernanceActionId& actionId, const VotingProcedure& procedure)
{
  cardano_voting_procedure_t*                                               existing = cardano_voting_procedures_get(checked(), voter.checked(), actionId.checked());
  CardanoObject<cardano_voting_procedure_t, cardano_voting_procedure_unref> existingOwner(existing);

  if (existing)
  {
    throw std::invalid_argument("VotingProcedures already contains a vote for this voter and action");
  }

  const Voter              voterCopy(voter);
  const GovernanceActionId idCopy(actionId);
  const VotingProcedure    procedureCopy(procedure);

  checkCardanoError(cardano_voting_procedures_insert(checked(), voterCopy.checked(), idCopy.checked(), procedureCopy.checked()), "VotingProcedures insert");
}

std::optional<VotingProcedure>
VotingProcedures::get(const Voter& voter, const GovernanceActionId& actionId) const
{
  cardano_voting_procedure_t*                                               value = cardano_voting_procedures_get(checked(), voter.checked(), actionId.checked());
  CardanoObject<cardano_voting_procedure_t, cardano_voting_procedure_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const VotingProcedure found(owner.release());
  return VotingProcedure(found);
}

VoterList
VotingProcedures::voters() const
{
  cardano_voter_list_t*                                         list   = nullptr;
  const cardano_error_t                                         status = cardano_voting_procedures_get_voters(checked(), &list);
  CardanoObject<cardano_voter_list_t, cardano_voter_list_unref> owner(list);

  checkCardanoError(status, "VotingProcedures voters");

  VoterList result;
  result.reserve(cardano_voter_list_get_length(owner.get()));

  for (std::size_t index = 0; index < cardano_voter_list_get_length(owner.get()); ++index)
  {
    cardano_voter_t*                                    value      = nullptr;
    const cardano_error_t                               resultCode = cardano_voter_list_get(owner.get(), index, &value);
    CardanoObject<cardano_voter_t, cardano_voter_unref> voterOwner(value);

    checkCardanoError(resultCode, "VotingProcedures voters get");
    const Voter found(voterOwner.release());
    result.push_back(found);
  }

  return result;
}

GovernanceActionIdList
VotingProcedures::actionIds(const Voter& voter) const
{
  cardano_governance_action_id_list_t*                                                        list   = nullptr;
  const cardano_error_t                                                                       status = cardano_voting_procedures_get_governance_ids_by_voter(checked(), voter.checked(), &list);
  CardanoObject<cardano_governance_action_id_list_t, cardano_governance_action_id_list_unref> owner(list);

  checkCardanoError(status, "VotingProcedures actionIds");

  GovernanceActionIdList result;

  if (!list)
  {
    return result;
  }

  result.reserve(cardano_governance_action_id_list_get_length(owner.get()));

  for (std::size_t index = 0; index < cardano_governance_action_id_list_get_length(owner.get()); ++index)
  {
    cardano_governance_action_id_t*                                                   value      = nullptr;
    const cardano_error_t                                                             resultCode = cardano_governance_action_id_list_get(owner.get(), index, &value);
    CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> idOwner(value);

    checkCardanoError(resultCode, "VotingProcedures actionIds get");
    const GovernanceActionId found(idOwner.release());
    result.push_back(found);
  }

  return result;
}

std::vector<std::uint8_t>
VotingProcedures::toCbor() const
{
  return encodeCbor(checked(), cardano_voting_procedures_to_cbor);
}

std::string
VotingProcedures::toJson() const
{
  return encodeJson(checked(), cardano_voting_procedures_to_cip116_json);
}

void
VotingProcedures::swap(VotingProcedures& other) noexcept
{
  std::swap(m_value, other.m_value);
}

VotingProcedures&
VotingProcedures::operator=(const VotingProcedures& other)
{
  if (this != &other)
  {
    VotingProcedures copy(other);
    swap(copy);
  }
  return *this;
}

VotingProcedures&
VotingProcedures::operator=(VotingProcedures&& other) noexcept
{
  if (this != &other)
  {
    cardano_voting_procedures_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
VotingProcedures::operator==(const VotingProcedures& other) const
{
  return toCbor() == other.toCbor();
}

bool
VotingProcedures::operator!=(const VotingProcedures& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

VotingProcedures::VotingProcedures(cardano_voting_procedures_t* value) noexcept
: m_value(value)
{
}

cardano_voting_procedures_t*
VotingProcedures::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VotingProcedures");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VotingProcedures& lhs, VotingProcedures& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
