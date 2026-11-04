/**
 * @file MultiAsset.cpp
 *
 * @brief Policy-indexed asset operations.
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

#include <Cometa/Assets/MultiAsset.h>

#include <Cometa/Cbor/CborReader.h>

#include "../CardanoInterop.h"
#include "AssetArithmetic.h"

#include <cardano/assets/multi_asset.h>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/**
 * @brief Validates a policy hash length before native operations.
 *
 * @param policyId The policy hash.
 *
 * @throws std::invalid_argument The hash does not contain 28 bytes.
 */
void
validatePolicy(const Cometa::Blake2bHash& policyId)
{
  if (policyId.toBytes().size() != 28)
  {
    throw std::invalid_argument("Policy ID must contain 28 bytes");
  }
}
} // namespace

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
MultiAsset::MultiAsset()
: m_value(nullptr)
{
  cardano_multi_asset_t*                                          value  = nullptr;
  const cardano_error_t                                           result = cardano_multi_asset_new(&value);
  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(value);

  checkCardanoError(result, "MultiAsset");
  m_value = owner.release();
}

MultiAsset::MultiAsset(const MultiAsset& other)
: m_value(nullptr)
{
  cardano_multi_asset_t*                                          value  = nullptr;
  const cardano_error_t                                           result = cardano_multi_asset_deep_clone(other.checked(), &value);
  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(value);

  checkCardanoError(result, "MultiAsset copy");
  m_value = owner.release();
}

MultiAsset::MultiAsset(MultiAsset&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_elements(std::move(other.m_elements))
, m_encoding(std::move(other.m_encoding))
, m_materialized(std::exchange(other.m_materialized, false))
, m_mutableAccess(std::exchange(other.m_mutableAccess, false))
{
}

MultiAsset::~MultiAsset()
{
  cardano_multi_asset_unref(&m_value);
}

MultiAsset
MultiAsset::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  CborReader reader(cbor);
  MultiAsset result;
  reader.readStartMap();

  while (reader.peekState() != CborReaderState::EndMap)
  {
    const Blake2bHash policy(reader.readBytes());
    validatePolicy(policy);

    if (result.assets(policy))
    {
      throw std::invalid_argument("Duplicate policy ID in CBOR map");
    }

    const AssetNameMap quantities = AssetNameMap::fromCbor(reader.readEncodedValue());
    result.setAssets(policy, quantities);
  }

  reader.readEndMap();

  if (reader.bytesRemaining() != 0)
  {
    throw std::invalid_argument("Trailing CBOR bytes");
  }

  return result;
}

MultiAsset
MultiAsset::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::size_t
MultiAsset::policyCount() const
{
  if (m_materialized)
  {
    return m_elements.size();
  }

  return cardano_multi_asset_get_policy_count(checked());
}

std::optional<AssetNameMap>
MultiAsset::assets(const Blake2bHash& policyId) const
{
  validatePolicy(policyId);

  cardano_asset_name_map_t*                                             value  = nullptr;
  const cardano_error_t                                                 result = cardano_multi_asset_get_assets(checked(), checkedNativeHandle(policyId), &value);
  CardanoObject<cardano_asset_name_map_t, cardano_asset_name_map_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "MultiAsset assets");
  const AssetNameMap map = AssetNameMap::adopt(owner.release());

  return map;
}

void
MultiAsset::setAssets(const Blake2bHash& policyId, const AssetNameMap& assets)
{
  validatePolicy(policyId);

  const Blake2bHash  policyCopy(policyId);
  const AssetNameMap assetsCopy(assets);

  checkCardanoError(cardano_multi_asset_insert_assets(checked(), checkedNativeHandle(policyCopy), checkedNativeHandle(assetsCopy)), "MultiAsset setAssets");
  invalidateElements();
}

std::optional<std::int64_t>
MultiAsset::get(const Blake2bHash& policyId, const AssetName& name) const
{
  validatePolicy(policyId);

  std::int64_t          amount = 0;
  const cardano_error_t result = cardano_multi_asset_get(checked(), checkedNativeHandle(policyId), checkedNativeHandle(name), &amount);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "MultiAsset get");
  return amount;
}

