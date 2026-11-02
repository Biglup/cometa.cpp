/**
 * @file NewConstitutionAction.cpp
 *
 * @brief New constitution governance action.
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

#include <Cometa/ProposalProcedures/NewConstitutionAction.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/new_constitution_action.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
NewConstitutionAction::NewConstitutionAction(const Constitution& constitution, const std::optional<GovernanceActionId>& previousAction)
: m_value(nullptr)
{
  const Constitution                                                                      constitutionCopy(constitution);
  const std::optional<GovernanceActionId>                                                 previousCopy(previousAction);
  cardano_governance_action_id_t*                                                         previous = previousCopy ? previousCopy->checked() : nullptr;
  cardano_new_constitution_action_t*                                                      value    = nullptr;
  const cardano_error_t                                                                   status   = cardano_new_constitution_action_new(constitutionCopy.checked(), previous, &value);
  CardanoObject<cardano_new_constitution_action_t, cardano_new_constitution_action_unref> owner(value);

  checkCardanoError(status, "NewConstitutionAction");
  m_value = owner.release();
}

NewConstitutionAction::NewConstitutionAction(const NewConstitutionAction& other)
: m_value(nullptr)
{
  cardano_new_constitution_action_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_new_constitution_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_new_constitution_action_t, cardano_new_constitution_action_unref> owner(value);

  checkCardanoError(status, "NewConstitutionAction copy");
  m_value = owner.release();
}

NewConstitutionAction::NewConstitutionAction(NewConstitutionAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

NewConstitutionAction::~NewConstitutionAction()
{
  cardano_new_constitution_action_unref(&m_value);
}

NewConstitutionAction
NewConstitutionAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return NewConstitutionAction(decodeCbor<cardano_new_constitution_action_t, cardano_new_constitution_action_unref>(cbor, cardano_new_constitution_action_from_cbor));
}

NewConstitutionAction
NewConstitutionAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Constitution
NewConstitutionAction::constitution() const
{
  cardano_constitution_t*                                           value = cardano_new_constitution_action_get_constitution(checked());
  CardanoObject<cardano_constitution_t, cardano_constitution_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("NewConstitutionAction has no constitution");
  }

  const Constitution found(owner.release());
  return Constitution(found);
}

void
NewConstitutionAction::setConstitution(const Constitution& value)
{
  const Constitution copy(value);
  checkCardanoError(cardano_new_constitution_action_set_constitution(checked(), copy.checked()), "NewConstitutionAction setConstitution");
}

std::optional<GovernanceActionId>
NewConstitutionAction::previousAction() const
{
  cardano_governance_action_id_t*                                                   value = cardano_new_constitution_action_get_governance_action_id(checked());
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const GovernanceActionId found(owner.release());
  return GovernanceActionId(found);
}

void
NewConstitutionAction::setPreviousAction(const std::optional<GovernanceActionId>& value)
{
  const std::optional<GovernanceActionId> copy(value);
  cardano_governance_action_id_t*         data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_new_constitution_action_set_governance_action_id(checked(), data), "NewConstitutionAction setPreviousAction");
}

std::vector<std::uint8_t>
NewConstitutionAction::toCbor() const
{
  return encodeCbor(checked(), cardano_new_constitution_action_to_cbor);
}

std::string
NewConstitutionAction::toJson() const
{
  return encodeJson(checked(), cardano_new_constitution_action_to_cip116_json);
}

void
NewConstitutionAction::swap(NewConstitutionAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

NewConstitutionAction&
NewConstitutionAction::operator=(const NewConstitutionAction& other)
{
  if (this != &other)
  {
    NewConstitutionAction copy(other);
    swap(copy);
  }
  return *this;
}

NewConstitutionAction&
NewConstitutionAction::operator=(NewConstitutionAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_new_constitution_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
NewConstitutionAction::operator==(const NewConstitutionAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
NewConstitutionAction::operator!=(const NewConstitutionAction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

NewConstitutionAction::NewConstitutionAction(cardano_new_constitution_action_t* value) noexcept
: m_value(value)
{
}

cardano_new_constitution_action_t*
NewConstitutionAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from NewConstitutionAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NewConstitutionAction& lhs, NewConstitutionAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
