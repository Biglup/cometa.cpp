/**
 * @file HardForkInitiationAction.cpp
 *
 * @brief Hard fork initiation governance action.
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

#include <Cometa/ProposalProcedures/HardForkInitiationAction.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/hard_fork_initiation_action.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
HardForkInitiationAction::HardForkInitiationAction(const ProtocolVersion& version, const std::optional<GovernanceActionId>& previousAction)
: m_value(nullptr)
{
  const ProtocolVersion                                                                           versionCopy(version);
  const std::optional<GovernanceActionId>                                                         previousCopy(previousAction);
  cardano_governance_action_id_t*                                                                 previous = previousCopy ? previousCopy->checked() : nullptr;
  cardano_hard_fork_initiation_action_t*                                                          value    = nullptr;
  const cardano_error_t                                                                           status   = cardano_hard_fork_initiation_action_new(versionCopy.checked(), previous, &value);
  CardanoObject<cardano_hard_fork_initiation_action_t, cardano_hard_fork_initiation_action_unref> owner(value);

  checkCardanoError(status, "HardForkInitiationAction");
  m_value = owner.release();
}

HardForkInitiationAction::HardForkInitiationAction(const HardForkInitiationAction& other)
: m_value(nullptr)
{
  cardano_hard_fork_initiation_action_t*                                                          value  = nullptr;
  const cardano_error_t                                                                           status = cardano_hard_fork_initiation_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_hard_fork_initiation_action_t, cardano_hard_fork_initiation_action_unref> owner(value);

  checkCardanoError(status, "HardForkInitiationAction copy");
  m_value = owner.release();
}

HardForkInitiationAction::HardForkInitiationAction(HardForkInitiationAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

HardForkInitiationAction::~HardForkInitiationAction()
{
  cardano_hard_fork_initiation_action_unref(&m_value);
}

HardForkInitiationAction
HardForkInitiationAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return HardForkInitiationAction(decodeCbor<cardano_hard_fork_initiation_action_t, cardano_hard_fork_initiation_action_unref>(cbor, cardano_hard_fork_initiation_action_from_cbor));
}

HardForkInitiationAction
HardForkInitiationAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ProtocolVersion
HardForkInitiationAction::version() const
{
  cardano_protocol_version_t*                                               value = cardano_hard_fork_initiation_action_get_protocol_version(checked());
  CardanoObject<cardano_protocol_version_t, cardano_protocol_version_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("HardForkInitiationAction has no protocol version");
  }

  const ProtocolVersion found(owner.release());
  return ProtocolVersion(found);
}

void
HardForkInitiationAction::setVersion(const ProtocolVersion& value)
{
  const ProtocolVersion copy(value);
  checkCardanoError(cardano_hard_fork_initiation_action_set_protocol_version(checked(), copy.checked()), "HardForkInitiationAction setVersion");
}

std::optional<GovernanceActionId>
HardForkInitiationAction::previousAction() const
{
  cardano_governance_action_id_t*                                                   value = cardano_hard_fork_initiation_action_get_governance_action_id(checked());
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const GovernanceActionId found(owner.release());
  return GovernanceActionId(found);
}

void
HardForkInitiationAction::setPreviousAction(const std::optional<GovernanceActionId>& value)
{
  const std::optional<GovernanceActionId> copy(value);
  cardano_governance_action_id_t*         data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_hard_fork_initiation_action_set_governance_action_id(checked(), data), "HardForkInitiationAction setPreviousAction");
}

std::vector<std::uint8_t>
HardForkInitiationAction::toCbor() const
{
  return encodeCbor(checked(), cardano_hard_fork_initiation_action_to_cbor);
}

std::string
HardForkInitiationAction::toJson() const
{
  return encodeJson(checked(), cardano_hard_fork_initiation_action_to_cip116_json);
}

void
HardForkInitiationAction::swap(HardForkInitiationAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

HardForkInitiationAction&
HardForkInitiationAction::operator=(const HardForkInitiationAction& other)
{
  if (this != &other)
  {
    HardForkInitiationAction copy(other);
    swap(copy);
  }
  return *this;
}

HardForkInitiationAction&
HardForkInitiationAction::operator=(HardForkInitiationAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_hard_fork_initiation_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
HardForkInitiationAction::operator==(const HardForkInitiationAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
HardForkInitiationAction::operator!=(const HardForkInitiationAction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

HardForkInitiationAction::HardForkInitiationAction(cardano_hard_fork_initiation_action_t* value) noexcept
: m_value(value)
{
}

cardano_hard_fork_initiation_action_t*
HardForkInitiationAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from HardForkInitiationAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(HardForkInitiationAction& lhs, HardForkInitiationAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
