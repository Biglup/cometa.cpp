/**
 * @file Value.cpp
 *
 * @brief Lovelace and native-token value operations.
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

#include <Cometa/TransactionBody/Value.h>

#include <Cometa/Cbor/CborReader.h>

#include "../Assets/AssetArithmetic.h"
#include "../CardanoInterop.h"

#include <cardano/transaction_body/value.h>

#include <limits>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Value::Value(std::int64_t coin)
: m_value(nullptr)
{
  cardano_value_t*                                    value  = nullptr;
  const cardano_error_t                               status = cardano_value_new(coin, nullptr, &value);
  CardanoObject<cardano_value_t, cardano_value_unref> owner(value);

  checkCardanoError(status, "Value");
  m_value = owner.release();
}

Value::Value(std::int64_t coin, const MultiAsset& assets)
: m_value(nullptr)
{
  const MultiAsset                                    copy(assets);
  cardano_value_t*                                    value  = nullptr;
  const cardano_error_t                               status = cardano_value_new(coin, copy.checked(), &value);
  CardanoObject<cardano_value_t, cardano_value_unref> owner(value);

  checkCardanoError(status, "Value with assets");
  m_value = owner.release();
}

Value::Value(const Value& other)
: m_value(nullptr)
{
  cardano_value_t*                                    value  = nullptr;
  const cardano_error_t                               status = cardano_value_deep_clone(other.checked(), &value);
  CardanoObject<cardano_value_t, cardano_value_unref> owner(value);

  checkCardanoError(status, "Value copy");
  m_value = owner.release();
}

Value::Value(Value&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Value::~Value()
{
  cardano_value_unref(&m_value);
}

Value
Value::fromAssetMap(const AssetIdMap& assets)
{
  Value                                               result;
  const std::vector<std::pair<AssetId, std::int64_t>> entries = assets.entries();

  for (const std::pair<AssetId, std::int64_t>& entry: entries)
  {
    result.addAsset(entry.first, entry.second);
  }

  return result;
}

Value
Value::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  CborReader reader(cbor);
  const bool hasAssets = reader.peekState() != CborReaderState::UnsignedInteger;

  if (hasAssets)
  {
    const std::optional<std::uint64_t> length = reader.readStartArray();

    if (length && *length != 2)
    {
      throw std::invalid_argument("Value array must contain lovelace and assets");
    }
  }

  const std::uint64_t lovelace = reader.readUInt();

  if (lovelace > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
  {
    throw std::overflow_error("Lovelace exceeds int64_t range");
  }

  Value result(static_cast<std::int64_t>(lovelace));

  if (hasAssets)
  {
    result.setAssets(MultiAsset::fromCbor(reader.readEncodedValue()));
    reader.readEndArray();
  }

  if (reader.bytesRemaining() != 0)
  {
    throw std::invalid_argument("Trailing CBOR bytes");
  }

  return result;
}

Value
Value::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::int64_t
Value::coin() const
{
  return cardano_value_get_coin(checked());
}

void
Value::setCoin(std::int64_t coin)
{
  checkCardanoError(cardano_value_set_coin(checked(), coin), "Value setCoin");
}

void
Value::addCoin(std::int64_t amount)
{
  setCoin(checkedAddAmount(coin(), amount));
}

void
Value::subtractCoin(std::int64_t amount)
{
  setCoin(checkedSubtractAmount(coin(), amount));
}

MultiAsset
Value::assets() const
{
  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> owner(cardano_value_get_multi_asset(checked()));

  if (!owner)
  {
    return MultiAsset();
  }

  const MultiAsset result(owner.release());
  return MultiAsset(result);
}

void
Value::setAssets(const MultiAsset& assets)
{
  const MultiAsset copy(assets);

  checkCardanoError(cardano_value_set_multi_asset(checked(), copy.checked()), "Value setAssets");
}

void
Value::addAssets(const MultiAsset& assets)
{
  setAssets(this->assets() + assets);
}

void
Value::subtractAssets(const MultiAsset& assets)
{
  setAssets(this->assets() - assets);
}

void
Value::addAsset(const Blake2bHash& policyId, const AssetName& name, std::int64_t quantity)
{
  MultiAsset         copy   = assets();
  const std::int64_t amount = checkedAddAmount(copy.get(policyId, name).value_or(0), quantity);

  copy.set(policyId, name, amount);
  setAssets(copy);
}

void
Value::addAsset(std::string_view policyHex, std::string_view nameHex, std::int64_t quantity)
{
  addAsset(Blake2bHash::fromHex(policyHex), AssetName::fromHex(nameHex), quantity);
}

void
Value::addAsset(const AssetId& id, std::int64_t quantity)
{
  if (id.isLovelace())
  {
    addCoin(quantity);
    return;
  }

  addAsset(*id.policyId(), *id.assetName(), quantity);
}

void
Value::addAsset(std::string_view idHex, std::int64_t quantity)
{
  addAsset(AssetId::fromHex(idHex), quantity);
}

AssetIdMap
Value::toAssetMap() const
{
  AssetIdMap result;

  if (coin() != 0)
  {
    result.set(AssetId::lovelace(), coin());
  }

  const std::vector<std::pair<Blake2bHash, AssetNameMap>> policies = assets().entries();

  for (const std::pair<Blake2bHash, AssetNameMap>& policy: policies)
  {
    const std::vector<std::pair<AssetName, std::int64_t>> amounts = policy.second.entries();

    for (const std::pair<AssetName, std::int64_t>& amount: amounts)
    {
      result.set(AssetId(policy.first, amount.first), amount.second);
    }
  }

  return result;
}

AssetIdList
Value::intersection(const Value& other) const
{
  const AssetIdMap  left  = toAssetMap();
  const AssetIdMap  right = other.toAssetMap();
  const AssetIdList keys  = left.keys();
  AssetIdList       result;

  for (std::size_t index = 0; index < keys.size(); ++index)
  {
    const AssetId& key = keys[index];

    if (right.get(key) && (!key.isLovelace() || (coin() > 0 && other.coin() > 0)))
    {
      result.push_back(key);
    }
  }

  return result;
}

std::size_t
Value::intersectionCount(const Value& other) const
{
  return intersection(other).size();
}

std::size_t
Value::assetCount() const
{
  std::size_t                                             count    = coin() > 0 ? 1 : 0;
  const std::vector<std::pair<Blake2bHash, AssetNameMap>> policies = assets().entries();

  for (std::size_t index = 0; index < policies.size(); ++index)
  {
    const AssetNameMap& quantities = policies[index].second;
    count                          += quantities.size();
  }

  return count;
}

bool
Value::isZero() const
{
  return cardano_value_is_zero(checked());
}

std::vector<std::uint8_t>
Value::toCbor() const
{
  if (coin() < 0)
  {
    throw std::invalid_argument("Negative lovelace cannot be encoded as an output value");
  }

  return encodeCbor(checked(), cardano_value_to_cbor);
}

std::string
Value::toJson() const
{
  return encodeJson(checked(), cardano_value_to_cip116_json);
}

void
Value::swap(Value& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Value&
Value::operator=(const Value& other)
{
  if (this != &other)
  {
    Value copy(other);
    swap(copy);
  }

  return *this;
}

Value&
Value::operator=(Value&& other) noexcept
{
  if (this != &other)
  {
    cardano_value_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

Value
Value::operator+(const Value& other) const
{
  const std::int64_t lovelace = checkedAddAmount(this->coin(), other.coin());
  const MultiAsset   tokens   = assets() + other.assets();

  return Value(lovelace, tokens);
}

Value&
Value::operator+=(const Value& other)
{
  Value result = *this + other;
  swap(result);

  return *this;
}

Value
Value::operator-(const Value& other) const
{
  const std::int64_t lovelace = checkedSubtractAmount(this->coin(), other.coin());
  const MultiAsset   tokens   = assets() - other.assets();

  return Value(lovelace, tokens);
}

Value&
Value::operator-=(const Value& other)
{
  Value result = *this - other;
  swap(result);

  return *this;
}

bool
Value::operator==(const Value& other) const
{
  return cardano_value_equals(checked(), other.checked());
}

bool
Value::operator!=(const Value& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Value::Value(cardano_value_t& value) noexcept
: m_value(&value)
{
}

cardano_value_t*
Value::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Value");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Value& lhs, Value& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
