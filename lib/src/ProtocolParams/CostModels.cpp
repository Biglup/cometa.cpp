/**
 * @file CostModels.cpp
 *
 * @brief Plutus cost models indexed by language.
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

#include <Cometa/ProtocolParams/CostModels.h>

#include "../CardanoInterop.h"

#include <cardano/buffer.h>
#include <cardano/protocol_params/costmdls.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CostModels::CostModels()
: m_value(nullptr)
{
  cardano_costmdls_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_costmdls_new(&value);
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> owner(value);

  checkCardanoError(status, "CostModels");
  m_value = owner.release();
}

CostModels::CostModels(const CostModels& other)
: m_value(nullptr)
{
  cardano_costmdls_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_costmdls_deep_clone(other.checked(), &value);
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> owner(value);

  checkCardanoError(status, "CostModels copy");
  m_value = owner.release();
}

CostModels::CostModels(CostModels&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

CostModels::~CostModels()
{
  cardano_costmdls_unref(&m_value);
}

CostModels
CostModels::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return CostModels(decodeCbor<cardano_costmdls_t, cardano_costmdls_unref>(cbor, cardano_costmdls_from_cbor));
}

CostModels
CostModels::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

void
CostModels::insert(const CostModel& model)
{
  const CostModel copy(model);
  checkCardanoError(cardano_costmdls_insert(checked(), copy.checked()), "CostModels insert");
  invalidateElements();
}

bool
CostModels::contains(PlutusLanguageVersion language) const
{
  return cardano_costmdls_has(checked(), static_cast<cardano_plutus_language_version_t>(language));
}

bool
CostModels::contains(std::uint8_t languageId) const
{
  return count(languageId) != 0;
}

std::optional<CostModel>
CostModels::get(std::uint8_t languageId) const
{
  cardano_cost_model_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_costmdls_get_by_language_id(checked(), languageId, &value);
  CardanoObject<cardano_cost_model_t, cardano_cost_model_unref> owner(value);

  checkCardanoError(status, "CostModels get");

  if (!value)
  {
    return std::nullopt;
  }

  const CostModel borrowed(owner.release());
  return CostModel(borrowed);
}

std::optional<CostModel>
CostModels::get(PlutusLanguageVersion language) const
{
  return get(static_cast<std::uint8_t>(language));
}

std::vector<std::uint8_t>
CostModels::languageViews() const
{
  cardano_buffer_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_costmdls_get_language_views_encoding(checked(), &value);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(status, "CostModels languageViews");

  const std::size_t length = cardano_buffer_get_size(owner.get());

  if (length == 0)
  {
    return {};
  }

  const std::uint8_t* bytes = cardano_buffer_get_data(owner.get());
  return std::vector<std::uint8_t>(bytes, bytes + length);
}

std::vector<std::uint8_t>
CostModels::toCbor() const
{
  return encodeCbor(checked(), cardano_costmdls_to_cbor);
}

std::string
CostModels::toJson() const
{
  return encodeJson(checked(), cardano_costmdls_to_cip116_json);
}

void
CostModels::swap(CostModels& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

CostModels&
CostModels::operator=(const CostModels& other)
{
  if (this != &other)
  {
    CostModels copy(other);
    swap(copy);
  }
  return *this;
}

CostModels&
CostModels::operator=(CostModels&& other) noexcept
{
  if (this != &other)
  {
    cardano_costmdls_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }
  return *this;
}

bool
CostModels::operator==(const CostModels& other) const
{
  return toCbor() == other.toCbor();
}

bool
CostModels::operator!=(const CostModels& other) const
{
  return !(*this == other);
}

std::size_t
CostModels::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return entries().size();
}

std::vector<std::pair<std::uint8_t, CostModel>>
CostModels::entries() const
{
  std::vector<std::pair<std::uint8_t, CostModel>> result;
  for (std::uint16_t language = 0; language <= 255; ++language)
  {
    const std::optional<CostModel> model = get(static_cast<std::uint8_t>(language));

    if (model)
    {
      result.emplace_back(static_cast<std::uint8_t>(language), *model);
    }
  }

  return result;
}

CostModels::iterator
CostModels::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

CostModels::const_iterator
CostModels::begin() const
{
  materialize();

  return m_elements.begin();
}

CostModels::iterator
CostModels::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

CostModels::const_iterator
CostModels::end() const
{
  materialize();

  return m_elements.end();
}

CostModels::reverse_iterator
CostModels::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

CostModels::const_reverse_iterator
CostModels::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

CostModels::reverse_iterator
CostModels::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

CostModels::const_reverse_iterator
CostModels::rend() const
{
  materialize();

  return m_elements.rend();
}

CostModels::const_iterator
CostModels::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

CostModels::const_iterator
CostModels::cend() const
{
  materialize();

  return m_elements.cend();
}

CostModels::const_reverse_iterator
CostModels::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

CostModels::const_reverse_iterator
CostModels::crend() const
{
  materialize();

  return m_elements.crend();
}

CostModels::iterator
CostModels::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

CostModels::const_iterator
CostModels::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

CostModels::size_type
CostModels::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

CostModels::mapped_type&
CostModels::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const CostModels::mapped_type&
CostModels::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<CostModels::iterator, bool>
CostModels::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  CostModels validation;
  validation.putEntry(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

CostModels::iterator
CostModels::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<CostModels::iterator, bool>
CostModels::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  CostModels validation;
  validation.putEntry(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

CostModels::iterator
CostModels::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

CostModels::iterator
CostModels::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

CostModels::size_type
CostModels::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
CostModels::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

bool
CostModels::empty() const
{
  return size() == 0;
}

/* PRIVATE FUNCTIONS *********************************************************/

CostModels::CostModels(cardano_costmdls_t* value) noexcept
: m_value(value)
{
}

cardano_costmdls_t*
CostModels::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CostModels");
  }
  synchronize();

  return m_value;
}

void
CostModels::materialize() const
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
CostModels::synchronize() const
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

  CostModels replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.putEntry(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
CostModels::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
CostModels::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    const std::string first = std::to_string(entry.first);
    encoding.emplace_back(first.begin(), first.end());
    encoding.push_back(entry.second.toCbor());
  }

  return encoding;
}

bool
CostModels::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

void
CostModels::putEntry(const std::uint8_t& key, const CostModel& value)
{
  if (key != value.languageId())
  {
    throw std::invalid_argument("Cost model language does not match the key");
  }

  insert(value);
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CostModels& lhs, CostModels& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
