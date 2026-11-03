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

#include <algorithm>
#include <functional>
#include <iterator>

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
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
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
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_proposal_procedure_set_get_length(checked());
}

void
ProposalProcedureSet::add(const ProposalProcedure& value)
{
  if (m_materialized)
  {
    push_back(value);
    return;
  }

  const ProposalProcedure copy(value);
  checkCardanoError(cardano_proposal_procedure_set_add(checked(), copy.checked()), "ProposalProcedureSet add");
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
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
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
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
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

ProposalProcedureSet::const_reference
ProposalProcedureSet::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

ProposalProcedureSet::iterator
ProposalProcedureSet::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

ProposalProcedureSet::iterator
ProposalProcedureSet::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

ProposalProcedureSet::const_iterator
ProposalProcedureSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

ProposalProcedureSet::const_iterator
ProposalProcedureSet::end() const
{
  materialize();

  return m_elements.cend();
}

ProposalProcedureSet::const_iterator
ProposalProcedureSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

ProposalProcedureSet::const_iterator
ProposalProcedureSet::cend() const
{
  materialize();

  return m_elements.cend();
}

ProposalProcedureSet::reverse_iterator
ProposalProcedureSet::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

ProposalProcedureSet::reverse_iterator
ProposalProcedureSet::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

ProposalProcedureSet::const_reverse_iterator
ProposalProcedureSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

ProposalProcedureSet::const_reverse_iterator
ProposalProcedureSet::rend() const
{
  materialize();

  return m_elements.crend();
}

ProposalProcedureSet::const_reverse_iterator
ProposalProcedureSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

ProposalProcedureSet::const_reverse_iterator
ProposalProcedureSet::crend() const
{
  materialize();

  return m_elements.crend();
}

ProposalProcedureSet::reference
ProposalProcedureSet::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

ProposalProcedureSet::reference
ProposalProcedureSet::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

ProposalProcedureSet::const_reference
ProposalProcedureSet::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

ProposalProcedureSet::reference
ProposalProcedureSet::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

ProposalProcedureSet::const_reference
ProposalProcedureSet::front() const
{
  materialize();

  return m_elements.front();
}

ProposalProcedureSet::reference
ProposalProcedureSet::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

ProposalProcedureSet::const_reference
ProposalProcedureSet::back() const
{
  materialize();

  return m_elements.back();
}

void
ProposalProcedureSet::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
ProposalProcedureSet::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
ProposalProcedureSet::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
ProposalProcedureSet::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
ProposalProcedureSet::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

ProposalProcedureSet::size_type
ProposalProcedureSet::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
ProposalProcedureSet::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

ProposalProcedureSet::iterator
ProposalProcedureSet::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

ProposalProcedureSet::iterator
ProposalProcedureSet::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

ProposalProcedureSet::iterator
ProposalProcedureSet::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

ProposalProcedureSet::iterator
ProposalProcedureSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

bool
ProposalProcedureSet::empty() const
{
  return size() == 0;
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
  synchronize();

  return m_value;
}

void
ProposalProcedureSet::materialize() const
{
  if (m_materialized)
  {
    return;
  }

  const size_type                        count = size();
  std::vector<value_type>                elements;
  std::vector<std::vector<std::uint8_t>> encoding;
  elements.reserve(count);
  encoding.reserve(count);

  for (size_type index = 0; index < count; ++index)
  {
    elements.push_back(copyElement(index));
    encoding.push_back(elements.back().toCbor());
  }

  m_elements.swap(elements);
  m_encoding.swap(encoding);
  m_materialized = true;
}

void
ProposalProcedureSet::synchronize() const
{
  if (!m_materialized || !m_mutableAccess)
  {
    return;
  }

  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size());

  std::transform(
    m_elements.cbegin(),
    m_elements.cend(),
    std::back_inserter(encoding),
    std::mem_fn(&value_type::toCbor));

  if (encoding == m_encoding)
  {
    return;
  }

  ProposalProcedureSet replacement;

  for (const value_type& value: m_elements)
  {
    replacement.add(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

ProposalProcedure
ProposalProcedureSet::copyElement(std::size_t index) const
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

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ProposalProcedureSet& lhs, ProposalProcedureSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
