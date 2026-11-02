/**
 * @file ValueTest.cpp
 *
 * @brief Unit tests for Value.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Value, ConstructsLovelaceAndChecksArithmetic)
{
  // Arrange
  Value value(5);

  // Act
  value.addCoin(3);
  value.subtractCoin(2);
  value.setCoin(7);

  // Assert
  EXPECT_EQ(value.coin(), 7);
  EXPECT_EQ(value.assetCount(), 1U);
  EXPECT_FALSE(value.isZero());
  EXPECT_TRUE(Value().isZero());
  EXPECT_EQ(value.toCbor(), (std::vector<std::uint8_t> { 7 }));
  EXPECT_EQ(value.toJson(), "{\"coin\":\"7\"}");
}

TEST(Value, RejectsLovelaceOverflowWithoutMutation)
{
  // Arrange
  Value maximum(std::numeric_limits<std::int64_t>::max());
  Value minimum(std::numeric_limits<std::int64_t>::min());

  // Act & Assert
  EXPECT_THROW(maximum.addCoin(1), std::overflow_error);
  EXPECT_THROW(maximum.subtractCoin(-1), std::overflow_error);
  EXPECT_THROW(minimum.addCoin(-1), std::overflow_error);
  EXPECT_THROW(minimum.subtractCoin(1), std::overflow_error);
  EXPECT_THROW(maximum += Value(1), std::overflow_error);
  EXPECT_THROW(minimum -= Value(1), std::overflow_error);
  EXPECT_EQ(maximum.coin(), std::numeric_limits<std::int64_t>::max());
  EXPECT_EQ(minimum.coin(), std::numeric_limits<std::int64_t>::min());
}

TEST(Value, CopiesComponentsAndReturnedAssets)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    assets;
  assets.set(id, 1);
  Value value(10, assets);

  // Act
  assets.set(id, 2);
  MultiAsset returned = value.assets();
  returned.set(id, 3);
  Value copy(value);
  copy.addAsset(id, 4);

  // Assert
  EXPECT_EQ(value.assets().get(id), 1);
  EXPECT_EQ(assets.get(id), 2);
  EXPECT_EQ(returned.get(id), 3);
  EXPECT_EQ(copy.assets().get(id), 5);
}

TEST(Value, SupportsAllAssetAdditionOverloads)
{
  // Arrange
  const std::string policy(56, '1');
  const AssetName   name = AssetName::fromHex("01");
  const AssetId     id(Blake2bHash::fromHex(policy), name);
  Value             value;

  // Act
  value.addAsset(Blake2bHash::fromHex(policy), name, 1);
  value.addAsset(policy, "01", 2);
  value.addAsset(id, 3);
  value.addAsset(policy + "01", 4);
  value.addAsset("", 5);

  // Assert
  EXPECT_EQ(value.assets().get(id), 10);
  EXPECT_EQ(value.coin(), 5);
  EXPECT_EQ(value.assetCount(), 2U);
}

TEST(Value, PreservesSignedAmountsThroughAssetMaps)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap quantities { { AssetId::lovelace(), -3 }, { id, -5 } };

  // Act
  const Value value = Value::fromAssetMap(quantities);

  // Assert
  EXPECT_EQ(value.coin(), -3);
  EXPECT_EQ(value.toAssetMap(), quantities);
  EXPECT_THROW(value.toCbor(), std::invalid_argument);
  EXPECT_EQ(value.assetCount(), 1U);
}

TEST(Value, AddsSubtractsAndReplacesAssetCollections)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    assets;
  assets.set(id, 3);
  Value       first(5, assets);
  const Value second(2, assets);

  // Act
  first.addAssets(assets);
  first.subtractAssets(assets);
  const Value sum        = first + second;
  const Value difference = first - second;
  first                  += second;
  first                  -= second;
  first.setAssets(MultiAsset());

  // Assert
  EXPECT_EQ(sum.coin(), 7);
  EXPECT_EQ(sum.assets().get(id), 6);
  EXPECT_EQ(difference.coin(), 3);
  EXPECT_FALSE(difference.assets().get(id));
  EXPECT_EQ(first.coin(), 5);
  EXPECT_EQ(first.assets().policyCount(), 0U);
  EXPECT_EQ(second.assets().get(id), 3);
}

TEST(Value, RejectsAssetOverflowWithoutMutation)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         value(7);
  value.addAsset(id, std::numeric_limits<std::int64_t>::max());
  const Value before(value);

  // Act & Assert
  EXPECT_THROW(value.addAsset(id, 1), std::overflow_error);
  EXPECT_EQ(value, before);
}

TEST(Value, FindsSharedTokenKeysAndPositiveLovelace)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         first(1);
  first.addAsset(id, 0);
  Value second(2);
  second.addAsset(id, -3);

  // Act
  const AssetIdList both = first.intersection(second);
  first.setCoin(-1);
  const AssetIdList tokens = first.intersection(second);

  // Assert
  EXPECT_EQ(both, (AssetIdList { AssetId::lovelace(), id }));
  EXPECT_EQ(tokens, (AssetIdList { id }));
  EXPECT_EQ(first.intersectionCount(second), 1U);
}

TEST(Value, ParsesCoinAndTokenCbor)
{
  // Arrange
  const std::string policy(56, '1');
  const std::string cbor = "82182aa1581c" + policy + "a14003";

  // Act
  const Value value = Value::fromCbor(cbor);

  // Assert
  EXPECT_EQ(value.coin(), 42);
  EXPECT_EQ(value.assets().get(AssetId::fromHex(policy)), 3);
  EXPECT_EQ(Value::fromCbor(value.toCbor()), value);
  EXPECT_EQ(Value::fromCbor("182a"), Value(42));
  EXPECT_EQ(Value::fromCbor("1b7fffffffffffffff").coin(), std::numeric_limits<std::int64_t>::max());
}

TEST(Value, RejectsInvalidCborAndLovelaceNarrowing)
{
  // Act & Assert
  EXPECT_THROW(Value::fromCbor("1b8000000000000000"), std::overflow_error);
  EXPECT_THROW(Value::fromCbor("821bffffffffffffffffa0"), std::overflow_error);
  EXPECT_THROW(Value::fromCbor("20"), Error);
  EXPECT_THROW(Value::fromCbor("8100"), std::invalid_argument);
  EXPECT_THROW(Value::fromCbor("8200"), Error);
  EXPECT_THROW(Value::fromCbor("0001"), std::invalid_argument);
  EXPECT_THROW(Value::fromCbor("z0"), std::invalid_argument);
  EXPECT_THROW(Value::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Value::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(Value, SupportsMoveSwapAndAssignment)
{
  // Arrange
  Value first(5);
  Value copy(first);

  // Act
  Value moved(std::move(copy));
  copy = Value(7);
  swap(copy, moved);
  Value& alias = copy;
  copy         = alias;
  copy         = std::move(alias);

  // Assert
  EXPECT_EQ(copy, first);
  EXPECT_NE(copy, moved);
  EXPECT_EQ(moved.coin(), 7);
}

TEST(Value, MatchesSharedNativeSerializationAndInspectionFixture)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/serialization.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("type").asString() != "value")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const Value       value    = Value::fromCbor(item.at("input").at("cbor").asString());
    const std::string expected = item.at("expected").at("cbor").asString();
    CborReader        encoded(expected);

    EXPECT_EQ(value.toCbor(), encoded.readEncodedValue());
    EXPECT_EQ(value.toJson(), item.at("expected").at("inspection").toJson());
    ++checked;
  }

  EXPECT_EQ(checked, 1U);
}
