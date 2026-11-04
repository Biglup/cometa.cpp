/**
 * @file NoConfidenceAction.cpp
 *
 * @brief No-confidence governance action.
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

#include <Cometa/ProposalProcedures/NoConfidenceAction.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/no_confidence_action.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
NoConfidenceAction::NoConfidenceAction(const std::optional<GovernanceActionId>& previousAction)
: m_value(nullptr)
{
  const std::optional<GovernanceActionId>                                           copy(previousAction);
  cardano_governance_action_id_t*                                                   previous = copy ? checkedNativeHandle(*copy) : nullptr;
  cardano_no_confidence_action_t*                                                   value    = nullptr;
  const cardano_error_t                                                             status   = cardano_no_confidence_action_new(previous, &value);
  CardanoObject<cardano_no_confidence_action_t, cardano_no_confidence_action_unref> owner(value);

  checkCardanoError(status, "NoConfidenceAction");
  m_value = owner.release();
}

NoConfidenceAction::NoConfidenceAction(const NoConfidenceAction& other)
: m_value(nullptr)
{
  cardano_no_confidence_action_t*                                                   value  = nullptr;
  const cardano_error_t                                                             status = cardano_no_confidence_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_no_confidence_action_t, cardano_no_confidence_action_unref> owner(value);

  checkCardanoError(status, "NoConfidenceAction copy");
  m_value = owner.release();
}

NoConfidenceAction::NoConfidenceAction(NoConfidenceAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

NoConfidenceAction::~NoConfidenceAction()
{
  cardano_no_confidence_action_unref(&m_value);
}

NoConfidenceAction
NoConfidenceAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return NoConfidenceAction(decodeCbor<cardano_no_confidence_action_t, cardano_no_confidence_action_unref>(cbor, cardano_no_confidence_action_from_cbor));
}

NoConfidenceAction
NoConfidenceAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::optional<GovernanceActionId>
NoConfidenceAction::previousAction() const
{
  cardano_governance_action_id_t*                                                   value = cardano_no_confidence_action_get_governance_action_id(checked());
  CardanoObject<cardano_governance_action_id_t, cardano_governance_action_id_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const GovernanceActionId found = GovernanceActionId::adopt(owner.release());
  return GovernanceActionId(found);
}

void
NoConfidenceAction::setPreviousAction(const std::optional<GovernanceActionId>& value)
{
  const std::optional<GovernanceActionId> copy(value);
  cardano_governance_action_id_t*         data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_no_confidence_action_set_governance_action_id(checked(), data), "NoConfidenceAction setPreviousAction");
}

std::vector<std::uint8_t>
NoConfidenceAction::toCbor() const
{
  return encodeCbor(checked(), cardano_no_confidence_action_to_cbor);
}

std::string
NoConfidenceAction::toJson() const
{
  return encodeJson(checked(), cardano_no_confidence_action_to_cip116_json);
}

void
NoConfidenceAction::swap(NoConfidenceAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

NoConfidenceAction&
NoConfidenceAction::operator=(const NoConfidenceAction& other)
{
  if (this != &other)
  {
    NoConfidenceAction copy(other);
    swap(copy);
  }
  return *this;
}

NoConfidenceAction&
NoConfidenceAction::operator=(NoConfidenceAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_no_confidence_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
NoConfidenceAction::operator==(const NoConfidenceAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
NoConfidenceAction::operator!=(const NoConfidenceAction& other) const
{
  return !(*this == other);
}

NoConfidenceAction
NoConfidenceAction::adopt(cardano_no_confidence_action_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null NoConfidenceAction");
  }

  return NoConfidenceAction(value);
}

cardano_no_confidence_action_t*
NoConfidenceAction::nativeHandle() noexcept
{
  return m_value;
}

const cardano_no_confidence_action_t*
NoConfidenceAction::nativeHandle() const noexcept
{
  return m_value;
}

cardano_no_confidence_action_t*
NoConfidenceAction::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

NoConfidenceAction::NoConfidenceAction(cardano_no_confidence_action_t* value) noexcept
: m_value(value)
{
}

cardano_no_confidence_action_t*
NoConfidenceAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from NoConfidenceAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(NoConfidenceAction& lhs, NoConfidenceAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