std::optional<std::int64_t>
MultiAsset::get(const AssetId& id) const
{
  if (id.isLovelace())
  {
    throw std::invalid_argument("MultiAsset does not contain lovelace");
  }

  return get(*id.policyId(), *id.assetName());
}

void
MultiAsset::set(const Blake2bHash& policyId, const AssetName& name, std::int64_t amount)
{
  validatePolicy(policyId);

  MultiAsset        copy(*this);
  const Blake2bHash policyCopy(policyId);
  const AssetName   nameCopy(name);

  checkCardanoError(cardano_multi_asset_set(copy.nativeHandle(), checkedNativeHandle(policyCopy), checkedNativeHandle(nameCopy), amount), "MultiAsset set");
  swap(copy);
  invalidateElements();
}

void
MultiAsset::set(const AssetId& id, std::int64_t amount)
{
  if (id.isLovelace())
  {
    throw std::invalid_argument("MultiAsset does not contain lovelace");
  }

  set(*id.policyId(), *id.assetName(), amount);
  invalidateElements();
}

PolicyIdList
MultiAsset::policies() const
{
  cardano_policy_id_list_t*                                             list   = nullptr;
  const cardano_error_t                                                 status = cardano_multi_asset_get_keys(checked(), &list);
  CardanoObject<cardano_policy_id_list_t, cardano_policy_id_list_unref> owner(list);

  checkCardanoError(status, "MultiAsset policies");
  PolicyIdList      result;
  const std::size_t elementCount = cardano_policy_id_list_get_length(owner.get());
  result.reserve(elementCount);

  for (std::size_t i = 0; i < elementCount; ++i)
  {
    cardano_blake2b_hash_t*                                           hash      = nullptr;
    const cardano_error_t                                             getResult = cardano_policy_id_list_get(owner.get(), i, &hash);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> hashOwner(hash);

    checkCardanoError(getResult, "MultiAsset policy");
    const Blake2bHash policy = Blake2bHash::adopt(hashOwner.release());
    result.push_back(policy);
  }

  return result;
}

std::vector<std::pair<Blake2bHash, AssetNameMap>>
MultiAsset::entries() const
{
  const PolicyIdList                                keys = policies();
  std::vector<std::pair<Blake2bHash, AssetNameMap>> result;
  result.reserve(keys.size());

  for (std::size_t index = 0; index < keys.size(); ++index)
  {
    const Blake2bHash& key        = keys[index];
    const AssetNameMap quantities = assets(key).value();

    result.emplace_back(key, quantities);
  }

  return result;
}

MultiAsset
MultiAsset::positive() const
{
  MultiAsset                                              result;
  const std::vector<std::pair<Blake2bHash, AssetNameMap>> policyEntries = entries();

  for (const std::pair<Blake2bHash, AssetNameMap>& policy: policyEntries)
  {
    const std::vector<std::pair<AssetName, std::int64_t>> amounts = policy.second.entries();

    for (const std::pair<AssetName, std::int64_t>& asset: amounts)
    {
      if (asset.second > 0)
      {
        result.set(policy.first, asset.first, asset.second);
      }
    }
  }

  return result;
}

MultiAsset
MultiAsset::negative() const
{
  MultiAsset                                              result;
  const std::vector<std::pair<Blake2bHash, AssetNameMap>> policyEntries = entries();

  for (const std::pair<Blake2bHash, AssetNameMap>& policy: policyEntries)
  {
    const std::vector<std::pair<AssetName, std::int64_t>> amounts = policy.second.entries();

    for (const std::pair<AssetName, std::int64_t>& asset: amounts)
    {
      if (asset.second < 0)
      {
        result.set(policy.first, asset.first, asset.second);
      }
    }
  }

  return result;
}

std::vector<std::uint8_t>
MultiAsset::toCbor() const
{
  return encodeCbor(checked(), cardano_multi_asset_to_cbor);
}

std::string
MultiAsset::toJson() const
{
  return encodeJson(checked(), cardano_multi_asset_to_cip116_json);
}

std::string
MultiAsset::toJsonEntries() const
{
  return encodeJson(checked(), cardano_multi_asset_to_cip116_json_ex);
}

