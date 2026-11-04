/**
 * @file ParameterChangeAction.cpp
 *
 * @brief Protocol parameter change governance action.
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

#include <Cometa/ProposalProcedures/ParameterChangeAction.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/parameter_change_action.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ParameterChangeAction::ParameterChangeAction(
  const ProtocolParamUpdate&               update,
  const std::optional<GovernanceActionId>& previousAction,
  const std::optional<Blake2bHash>&        policyHash)
: m_value(nullptr)
{
  const ProtocolParamUpdate                                                               updateCopy(update);
  const std::optional<GovernanceActionId>                                                 previousCopy(previousAction);
  const std::optional<Blake2bHash>                                                        policyCopy(policyHash);
  cardano_governance_action_id_t*                                                         previous = previousCopy ? checkedNativeHandle(*previousCopy) : nullptr;
  cardano_blake2b_hash_t*                                                                 policy   = policyCopy ? checkedNativeHandle(*policyCopy) : nullptr;
  cardano_parameter_change_action_t*                                                      value    = nullptr;
  const cardano_error_t                                                                   status   = cardano_parameter_change_action_new(checkedNativeHandle(updateCopy), previous, policy, &value);
  CardanoObject<cardano_parameter_change_action_t, cardano_parameter_change_action_unref> owner(value);

  checkCardanoError(status, "ParameterChangeAction");
  m_value = owner.release();
}

ParameterChangeAction::ParameterChangeAction(const ParameterChangeAction& other)
: m_value(nullptr)
{
  cardano_parameter_change_action_t*                                                      value  = nullptr;
  const cardano_error_t                                                                   status = cardano_parameter_change_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_parameter_change_action_t, cardano_parameter_change_action_unref> owner(value);

  checkCardanoError(status, "ParameterChangeAction copy");
  m_value = owner.release();
}

ParameterChangeAction::ParameterChangeAction(ParameterChangeAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ParameterChangeAction::~ParameterChangeAction()
{
  cardano_parameter_change_action_unref(&m_value);
}

ParameterChangeAction
ParameterChangeAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ParameterChangeAction(decodeCbor<cardano_parameter_change_action_t, cardano_parameter_change_action_unref>(cbor, cardano_parameter_change_action_from_cbor));
}

ParameterChangeAction
ParameterChangeAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

ProtocolParamUpdate
ParameterChangeAction::update() const
{
  cardano_protocol_param_update_t*                                                    value = cardano_parameter_change_action_get_protocol_param_update(checked());
  CardanoObject<cardano_protocol_param_update_t, cardano_protocol_param_update_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ParameterChangeAction has no parameter update");
  }

  const ProtocolParamUpdate found = ProtocolParamUpdate::adopt(owner.release());
  return ProtocolParamUpdate(found);
}

void
ParameterChangeAction::setUpdate(const ProtocolParamUpdate& value)
{
  const ProtocolParamUpdate copy(value);
  checkCardanoError(cardano_parameter_change_action_set_protocol_param_update(checked(), checkedNativeHandle(copy)), "ParameterChangeAction setUpdate");
}

std::optional<Blake2bHash>
ParameterChangeAction::policyHash() const
{
  cardano_blake2b_hash_t*                                           value = cardano_parameter_change_action_get_policy_hash(checked());
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const Blake2bHash found = Blake2bHash::adopt(owner.release());
  return Blake2bHash(found);
}

void
ParameterChangeAction::setPolicyHash(const std::optional<Blake2bHash>& value)
{
  const std::optional<Blake2bHash> copy(value);
  cardano_blake2b_hash_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_parameter_change_action_set_policy_hash(checked(), data), "ParameterChangeAction setPolicyHash");
}

std::optional<GovernanceActionId>
ParameterChangeAction::previousAction() const
{
  cardano_governance_action_id_t*                                                   value = cardano_parameter_change_action_get_governance_action_id(checked());
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const GovernanceActionId found = GovernanceActionId::adopt(owner.release());
  return GovernanceActionId(found);
}

void
ParameterChangeAction::setPreviousAction(const std::optional<GovernanceActionId>& value)
{
  const std::optional<GovernanceActionId> copy(value);
  cardano_governance_action_id_t*         data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_parameter_change_action_set_governance_action_id(checked(), data), "ParameterChangeAction setPreviousAction");
}

std::vector<std::uint8_t>
ParameterChangeAction::toCbor() const
{
  return encodeCbor(checked(), cardano_parameter_change_action_to_cbor);
}

std::string
ParameterChangeAction::toJson() const
{
  return encodeJson(checked(), cardano_parameter_change_action_to_cip116_json);
}

void
ParameterChangeAction::swap(ParameterChangeAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ParameterChangeAction&
ParameterChangeAction::operator=(const ParameterChangeAction& other)
{
  if (this != &other)
  {
    ParameterChangeAction copy(other);
    swap(copy);
  }
  return *this;
}

ParameterChangeAction&
ParameterChangeAction::operator=(ParameterChangeAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_parameter_change_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ParameterChangeAction::operator==(const ParameterChangeAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
ParameterChangeAction::operator!=(const ParameterChangeAction& other) const
{
  return !(*this == other);
}

ParameterChangeAction
ParameterChangeAction::adopt(cardano_parameter_change_action_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ParameterChangeAction");
  }

  return ParameterChangeAction(value);
}

cardano_parameter_change_action_t*
ParameterChangeAction::nativeHandle() noexcept
{
  return m_value;
}

const cardano_parameter_change_action_t*
ParameterChangeAction::nativeHandle() const noexcept
{
  return m_value;
}

cardano_parameter_change_action_t*
ParameterChangeAction::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ParameterChangeAction::ParameterChangeAction(cardano_parameter_change_action_t* value) noexcept
: m_value(value)
{
}

cardano_parameter_change_action_t*
ParameterChangeAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ParameterChangeAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ParameterChangeAction& lhs, ParameterChangeAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
