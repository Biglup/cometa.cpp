/**
 * @file RequiredGuardsMap.cpp
 *
 * @brief Guard credentials and their optional datums.
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

#include <Cometa/TransactionBody/RequiredGuardsMap.h>

#include "../CardanoInterop.h"

#include <cardano/common/credential.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/transaction_body/required_guards_map.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
RequiredGuardsMap::RequiredGuardsMap()
: m_value(nullptr)
{
  cardano_required_guards_map_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_required_guards_map_new(&value);
  CardanoObject<cardano_required_guards_map_t, cardano_required_guards_map_unref> owner(value);

  checkCardanoError(status, "RequiredGuardsMap");
  m_value = owner.release();
}

RequiredGuardsMap::RequiredGuardsMap(const RequiredGuardsMap& other)
: m_value(nullptr)
{
  cardano_required_guards_map_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_required_guards_map_deep_clone(other.checked(), &value);
  CardanoObject<cardano_required_guards_map_t, cardano_required_guards_map_unref> owner(value);

  checkCardanoError(status, "RequiredGuardsMap copy");
  m_value = owner.release();
}

RequiredGuardsMap::RequiredGuardsMap(RequiredGuardsMap&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

RequiredGuardsMap::~RequiredGuardsMap()
{
  cardano_required_guards_map_unref(&m_value);
}

RequiredGuardsMap
RequiredGuardsMap::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return RequiredGuardsMap(decodeCbor<cardano_required_guards_map_t, cardano_required_guards_map_unref>(cbor, cardano_required_guards_map_from_cbor));
}

RequiredGuardsMap
RequiredGuardsMap::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
RequiredGuardsMap::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_required_guards_map_get_length(checked());
}

bool
RequiredGuardsMap::empty() const
{
  return size() == 0U;
}

bool
RequiredGuardsMap::contains(const Credential& credential) const
{
  cardano_plutus_data_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_required_guards_map_get(checked(), credential.checked(), &value);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return false;
  }

  checkCardanoError(status, "RequiredGuardsMap contains");
  return true;
}

std::optional<std::optional<PlutusData>>
RequiredGuardsMap::get(const Credential& credential) const
{
  cardano_plutus_data_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_required_guards_map_get(checked(), credential.checked(), &value);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "RequiredGuardsMap get");
  if (!owner.get())
  {
    return std::make_optional(std::optional<PlutusData>());
  }

  const PlutusData copied(*owner.release());
  return std::make_optional(std::make_optional(PlutusData(copied)));
}

void
RequiredGuardsMap::insert(const Credential& credential, const std::optional<PlutusData>& datum)
{
  const Credential copyCredential(credential);

  if (datum)
  {
    const PlutusData copyDatum(*datum);
    checkCardanoError(cardano_required_guards_map_insert(checked(), copyCredential.checked(), copyDatum.checked()), "RequiredGuardsMap insert");
  }
  else
  {
    checkCardanoError(cardano_required_guards_map_insert(checked(), copyCredential.checked(), nullptr), "RequiredGuardsMap insert");
  }
  invalidateElements();
}

std::pair<Credential, std::optional<PlutusData>>
RequiredGuardsMap::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("Required guard index is out of range");
  }

  cardano_credential_t*                                           key    = nullptr;
  cardano_plutus_data_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_required_guards_map_get_key_value_at(checked(), index, &key, &value);
  CardanoObject<cardano_credential_t, cardano_credential_unref>   keyOwner(key);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> valueOwner(value);

  checkCardanoError(status, "RequiredGuardsMap entryAt");
  Credential                credential(keyOwner.release());
  std::optional<PlutusData> datum;

  if (valueOwner.get())
  {
    PlutusData copied(*valueOwner.release());
    datum = std::move(copied);
  }

  return std::make_pair(std::move(credential), std::move(datum));
}

std::vector<Credential>
RequiredGuardsMap::keys() const
{
  std::vector<Credential> result;
  result.reserve(size());

  for (std::size_t index = 0; index < size(); ++index)
  {
    result.push_back(entryAt(index).first);
  }

  return result;
}

std::vector<std::pair<Credential, std::optional<PlutusData>>>
RequiredGuardsMap::entries() const
{
  std::vector<std::pair<Credential, std::optional<PlutusData>>> result;
  result.reserve(size());

  for (std::size_t index = 0; index < size(); ++index)
  {
    result.push_back(entryAt(index));
  }

  return result;
}

std::vector<std::uint8_t>
RequiredGuardsMap::toCbor() const
{
  return encodeCbor(checked(), cardano_required_guards_map_to_cbor);
}

std::string
RequiredGuardsMap::toJson() const
{
  return encodeJson(checked(), cardano_required_guards_map_to_cip116_json);
}

void
RequiredGuardsMap::swap(RequiredGuardsMap& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

RequiredGuardsMap&
RequiredGuardsMap::operator=(const RequiredGuardsMap& other)
{
  if (this != &other)
  {
    RequiredGuardsMap copy(other);
    swap(copy);
  }
  return *this;
}

RequiredGuardsMap&
RequiredGuardsMap::operator=(RequiredGuardsMap&& other) noexcept
{
  if (this != &other)
  {
    cardano_required_guards_map_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }
  return *this;
}

bool
RequiredGuardsMap::operator==(const RequiredGuardsMap& other) const
{
  return toCbor() == other.toCbor();
}

bool
RequiredGuardsMap::operator!=(const RequiredGuardsMap& other) const
{
  return !(*this == other);
}

RequiredGuardsMap::iterator
RequiredGuardsMap::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

RequiredGuardsMap::const_iterator
RequiredGuardsMap::begin() const
{
  materialize();

  return m_elements.begin();
}

RequiredGuardsMap::iterator
RequiredGuardsMap::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

RequiredGuardsMap::const_iterator
RequiredGuardsMap::end() const
{
  materialize();

  return m_elements.end();
}

RequiredGuardsMap::reverse_iterator
RequiredGuardsMap::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

RequiredGuardsMap::const_reverse_iterator
RequiredGuardsMap::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

RequiredGuardsMap::reverse_iterator
RequiredGuardsMap::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

RequiredGuardsMap::const_reverse_iterator
RequiredGuardsMap::rend() const
{
  materialize();

  return m_elements.rend();
}

RequiredGuardsMap::const_iterator
RequiredGuardsMap::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

RequiredGuardsMap::const_iterator
RequiredGuardsMap::cend() const
{
  materialize();

  return m_elements.cend();
}

RequiredGuardsMap::const_reverse_iterator
RequiredGuardsMap::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

RequiredGuardsMap::const_reverse_iterator
RequiredGuardsMap::crend() const
{
  materialize();

  return m_elements.crend();
}

RequiredGuardsMap::iterator
RequiredGuardsMap::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

RequiredGuardsMap::const_iterator
RequiredGuardsMap::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

RequiredGuardsMap::size_type
RequiredGuardsMap::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

RequiredGuardsMap::mapped_type&
RequiredGuardsMap::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const RequiredGuardsMap::mapped_type&
RequiredGuardsMap::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<RequiredGuardsMap::iterator, bool>
RequiredGuardsMap::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  RequiredGuardsMap validation;
  validation.insert(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

RequiredGuardsMap::iterator
RequiredGuardsMap::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<RequiredGuardsMap::iterator, bool>
RequiredGuardsMap::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  RequiredGuardsMap validation;
  validation.insert(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

RequiredGuardsMap::mapped_type&
RequiredGuardsMap::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

RequiredGuardsMap::iterator
RequiredGuardsMap::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

RequiredGuardsMap::iterator
RequiredGuardsMap::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

RequiredGuardsMap::size_type
RequiredGuardsMap::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
RequiredGuardsMap::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

/* PRIVATE FUNCTIONS *********************************************************/

RequiredGuardsMap::RequiredGuardsMap(cardano_required_guards_map_t* value) noexcept
: m_value(value)
{
}

cardano_required_guards_map_t*
RequiredGuardsMap::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from RequiredGuardsMap");
  }
  synchronize();

  return m_value;
}

void
RequiredGuardsMap::materialize() const
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
RequiredGuardsMap::synchronize() const
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

  RequiredGuardsMap replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.insert(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
RequiredGuardsMap::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
RequiredGuardsMap::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    encoding.push_back(entry.first.toCbor());
    encoding.push_back(entry.second ? entry.second->toCbor() : std::vector<std::uint8_t>());
  }

  return encoding;
}

bool
RequiredGuardsMap::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RequiredGuardsMap& lhs, RequiredGuardsMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
