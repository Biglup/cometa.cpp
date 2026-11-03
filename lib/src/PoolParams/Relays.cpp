/**
 * @file Relays.cpp
 *
 * @brief An ordered sequence of pool relays.
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

#include <Cometa/PoolParams/Relays.h>

#include <Cometa/PoolParams/Relay.h>

#include "../CardanoInterop.h"

#include <cardano/pool_params/relay.h>
#include <cardano/pool_params/relays.h>

#include <algorithm>
#include <functional>
#include <iterator>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Relays::Relays()
: m_value(nullptr)
{
  cardano_relays_t*     value  = nullptr;
  const cardano_error_t result = cardano_relays_new(&value);

  CardanoObject<cardano_relays_t, cardano_relays_unref> owner(value);

  checkCardanoError(result, "Relays");

  m_value = owner.release();
}

Relays::Relays(std::initializer_list<Relay> relays)
: Relays()
{
  for (const Relay& relay: relays)
  {
    pushBack(relay);
  }
}

Relays::Relays(const Relays& other)
: m_value(nullptr)
{
  cardano_relays_t*     value  = nullptr;
  const cardano_error_t result = cardano_relays_deep_clone(other.checked(), &value);

  CardanoObject<cardano_relays_t, cardano_relays_unref> owner(value);

  checkCardanoError(result, "Relays");

  m_value = owner.release();
}

Relays::Relays(Relays&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

Relays::~Relays()
{
  cardano_relays_unref(&m_value);
}

Relays
Relays::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  Relays value(*decodeCbor<cardano_relays_t, cardano_relays_unref>(cbor, cardano_relays_from_cbor));

  return value;
}

Relays
Relays::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
Relays::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_relays_get_length(checked());
}

bool
Relays::empty() const
{
  return size() == 0U;
}

void
Relays::pushBack(const Relay& relay)
{
  if (m_materialized)
  {
    push_back(relay);
    return;
  }

  Relay copy(relay);

  checkCardanoError(cardano_relays_add(checked(), copy.checked()), "Relays pushBack");
}

std::vector<Relay>
Relays::values() const
{
  std::vector<Relay> result;
  const std::size_t  count = size();

  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(at(index));
  }

  return result;
}

std::vector<std::uint8_t>
Relays::toCbor() const
{
  return encodeCbor(checked(), cardano_relays_to_cbor);
}

std::string
Relays::toJson() const
{
  return encodeJson(checked(), cardano_relays_to_cip116_json);
}

void
Relays::swap(Relays& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

Relays&
Relays::operator=(const Relays& other)
{
  if (this != &other)
  {
    Relays copy(other);

    swap(copy);
  }

  return *this;
}

Relays&
Relays::operator=(Relays&& other) noexcept
{
  if (this != &other)
  {
    cardano_relays_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
Relays::operator==(const Relays& other) const
{
  return values() == other.values();
}

bool
Relays::operator!=(const Relays& other) const
{
  return !(*this == other);
}

Relays::const_reference
Relays::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

Relays::iterator
Relays::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

Relays::iterator
Relays::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

Relays::const_iterator
Relays::begin() const
{
  materialize();

  return m_elements.cbegin();
}

Relays::const_iterator
Relays::end() const
{
  materialize();

  return m_elements.cend();
}

Relays::const_iterator
Relays::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

Relays::const_iterator
Relays::cend() const
{
  materialize();

  return m_elements.cend();
}

Relays::reverse_iterator
Relays::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

Relays::reverse_iterator
Relays::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

Relays::const_reverse_iterator
Relays::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

Relays::const_reverse_iterator
Relays::rend() const
{
  materialize();

  return m_elements.crend();
}

Relays::const_reverse_iterator
Relays::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

Relays::const_reverse_iterator
Relays::crend() const
{
  materialize();

  return m_elements.crend();
}

Relays::reference
Relays::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

Relays::reference
Relays::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

Relays::const_reference
Relays::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

Relays::reference
Relays::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

Relays::const_reference
Relays::front() const
{
  materialize();

  return m_elements.front();
}

Relays::reference
Relays::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

Relays::const_reference
Relays::back() const
{
  materialize();

  return m_elements.back();
}

void
Relays::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
Relays::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
Relays::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
Relays::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
Relays::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

Relays::size_type
Relays::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
Relays::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

Relays::iterator
Relays::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

Relays::iterator
Relays::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

Relays::iterator
Relays::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

Relays::iterator
Relays::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

/* PRIVATE FUNCTIONS *********************************************************/

Relays::Relays(cardano_relays_t& value) noexcept
: m_value(&value)
{
}

cardano_relays_t*
Relays::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Relays");
  }

  synchronize();

  return m_value;
}

void
Relays::materialize() const
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
Relays::synchronize() const
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

  Relays replacement;

  for (const value_type& value: m_elements)
  {
    replacement.pushBack(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

Relay
Relays::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Relays index");
  }

  cardano_relay_t*      value  = nullptr;
  const cardano_error_t result = cardano_relays_get(checked(), index, &value);

  CardanoObject<cardano_relay_t, cardano_relay_unref> owner(value);

  checkCardanoError(result, "Relays at");

  const Relay relay(*owner.release());

  return Relay(relay);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Relays& lhs, Relays& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
