/**
 * @file VotingProcedure.cpp
 *
 * @brief Governance vote and optional metadata anchor.
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

#include <Cometa/VotingProcedures/VotingProcedure.h>

#include "../CardanoInterop.h"

#include <cardano/common/anchor.h>
#include <cardano/voting_procedures/voting_procedure.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
VotingProcedure::VotingProcedure(Vote vote, const std::optional<Anchor>& anchor)
: m_value(nullptr)
{
  const std::optional<Anchor>                                               copy(anchor);
  cardano_anchor_t*                                                         data   = copy ? copy->checked() : nullptr;
  cardano_voting_procedure_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_voting_procedure_new(static_cast<cardano_vote_t>(vote), data, &value);
  CardanoObject<cardano_voting_procedure_t, cardano_voting_procedure_unref> owner(value);

  checkCardanoError(status, "VotingProcedure");
  m_value = owner.release();
}

VotingProcedure::VotingProcedure(const VotingProcedure& other)
: m_value(nullptr)
{
  cardano_voting_procedure_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_voting_procedure_deep_clone(other.checked(), &value);
  CardanoObject<cardano_voting_procedure_t, cardano_voting_procedure_unref> owner(value);

  checkCardanoError(status, "VotingProcedure copy");
  m_value = owner.release();
}

VotingProcedure::VotingProcedure(VotingProcedure&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

VotingProcedure::~VotingProcedure()
{
  cardano_voting_procedure_unref(&m_value);
}

VotingProcedure
VotingProcedure::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return VotingProcedure(decodeCbor<cardano_voting_procedure_t, cardano_voting_procedure_unref>(cbor, cardano_voting_procedure_from_cbor));
}

VotingProcedure
VotingProcedure::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

Vote
VotingProcedure::vote() const
{
  return static_cast<Vote>(cardano_voting_procedure_get_vote(checked()));
}

void
VotingProcedure::setVote(Vote value)
{
  checkCardanoError(cardano_voting_procedure_set_vote(checked(), static_cast<cardano_vote_t>(value)), "VotingProcedure setVote");
}

std::optional<Anchor>
VotingProcedure::anchor() const
{
  cardano_anchor_t*                                     value = cardano_voting_procedure_get_anchor(checked());
  CardanoObject<cardano_anchor_t, cardano_anchor_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const Anchor found(owner.release());
  return Anchor(found);
}

void
VotingProcedure::setAnchor(const Anchor& value)
{
  const Anchor copy(value);
  checkCardanoError(cardano_voting_procedure_set_anchor(checked(), copy.checked()), "VotingProcedure setAnchor");
}

std::vector<std::uint8_t>
VotingProcedure::toCbor() const
{
  return encodeCbor(checked(), cardano_voting_procedure_to_cbor);
}

std::string
VotingProcedure::toJson() const
{
  return encodeJson(checked(), cardano_voting_procedure_to_cip116_json);
}

void
VotingProcedure::swap(VotingProcedure& other) noexcept
{
  std::swap(m_value, other.m_value);
}

VotingProcedure&
VotingProcedure::operator=(const VotingProcedure& other)
{
  if (this != &other)
  {
    VotingProcedure copy(other);
    swap(copy);
  }
  return *this;
}

VotingProcedure&
VotingProcedure::operator=(VotingProcedure&& other) noexcept
{
  if (this != &other)
  {
    cardano_voting_procedure_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
VotingProcedure::operator==(const VotingProcedure& other) const
{
  return toCbor() == other.toCbor();
}

bool
VotingProcedure::operator!=(const VotingProcedure& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

VotingProcedure::VotingProcedure(cardano_voting_procedure_t* value) noexcept
: m_value(value)
{
}

cardano_voting_procedure_t*
VotingProcedure::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from VotingProcedure");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VotingProcedure& lhs, VotingProcedure& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