void
MultiAsset::swap(MultiAsset& other) noexcept
{
  std::swap(m_value, other.m_value);
  m_elements.swap(other.m_elements);
  m_encoding.swap(other.m_encoding);
  std::swap(m_materialized, other.m_materialized);
  std::swap(m_mutableAccess, other.m_mutableAccess);
}

MultiAsset&
MultiAsset::operator=(const MultiAsset& other)
{
  if (this != &other)
  {
    MultiAsset copy(other);
    swap(copy);
  }

  return *this;
}

MultiAsset&
MultiAsset::operator=(MultiAsset&& other) noexcept
{
  if (this != &other)
  {
    cardano_multi_asset_unref(&m_value);
    m_value         = std::exchange(other.m_value, nullptr);
    m_elements      = std::move(other.m_elements);
    m_encoding      = std::move(other.m_encoding);
    m_materialized  = std::exchange(other.m_materialized, false);
    m_mutableAccess = std::exchange(other.m_mutableAccess, false);
  }

  return *this;
}

MultiAsset
MultiAsset::operator+(const MultiAsset& other) const
{
  const std::vector<std::pair<Blake2bHash, AssetNameMap>> right = other.entries();

  for (const std::pair<Blake2bHash, AssetNameMap>& policy: right)
  {
    const std::vector<std::pair<AssetName, std::int64_t>> amounts = policy.second.entries();

    for (const std::pair<AssetName, std::int64_t>& amount: amounts)
    {
      const std::int64_t left = get(policy.first, amount.first).value_or(0);
      (void)checkedAddAmount(left, amount.second);
    }
  }

  cardano_multi_asset_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_multi_asset_add(checked(), other.checked(), &value);
  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(value);

  checkCardanoError(status, "MultiAsset add");
  return MultiAsset(owner.release());
}

MultiAsset&
MultiAsset::operator+=(const MultiAsset& other)
{
  MultiAsset result = *this + other;
  swap(result);

  return *this;
}

MultiAsset
MultiAsset::operator-(const MultiAsset& other) const
{
  const std::vector<std::pair<Blake2bHash, AssetNameMap>> right = other.entries();

  for (const std::pair<Blake2bHash, AssetNameMap>& policy: right)
  {
    const std::vector<std::pair<AssetName, std::int64_t>> amounts = policy.second.entries();

    for (const std::pair<AssetName, std::int64_t>& amount: amounts)
    {
      const std::int64_t left = get(policy.first, amount.first).value_or(0);
      (void)checkedSubtractAmount(left, amount.second);
    }
  }

  cardano_multi_asset_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_multi_asset_subtract(checked(), other.checked(), &value);
  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(value);

  checkCardanoError(status, "MultiAsset subtract");
  return MultiAsset(owner.release());
}

MultiAsset&
MultiAsset::operator-=(const MultiAsset& other)
{
  MultiAsset result = *this - other;
  swap(result);

  return *this;
}

bool
MultiAsset::operator==(const MultiAsset& other) const
{
  return cardano_multi_asset_equals(checked(), other.checked());
}

bool
MultiAsset::operator!=(const MultiAsset& other) const
{
  return !(*this == other);
}

MultiAsset::iterator
MultiAsset::begin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.begin();
}

MultiAsset::const_iterator
MultiAsset::begin() const
{
  materialize();

  return m_elements.begin();
}

MultiAsset::iterator
MultiAsset::end()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.end();
}

MultiAsset::const_iterator
MultiAsset::end() const
{
  materialize();

  return m_elements.end();
}

MultiAsset::reverse_iterator
MultiAsset::rbegin()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rbegin();
}

MultiAsset::const_reverse_iterator
MultiAsset::rbegin() const
{
  materialize();

  return m_elements.rbegin();
}

MultiAsset::reverse_iterator
MultiAsset::rend()
{
  materialize();
  m_mutableAccess = true;

  return m_elements.rend();
}

MultiAsset::const_reverse_iterator
MultiAsset::rend() const
{
  materialize();

  return m_elements.rend();
}

MultiAsset::const_iterator
MultiAsset::cbegin() const
{
  materialize();

  return m_elements.cbegin();
}

MultiAsset::const_iterator
MultiAsset::cend() const
{
  materialize();

  return m_elements.cend();
}

