/**
 * @file CertificateSet.cpp
 *
 * @brief Ledger certificate collections.
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

#include <Cometa/Certificates/CertificateSet.h>

#include "../CardanoInterop.h"

#include <cardano/certs/certificate_set.h>

#include <algorithm>
#include <functional>
#include <iterator>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CertificateSet::CertificateSet()
: m_value(nullptr)
{
  cardano_certificate_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_certificate_set_new(&value);
  CardanoObject<cardano_certificate_set_t, cardano_certificate_set_unref> owner(value);

  checkCardanoError(status, "CertificateSet");
  m_value = owner.release();
}

CertificateSet::CertificateSet(const CertificateSet& other)
: m_value(nullptr)
{
  cardano_certificate_set_t*                                              value  = nullptr;
  const cardano_error_t                                                   status = cardano_certificate_set_deep_clone(other.checked(), &value);
  CardanoObject<cardano_certificate_set_t, cardano_certificate_set_unref> owner(value);

  checkCardanoError(status, "CertificateSet copy");
  m_value = owner.release();
}

CertificateSet::CertificateSet(CertificateSet&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

CertificateSet::~CertificateSet()
{
  cardano_certificate_set_unref(&m_value);
}

CertificateSet
CertificateSet::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return CertificateSet(decodeCbor<cardano_certificate_set_t, cardano_certificate_set_unref>(cbor, cardano_certificate_set_from_cbor));
}

CertificateSet
CertificateSet::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
CertificateSet::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_certificate_set_get_length(checked());
}

void
CertificateSet::add(const Certificate& value)
{
  if (m_materialized)
  {
    push_back(value);
    return;
  }

  const Certificate copy(value);
  checkCardanoError(cardano_certificate_set_add(checked(), checkedNativeHandle(copy)), "CertificateSet add");
}

std::vector<std::uint8_t>
CertificateSet::toCbor() const
{
  return encodeCbor(checked(), cardano_certificate_set_to_cbor);
}

std::string
CertificateSet::toJson() const
{
  return encodeJson(checked(), cardano_certificate_set_to_cip116_json);
}

void
CertificateSet::swap(CertificateSet& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

CertificateSet&
CertificateSet::operator=(const CertificateSet& other)
{
  if (this != &other)
  {
    CertificateSet copy(other);
    swap(copy);
  }
  return *this;
}

CertificateSet&
CertificateSet::operator=(CertificateSet&& other) noexcept
{
  if (this != &other)
  {
    cardano_certificate_set_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }
  return *this;
}

bool
CertificateSet::operator==(const CertificateSet& other) const
{
  return toCbor() == other.toCbor();
}

bool
CertificateSet::operator!=(const CertificateSet& other) const
{
  return !(*this == other);
}

CertificateSet::const_reference
CertificateSet::at(std::size_t index) const
{
  materialize();

  return m_elements.at(index);
}

CertificateSet::iterator
CertificateSet::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

CertificateSet::iterator
CertificateSet::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

CertificateSet::const_iterator
CertificateSet::begin() const
{
  materialize();

  return m_elements.cbegin();
}

CertificateSet::const_iterator
CertificateSet::end() const
{
  materialize();

  return m_elements.cend();
}

CertificateSet::const_iterator
CertificateSet::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

CertificateSet::const_iterator
CertificateSet::cend() const
{
  materialize();

  return m_elements.cend();
}

CertificateSet::reverse_iterator
CertificateSet::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

CertificateSet::reverse_iterator
CertificateSet::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

CertificateSet::const_reverse_iterator
CertificateSet::rbegin() const
{
  materialize();

  return m_elements.crbegin();
}

CertificateSet::const_reverse_iterator
CertificateSet::rend() const
{
  materialize();

  return m_elements.crend();
}

CertificateSet::const_reverse_iterator
CertificateSet::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

CertificateSet::const_reverse_iterator
CertificateSet::crend() const
{
  materialize();

  return m_elements.crend();
}

CertificateSet::reference
CertificateSet::at(size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(index);
}

CertificateSet::reference
CertificateSet::operator[](size_type index)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.operator[](index);
}

CertificateSet::const_reference
CertificateSet::operator[](size_type index) const
{
  materialize();

  return m_elements.operator[](index);
}

CertificateSet::reference
CertificateSet::front()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.front();
}

CertificateSet::const_reference
CertificateSet::front() const
{
  materialize();

  return m_elements.front();
}

CertificateSet::reference
CertificateSet::back()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.back();
}

CertificateSet::const_reference
CertificateSet::back() const
{
  materialize();

  return m_elements.back();
}

void
CertificateSet::push_back(const value_type& value)
{
  materialize();
  m_elements.push_back(value);
  m_mutableAccess = true;
}

void
CertificateSet::push_back(value_type&& value)
{
  materialize();
  m_elements.push_back(std::move(value));
  m_mutableAccess = true;
}

void
CertificateSet::pop_back()
{
  materialize();
  m_elements.pop_back();
  m_mutableAccess = true;
}

void
CertificateSet::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

void
CertificateSet::reserve(size_type count)
{
  materialize();
  m_elements.reserve(count);
}

CertificateSet::size_type
CertificateSet::capacity() const
{
  materialize();

  return m_elements.capacity();
}

void
CertificateSet::shrink_to_fit()
{
  materialize();
  m_elements.shrink_to_fit();
}

CertificateSet::iterator
CertificateSet::insert(const_iterator position, const value_type& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, value);
}

CertificateSet::iterator
CertificateSet::insert(const_iterator position, value_type&& value)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.insert(position, std::move(value));
}

CertificateSet::iterator
CertificateSet::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

CertificateSet::iterator
CertificateSet::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

bool
CertificateSet::empty() const
{
  return size() == 0;
}

CertificateSet
CertificateSet::adopt(cardano_certificate_set_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null CertificateSet");
  }

  return CertificateSet(value);
}

cardano_certificate_set_t*
CertificateSet::nativeHandle()
{
  if (m_value)
  {
    synchronize();
  }

  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;

  return m_value;
}

const cardano_certificate_set_t*
CertificateSet::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_certificate_set_t*
CertificateSet::release()
{
  if (m_value)
  {
    synchronize();
  }

  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;

  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

CertificateSet::CertificateSet(cardano_certificate_set_t* value) noexcept
: m_value(value)
{
}

cardano_certificate_set_t*
CertificateSet::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CertificateSet");
  }
  synchronize();

  return m_value;
}

void
CertificateSet::materialize() const
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
CertificateSet::synchronize() const
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

  CertificateSet replacement;

  for (const value_type& value: m_elements)
  {
    replacement.add(value);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

Certificate
CertificateSet::copyElement(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Certificate index is out of range");
  }

  cardano_certificate_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_certificate_set_get(checked(), index, &value);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(value);

  checkCardanoError(status, "CertificateSet at");
  const Certificate copied = Certificate::adopt(owner.release());
  return Certificate(copied);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CertificateSet& lhs, CertificateSet& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
