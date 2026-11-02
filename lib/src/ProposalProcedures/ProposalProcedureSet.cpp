/**
 * @file ProposalProcedureSet.cpp
 *
 * @brief Ledger proposal procedure collections.
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

#include <Cometa/ProposalProcedures/ProposalProcedureSet.h>

#include "../CardanoInterop.h"

#include <cardano/proposal_procedures/proposal_procedure_set.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ProposalProcedureSet::ProposalProcedureSet()
: m_value(nullptr)
{
  cardano_proposal_procedure_set_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_proposal_procedure_set_new(&value);
  CardanoObject<cardano_proposal_procedure_set_t, cardano_proposal_procedure_set_unref> owner(value);

  checkCardanoError(status, "ProposalProcedureSet");
  m_value = owner.release();
}

ProposalProcedureSet::ProposalProcedureSet(const ProposalProcedureSet& other)
: m_value(nullptr)
{
  cardano_proposal_procedure_set_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_proposal_procedure_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_proposal_procedure_set_t, cardano_proposal_procedure_set_unref> owner(value);

  checkCardanoError(status, "ProposalProcedureSet copy");
  m_value = owner.release();
}

ProposalProcedureSet::ProposalProcedureSet(ProposalProcedureSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ProposalProcedureSet::~ProposalProcedureSet()
{
  cardano_proposal_procedure_set_unref(&m_value);
}

ProposalProcedureSet
ProposalProcedureSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ProposalProcedureSet(decodeCbor<cardano_proposal_procedure_set_t, cardano_proposal_procedure_set_unref>(cbor, cardano_proposal_procedure_set_from_cbor));
}

ProposalProcedureSet
ProposalProcedureSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
ProposalProcedureSet::size() const
{
  return cardano_proposal_procedure_set_get_length(checked());
}

void
ProposalProcedureSet::add(const ProposalProcedure& value)
{
  const ProposalProcedure copy(value);
  checkCardanoError(cardano_proposal_procedure_set_add(checked(), copy.checked()), "ProposalProcedureSet add");
}

ProposalProcedure
ProposalProcedureSet::at(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("ProposalProcedure index is out of range");
  }

  cardano_proposal_procedure_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_proposal_procedure_set_get(checked(), index, &value);
  CardanoObject<cardano_proposal_procedure_t, cardano_proposal_procedure_unref> owner(value);

  checkCardanoError(status, "ProposalProcedureSet at");
  const ProposalProcedure copied(owner.release());
  return ProposalProcedure(copied);
}

std::vector<std::uint8_t>
ProposalProcedureSet::toCbor() const
{
  return encodeCbor(checked(), cardano_proposal_procedure_set_to_cbor);
}

std::string
ProposalProcedureSet::toJson() const
{
  return encodeJson(checked(), cardano_proposal_procedure_set_to_cip116_json);
}

void
ProposalProcedureSet::swap(ProposalProcedureSet& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ProposalProcedureSet&
ProposalProcedureSet::operator=(const ProposalProcedureSet& other)
{
  if (this != &other)
  {
    ProposalProcedureSet copy(other);
    swap(copy);
  }
  return *this;
}

ProposalProcedureSet&
ProposalProcedureSet::operator=(ProposalProcedureSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_proposal_procedure_set_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ProposalProcedureSet::operator==(const ProposalProcedureSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
ProposalProcedureSet::operator!=(const ProposalProcedureSet& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

ProposalProcedureSet::ProposalProcedureSet(cardano_proposal_procedure_set_t* value) noexcept
: m_value(value)
{
}

cardano_proposal_procedure_set_t*
ProposalProcedureSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ProposalProcedureSet");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ProposalProcedureSet& lhs, ProposalProcedureSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