MultiAsset::const_reverse_iterator
MultiAsset::crbegin() const
{
  materialize();

  return m_elements.crbegin();
}

MultiAsset::const_reverse_iterator
MultiAsset::crend() const
{
  materialize();

  return m_elements.crend();
}

MultiAsset::iterator
MultiAsset::find(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.find(key);
}

MultiAsset::const_iterator
MultiAsset::find(const key_type& key) const
{
  materialize();

  return m_elements.find(key);
}

MultiAsset::size_type
MultiAsset::count(const key_type& key) const
{
  materialize();

  return m_elements.count(key);
}

bool
MultiAsset::contains(const key_type& key) const
{
  return count(key) != 0;
}

MultiAsset::mapped_type&
MultiAsset::at(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.at(key);
}

const MultiAsset::mapped_type&
MultiAsset::at(const key_type& key) const
{
  materialize();

  return m_elements.at(key);
}

std::pair<MultiAsset::iterator, bool>
MultiAsset::insert(const value_type& value)
{
  materialize();
  const iterator existing = m_elements.find(value.first);

  if (existing != m_elements.end())
  {
    m_mutableAccess = true;
    return { existing, false };
  }

  MultiAsset validation;
  validation.setAssets(value.first, value.second);
  m_mutableAccess = true;

  return m_elements.insert(value);
}

MultiAsset::iterator
MultiAsset::insert(const_iterator position, const value_type& value)
{
  (void)position;

  return insert(value).first;
}

std::pair<MultiAsset::iterator, bool>
MultiAsset::insert_or_assign(const key_type& key, const mapped_type& value)
{
  materialize();
  MultiAsset validation;
  validation.setAssets(key, value);
  m_mutableAccess = true;

  return m_elements.insert_or_assign(key, value);
}

MultiAsset::mapped_type&
MultiAsset::operator[](const key_type& key)
{
  return try_emplace(key).first->second;
}

MultiAsset::iterator
MultiAsset::erase(const_iterator position)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(position);
}

MultiAsset::iterator
MultiAsset::erase(const_iterator first, const_iterator last)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(first, last);
}

MultiAsset::size_type
MultiAsset::erase(const key_type& key)
{
  materialize();
  m_mutableAccess = true;

  return m_elements.erase(key);
}

void
MultiAsset::clear()
{
  materialize();
  m_elements.clear();
  m_mutableAccess = true;
}

MultiAsset::size_type
MultiAsset::size() const
{
  return policyCount();
}

bool
MultiAsset::empty() const
{
  return size() == 0;
}

MultiAsset
MultiAsset::adopt(cardano_multi_asset_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null MultiAsset");
  }

  return MultiAsset(value);
}

cardano_multi_asset_t*
MultiAsset::nativeHandle()
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

const cardano_multi_asset_t*
MultiAsset::nativeHandle() const
{
  if (m_value)
  {
    synchronize();
  }

  return m_value;
}

cardano_multi_asset_t*
MultiAsset::release()
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

MultiAsset::MultiAsset(cardano_multi_asset_t* value) noexcept
: m_value(value)
{
}

cardano_multi_asset_t*
MultiAsset::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from MultiAsset");
  }

  synchronize();

  return m_value;
}

void
MultiAsset::materialize() const
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
MultiAsset::synchronize() const
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

  MultiAsset replacement;

  for (const value_type& entry: m_elements)
  {
    replacement.setAssets(entry.first, entry.second);
  }

  std::swap(m_value, replacement.m_value);
  m_encoding.swap(encoding);
}

void
MultiAsset::invalidateElements()
{
  m_elements.clear();
  m_encoding.clear();
  m_materialized  = false;
  m_mutableAccess = false;
}

std::vector<std::vector<std::uint8_t>>
MultiAsset::encodeElements() const
{
  std::vector<std::vector<std::uint8_t>> encoding;
  encoding.reserve(m_elements.size() * 2);

  for (const value_type& entry: m_elements)
  {
    encoding.push_back(entry.first.toCbor());
    encoding.push_back(entry.second.toCbor());
  }

  return encoding;
}

bool
MultiAsset::compareKeys(const key_type& left, const key_type& right)
{
  return left < right;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MultiAsset& lhs, MultiAsset& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
