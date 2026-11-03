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
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
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
  invalidateElements();
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
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
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
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
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

std::size_t
VotingProcedures::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return entries().size();
}

std::vector<std::pair<std::pair<Voter, GovernanceActionId>, VotingProcedure>>
VotingProcedures::entries() const
{
  std::vector<std::pair<std::pair<Voter, GovernanceActionId>, VotingProcedure>> result;
  for (const Voter& voter: voters())
  {
    for (const GovernanceActionId& action: actionIds(voter))
    {
      const VotingProcedure procedure = get(voter, action).value();

      result.emplace_back(std::make_pair(voter, action), procedure);
    }
  }

  return result;
}

VotingProcedures::iterator
VotingProcedures::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

VotingProcedures::const_iterator
VotingProcedures::begin() const
{
  materialize();

  return m_elements.begin();
}

VotingProcedures::iterator
VotingProcedures::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

VotingProcedures::const_iterator
VotingProcedures::end() const
{
  materialize();

  return m_elements.end();
}

VotingProcedures::reverse_iterator
VotingProcedures::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

VotingProcedures::const_reverse_iterator
VotingProcedures::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

VotingProcedures::reverse_iterator
VotingProcedures::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

VotingProcedures::const_reverse_iterator
VotingProcedures::rend() const
{
  materialize();

  return m_elements.rend();
}

VotingProcedures::const_iterator
VotingProcedures::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

VotingProcedures::const_iterator
VotingProcedures::cend() const
{
  materialize();

  return m_elements.cend();
}

VotingProcedures::const_reverse_iterator
VotingProcedures::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

VotingProcedures::const_reverse_iterator
VotingProcedures::crend() const
{
  materialize();

  return m_elements.crend();
}

VotingProcedures::iterator
VotingProcedures::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

VotingProcedures::const_iterator
VotingProcedures::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

VotingProcedures::size_type
VotingProcedures::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
VotingProcedures::contains(const key_type& key) const
{
  return count(key) != 0;
}

VotingProcedures::mapped_type&
VotingProcedures::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const VotingProcedures::mapped_type&
VotingProcedures::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<VotingProcedures::iterator, bool>
VotingProcedures::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  VotingProcedures validation;
  validation.putEntry(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

VotingProcedures::iterator
VotingProcedures::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<VotingProcedures::iterator, bool>
VotingProcedures::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  VotingProcedures validation;
  validation.putEntry(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

VotingProcedures::iterator
VotingProcedures::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

VotingProcedures::iterator
VotingProcedures::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

VotingProcedures::size_type
VotingProcedures::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
VotingProcedures::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

bool
VotingProcedures::empty() const
{
  return size() == 0;
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
  synchronize();

  return m_value;
}

void
VotingProcedures::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  const std::vector<std::pair<key_type, mapped_type>> entriesCopy = entries();
  std::map<key_type, mapped_type, key_compare>        elements(compareKeys);

  for (const std::pair<key_type, mapped_type>& entry: entriesCopy)
  {
    elements.emplace(entry.first, entry.second);
  }

  m_elements.swap(elements);

  try
  {
    std::vector<std::vector<std::uint8_t>> encoding = encodeElements();
    m_encoding.swap(encoding);
    m_materialized = true;
  }
  catch (...)
  {
    m_elements.clear();
    throw;
  }
}

void
VotingProcedures::synchronize() const
{
  if (!m_materialized || !m_mutableAccess)
  {
    return;
  }

  std::vector<std::vector<std::uint8_t>> encoding = encodeElements();

  if (encoding == m_encoding)
  {
    return;
  }

  VotingProcedures replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.putEntry(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
VotingProcedures::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
VotingProcedures::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    encoding.push_back(entry.first.first.toCbor());
    encoding.push_back(entry.first.second.toCbor());
    encoding.push_back(entry.second.toCbor());
  }

  return encoding;
}

bool
VotingProcedures::compareKeys(const key_type& left, const key_type& right)
{
  return left.first < right.first || (!(right.first < left.first) && left.second.toCbor() < right.second.toCbor());
}

void
VotingProcedures::putEntry(const std::pair<Voter, GovernanceActionId>& key, const VotingProcedure& value)
{
  insert(key.first, key.second, value);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(VotingProcedures& lhs, VotingProcedures& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
