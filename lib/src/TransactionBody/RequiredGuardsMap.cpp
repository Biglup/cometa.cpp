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

std::optional<PlutusData>
RequiredGuardsMap::at(const Credential& credential) const
{
  std::optional<std::optional<PlutusData>> value = get(credential);
  if (!value)
  {
    throw std::out_of_range("Required guard credential not found");
  }

  return std::move(*value);
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
    m_value = std::exchange(other.m_value, nullptr);
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
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(RequiredGuardsMap& lhs, RequiredGuardsMap& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
