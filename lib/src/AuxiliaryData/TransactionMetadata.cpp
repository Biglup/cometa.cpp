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

#include <cardano/auxiliary_data/metadatum.h>
#include <cardano/auxiliary_data/transaction_metadata.h>

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

  value.validate();

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

  const Metadatum data(*owner.release());

  return Metadatum(data);
}

Metadatum
TransactionMetadata::at(std::uint64_t label) const
{
  const std::optional<Metadatum> value = get(label);

  if (!value)
  {
    throw std::out_of_range("Metadata label not found");
  }

  return *value;
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
      checkCardanoError(cardano_transaction_metadata_insert(replacement.checked(), entry.first, item.checked()), "TransactionMetadata replacement");
    }

    swap(replacement);
    return;
  }

  checkCardanoError(cardano_transaction_metadata_insert(checked(), label, copy.checked()), "TransactionMetadata set");
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

  const Metadatum data(*owner.release());

  return std::make_pair(label, Metadatum(data));
}

MetadatumLabelList
TransactionMetadata::keys() const
{
  MetadatumLabelList result;
  const std::size_t  count = size();
  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
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
  const std::size_t      count = size();
  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
  {
    result.push_back(entryAt(index).second);
  }

  return result;
}

std::vector<std::pair<std::uint64_t, Metadatum>>
TransactionMetadata::entries() const
{
  std::vector<std::pair<std::uint64_t, Metadatum>> result;
  const std::size_t                                count = size();
  result.reserve(count);

  for (std::size_t index = 0U; index < count; ++index)
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
    m_value = std::exchange(other.m_value, nullptr);
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

  return m_value;
}

void
TransactionMetadata::validate() const
{
  const MetadatumLabelList labels = keys();

  for (std::size_t index = 1U; index < labels.size(); ++index)
  {
    if (labels[index - 1U] == labels[index])
    {
      throw std::invalid_argument("Duplicate metadata label");
    }
  }
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(TransactionMetadata& lhs, TransactionMetadata& rhs) noexcept
{
  lhs.swap(rhs);
}

} // namespace Cometa
