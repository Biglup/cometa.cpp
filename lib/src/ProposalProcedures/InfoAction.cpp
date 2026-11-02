/**
 * @file InfoAction.cpp
 *
 * @brief Informational governance action.
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

#include <Cometa/ProposalProcedures/InfoAction.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/info_action.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
InfoAction::InfoAction()
: m_value(nullptr)
{
  cardano_info_action_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_info_action_new(&value);
  CardanoObject<cardano_info_action_t, cardano_info_action_unref> owner(value);

  checkCardanoError(status, "InfoAction");
  m_value = owner.release();
}

InfoAction::InfoAction(const InfoAction& other)
: m_value(nullptr)
{
  cardano_info_action_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_info_action_deep_clone(other.checked(), &value);
  CardanoObject<cardano_info_action_t, cardano_info_action_unref> owner(value);

  checkCardanoError(status, "InfoAction copy");
  m_value = owner.release();
}

InfoAction::InfoAction(InfoAction&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

InfoAction::~InfoAction()
{
  cardano_info_action_unref(&m_value);
}

InfoAction
InfoAction::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return InfoAction(decodeCbor<cardano_info_action_t, cardano_info_action_unref>(cbor, cardano_info_action_from_cbor));
}

InfoAction
InfoAction::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::vector<std::uint8_t>
InfoAction::toCbor() const
{
  return encodeCbor(checked(), cardano_info_action_to_cbor);
}

std::string
InfoAction::toJson() const
{
  return encodeJson(checked(), cardano_info_action_to_cip116_json);
}

void
InfoAction::swap(InfoAction& other) noexcept
{
  std::swap(m_value, other.m_value);
}

InfoAction&
InfoAction::operator=(const InfoAction& other)
{
  if (this != &other)
  {
    InfoAction copy(other);
    swap(copy);
  }
  return *this;
}

InfoAction&
InfoAction::operator=(InfoAction&& other) noexcept
{
  if (this != &other)
  {
    cardano_info_action_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
InfoAction::operator==(const InfoAction& other) const
{
  return toCbor() == other.toCbor();
}

bool
InfoAction::operator!=(const InfoAction& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

InfoAction::InfoAction(cardano_info_action_t* value) noexcept
: m_value(value)
{
}

cardano_info_action_t*
InfoAction::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from InfoAction");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(InfoAction& lhs, InfoAction& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
