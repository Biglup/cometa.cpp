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
  return cardano_multi_asset_get_policy_count(checked());
}

std::optional<AssetNameMap>
MultiAsset::assets(const Blake2bHash& policyId) const
{
  validatePolicy(policyId);

  cardano_asset_name_map_t*                                             value  = nullptr;
  const cardano_error_t                                                 result = cardano_multi_asset_get_assets(checked(), policyId.checked(), &value);
  CardanoObject<cardano_asset_name_map_t, cardano_asset_name_map_unref> owner(value);

  if (result == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(result, "MultiAsset assets");
  const AssetNameMap map(owner.release());

  return map;
}

void
MultiAsset::setAssets(const Blake2bHash& policyId, const AssetNameMap& assets)
{
  validatePolicy(policyId);

  const Blake2bHash  policyCopy(policyId);
  const AssetNameMap assetsCopy(assets);

  checkCardanoError(cardano_multi_asset_insert_assets(checked(), policyCopy.checked(), assetsCopy.checked()), "MultiAsset setAssets");
}

std::optional<std::int64_t>
MultiAsset::get(const Blake2bHash& policyId, const AssetName& name) const
{
  validatePolicy(policyId);

  std::int64_t          amount = 0;
  const cardano_error_t result = cardano_multi_asset_get(checked(), policyId.checked(), name.checked(), &amount);

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

  checkCardanoError(cardano_multi_asset_set(copy.checked(), policyCopy.checked(), nameCopy.checked(), amount), "MultiAsset set");
  swap(copy);
}

void
MultiAsset::set(const AssetId& id, std::int64_t amount)
{
  if (id.isLovelace())
  {
    throw std::invalid_argument("MultiAsset does not contain lovelace");
  }

  set(*id.policyId(), *id.assetName(), amount);
}

PolicyIdList
MultiAsset::policies() const
{
  cardano_policy_id_list_t*                                             list   = nullptr;
  const cardano_error_t                                                 status = cardano_multi_asset_get_keys(checked(), &list);
  CardanoObject<cardano_policy_id_list_t, cardano_policy_id_list_unref> owner(list);

  checkCardanoError(status, "MultiAsset policies");
  PolicyIdList      result;
  const std::size_t count = cardano_policy_id_list_get_length(owner.get());
  result.reserve(count);

  for (std::size_t i = 0; i < count; ++i)
  {
    cardano_blake2b_hash_t*                                           hash      = nullptr;
    const cardano_error_t                                             getResult = cardano_policy_id_list_get(owner.get(), i, &hash);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> hashOwner(hash);

    checkCardanoError(getResult, "MultiAsset policy");
    const Blake2bHash policy(hashOwner.release());
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
    m_value = std::exchange(other.m_value, nullptr);
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

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(MultiAsset& lhs, MultiAsset& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
