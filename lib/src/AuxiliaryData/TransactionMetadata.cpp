/**
 * @file TransactionMetadata.cpp
 *
 * @brief Transaction metadata indexed by unsigned labels.
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

#include <Cometa/AuxiliaryData/TransactionMetadata.h>

#include "../CardanoInterop.h"
#include "../ValueValidation.h"

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/transaction_metadata.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
TransactionMetadata::TransactionMetadata()
: m_value(nullptr)
{
  cardano_transaction_metadata_t* value  = nullptr;
  const cardano_error_t           result = cardano_transaction_metadata_new(&value);

  CardanoObject<cardano_transaction_metadata_t, cardano_transaction_metadata_unref> owner(value);

  checkCardanoError(result, "TransactionMetadata");

  m_value = owner.release();
}

TransactionMetadata::TransactionMetadata(const TransactionMetadata& other)
: m_value(nullptr)
{
  cardano_transaction_metadata_t* value  = nullptr;
  const cardano_error_t           result = cardano_transaction_metadata_deep_clone(other.checked(), &value);

  CardanoObject<cardano_transaction_metadata_t, cardano_transaction_metadata_unref> owner(value);

  checkCardanoError(result, "TransactionMetadata");

  m_value = owner.release();
}

TransactionMetadata::TransactionMetadata(TransactionMetadata&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

TransactionMetadata::~TransactionMetadata()
{
  cardano_transaction_metadata_unref(&m_value);
}

TransactionMetadata
TransactionMetadata::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  TransactionMetadata value(*decodeCbor<cardano_transaction_metadata_t, cardano_transaction_metadata_unref>(cbor, cardano_transaction_metadata_from_cbor));

  validateValue(value);

  return value;
}

TransactionMetadata
TransactionMetadata::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
TransactionMetadata::size() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_transaction_metadata_get_length(checked());
}

bool
TransactionMetadata::empty() const
{
  return size() == 0U;
}

std::optional<Metadatum>
TransactionMetadata::get(std::uint64_t label) const
{
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_transaction_metadata_get(checked(), label, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "TransactionMetadata get");

  const Metadatum data = Metadatum::adopt(owner.release());

  return Metadatum(data);
}

void
TransactionMetadata::set(std::uint64_t label, const Metadatum& value)
{
  Metadatum copy(value);

  if (get(label))
  {
    TransactionMetadata                                    replacement;
    const std::vector<std::pair<std::uint64_t, Metadatum>> current = entries();

    for (const std::pair<std::uint64_t, Metadatum>& entry: current)
    {
      const Metadatum& item = entry.first == label ? copy : entry.second;
      checkCardanoError(cardano_transaction_metadata_insert(replacement.checked(), entry.first, checkedNativeHandle(item)), "TransactionMetadata replacement");
    }

    swap(replacement);
    return;
  }

  checkCardanoError(cardano_transaction_metadata_insert(checked(), label, checkedNativeHandle(copy)), "TransactionMetadata set");
  invalidateElements();
}

std::pair<std::uint64_t, Metadatum>
TransactionMetadata::entryAt(std::size_t index) const
{
  if (index >= size())
  {
    throw std::out_of_range("TransactionMetadata index");
  }

  std::uint64_t         label  = 0U;
  cardano_metadatum_t*  value  = nullptr;
  const cardano_error_t result = cardano_transaction_metadata_get_key_value_at(checked(), index, &label, &value);

  CardanoObject<cardano_metadatum_t, cardano_metadatum_unref> owner(value);

  checkCardanoError(result, "TransactionMetadata entryAt");

  const Metadatum data = Metadatum::adopt(owner.release());

  return std::make_pair(label, Metadatum(data));
}

MetadatumLabelList
TransactionMetadata::keys() const
{
  MetadatumLabelList result;
  const std::size_t  elementCount = size();
  result.reserve(elementCount);

  for (std::size_t index = 0U; index < elementCount; ++index)
  {
    std::uint64_t label = 0U;
    checkCardanoError(cardano_transaction_metadata_get_key_at(checked(), index, &label), "TransactionMetadata keys");
    result.push_back(label);
  }

  return result;
}

std::vector<Metadatum>
TransactionMetadata::values() const
{
  std::vector<Metadatum> result;
  const std::size_t      elementCount = size();
  result.reserve(elementCount);

  for (std::size_t index = 0U; index < elementCount; ++index)
  {
    result.push_back(entryAt(index).second);
  }

  return result;
}

std::vector<std::pair<std::uint64_t, Metadatum>>
TransactionMetadata::entries() const
{
  std::vector<std::pair<std::uint64_t, Metadatum>> result;
  const std::size_t                                elementCount = size();
  result.reserve(elementCount);

  for (std::size_t index = 0U; index < elementCount; ++index)
  {
    result.push_back(entryAt(index));
  }

  return result;
}

std::vector<std::uint8_t>
TransactionMetadata::toCbor() const
{
  return encodeCbor(checked(), cardano_transaction_metadata_to_cbor);
}

std::string
TransactionMetadata::toJson() const
{
  return encodeJson(checked(), cardano_transaction_metadata_to_cip116_json);
}

void
TransactionMetadata::swap(TransactionMetadata& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

TransactionMetadata&
TransactionMetadata::operator=(const TransactionMetadata& other)
{
  if (this != &other)
  {
    TransactionMetadata copy(other);

    swap(copy);
  }

  return *this;
}

TransactionMetadata&
TransactionMetadata::operator=(TransactionMetadata&& other) noexcept
{
  if (this != &other)
  {
    cardano_transaction_metadata_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

bool
TransactionMetadata::operator==(const TransactionMetadata& other) const
{
  return entries() == other.entries();
}

bool
TransactionMetadata::operator!=(const TransactionMetadata& other) const
{
  return !(*this == other);
}

TransactionMetadata::iterator
TransactionMetadata::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

TransactionMetadata::const_iterator
TransactionMetadata::begin() const
{
  materialize();

  return m_elements.begin();
}

TransactionMetadata::iterator
TransactionMetadata::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

TransactionMetadata::const_iterator
TransactionMetadata::end() const
{
  materialize();

  return m_elements.end();
}

TransactionMetadata::reverse_iterator
TransactionMetadata::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

TransactionMetadata::const_reverse_iterator
TransactionMetadata::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

TransactionMetadata::reverse_iterator
TransactionMetadata::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

TransactionMetadata::const_reverse_iterator
TransactionMetadata::rend() const
{
  materialize();

  return m_elements.rend();
}

TransactionMetadata::const_iterator
TransactionMetadata::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

TransactionMetadata::const_iterator
TransactionMetadata::cend() const
{
  materialize();

  return m_elements.cend();
}

TransactionMetadata::const_reverse_iterator
TransactionMetadata::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

TransactionMetadata::const_reverse_iterator
TransactionMetadata::crend() const
{
  materialize();

  return m_elements.crend();
}

TransactionMetadata::iterator
TransactionMetadata::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

TransactionMetadata::const_iterator
TransactionMetadata::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

TransactionMetadata::size_type
TransactionMetadata::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
TransactionMetadata::contains(const key_type& key) const
{
  return count(key) != 0;
}

TransactionMetadata::mapped_type&
TransactionMetadata::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const TransactionMetadata::mapped_type&
TransactionMetadata::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<TransactionMetadata::iterator, bool>
TransactionMetadata::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  TransactionMetadata validation;
  validation.set(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

TransactionMetadata::iterator
TransactionMetadata::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<TransactionMetadata::iterator, bool>
TransactionMetadata::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  TransactionMetadata validation;
  validation.set(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

TransactionMetadata::iterator
TransactionMetadata::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

TransactionMetadata::iterator
TransactionMetadata::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

TransactionMetadata::size_type
TransactionMetadata::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
TransactionMetadata::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

TransactionMetadata
TransactionMetadata::adopt(cardano_transaction_metadata_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null TransactionMetadata");
  }

  return TransactionMetadata(*value);
}

cardano_transaction_metadata_t*
TransactionMetadata::nativeHandle()
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

const cardano_transaction_metadata_t*
TransactionMetadata::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_transaction_metadata_t*
TransactionMetadata::release()
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

TransactionMetadata::TransactionMetadata(cardano_transaction_metadata_t& value) noexcept
: m_value(&value)
{
}

cardano_transaction_metadata_t*
TransactionMetadata::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from TransactionMetadata");
  }

  synchronize();

  return m_value;
}

void
validateValue(const TransactionMetadata& value)
{
  const MetadatumLabelList labels = value.keys();

  for (std::size_t index = 1U; index < labels.size(); ++index)
  {
    if (labels[index - 1U] == labels[index])
    {
      throw std::invalid_argument("Duplicate metadata label");
    }
  }
}

void
TransactionMetadata::materialize() const
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
TransactionMetadata::synchronize() const
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

  TransactionMetadata replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.set(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
TransactionMetadata::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
TransactionMetadata::encodeElements() const
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
TransactionMetadata::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionMetadata& lhs, TransactionMetadata& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
