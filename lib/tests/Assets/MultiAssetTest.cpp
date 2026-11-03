/**
 * @file MultiAssetTest.cpp
 *
 * @brief Unit tests for MultiAsset.
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

#include <limits>
#include <stdexcept>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(MultiAsset, SetsAndLooksUpTokenQuantities)
{
  // Arrange
  const Blake2bHash policy = Blake2bHash::fromHex(std::string(56, '1'));
  const AssetName   name   = AssetName::fromHex("00");
  const AssetId     id(policy, name);
  MultiAsset        value;

  // Act
  value.set(policy, name, -3);
  value.set(id, 4);

  // Assert
  EXPECT_EQ(value.policyCount(), 1U);
  EXPECT_EQ(value.get(id), 4);
  EXPECT_EQ(value.get(policy, name), 4);
  EXPECT_EQ(value.assets(policy)->at(name), 4);
  EXPECT_EQ(value.policies(), (PolicyIdList { policy }));
  EXPECT_EQ(value.entries().front().second.at(name), 4);
  EXPECT_FALSE(value.get(policy, AssetName::fromHex("01")));
  EXPECT_FALSE(value.assets(Blake2bHash::fromHex(std::string(56, '2'))));
}

TEST(MultiAsset, CopiesPolicyMapsOnInsertionAndRetrieval)
{
  // Arrange
  const Blake2bHash policy = Blake2bHash::fromHex(std::string(56, '1'));
  const AssetName   name   = AssetName::fromString("test");
  AssetNameMap      map { { name, 1 } };
  MultiAsset        value;

  // Act
  value.setAssets(policy, map);
  map.set(name, 2);
  AssetNameMap returned = *value.assets(policy);
  returned.set(name, 3);
  MultiAsset copy(value);
  copy.set(policy, name, 4);

  // Assert
  EXPECT_EQ(value.get(policy, name), 1);
  EXPECT_EQ(map.at(name), 2);
  EXPECT_EQ(returned.at(name), 3);
  EXPECT_EQ(copy.get(policy, name), 4);
}

TEST(MultiAsset, EncodesKnownCborAndJson)
{
  // Arrange
  const std::string policyHex(56, '1');
  const Blake2bHash policy = Blake2bHash::fromHex(policyHex);
  const std::string cbor   = "a1581c" + policyHex + "a1410020";

  // Act
  const MultiAsset value = MultiAsset::fromCbor(cbor);

  // Assert
  EXPECT_EQ(value.get(policy, AssetName::fromHex("00")), -1);
  EXPECT_EQ(MultiAsset::fromCbor(value.toCbor()), value);
  EXPECT_EQ(value.toJson(), "{\"" + policyHex + "\":{\"00\":\"-1\"}}");
  EXPECT_EQ(value.toJsonEntries(), "[{\"script_hash\":\"" + policyHex + "\",\"assets\":{\"00\":\"-1\"}}]");
}

TEST(MultiAsset, RejectsInvalidPoliciesAndLovelace)
{
  // Arrange
  MultiAsset        value;
  const Blake2bHash invalid(std::vector<std::uint8_t>(32, 0));
  const AssetName   name = AssetName::fromHex("");

  // Act & Assert
  EXPECT_THROW(value.set(invalid, name, 1), std::invalid_argument);
  EXPECT_THROW(value.setAssets(invalid, AssetNameMap()), std::invalid_argument);
  EXPECT_THROW(value.get(invalid, name), std::invalid_argument);
  EXPECT_THROW(value.assets(invalid), std::invalid_argument);
  EXPECT_THROW(value.get(AssetId::lovelace()), std::invalid_argument);
  EXPECT_THROW(value.set(AssetId::lovelace(), 1), std::invalid_argument);
  EXPECT_EQ(value.policyCount(), 0U);
}

TEST(MultiAsset, RejectsDuplicateAndMalformedCbor)
{
  // Arrange
  const std::string policy(56, '1');

  // Act & Assert
  EXPECT_THROW(MultiAsset::fromCbor("a2581c" + policy + "a0581c" + policy + "a0"), std::invalid_argument);
  EXPECT_THROW(MultiAsset::fromCbor("a14100a0"), std::invalid_argument);
  EXPECT_THROW(MultiAsset::fromCbor("a1581c" + policy + "a240014002"), std::invalid_argument);
  EXPECT_THROW(MultiAsset::fromCbor("a1581c" + policy + "a1401b8000000000000000"), Error);
  EXPECT_THROW(MultiAsset::fromCbor("a000"), std::invalid_argument);
  EXPECT_THROW(MultiAsset::fromCbor("a1"), Error);
  EXPECT_THROW(MultiAsset::fromCbor("z0"), std::invalid_argument);
  EXPECT_THROW(MultiAsset::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(MultiAsset::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(MultiAsset, SelectsPositiveAndNegativeAmounts)
{
  // Arrange
  const Blake2bHash policy   = Blake2bHash::fromHex(std::string(56, '1'));
  const AssetName   positive = AssetName::fromHex("01");
  const AssetName   negative = AssetName::fromHex("02");
  MultiAsset        value;
  value.set(policy, positive, 5);
  value.set(policy, negative, -7);
  value.set(policy, AssetName::fromHex("03"), 0);

  // Act
  const MultiAsset positives = value.positive();
  const MultiAsset negatives = value.negative();

  // Assert
  EXPECT_EQ(positives.get(policy, positive), 5);
  EXPECT_EQ(negatives.get(policy, negative), -7);
  EXPECT_EQ(positives.assets(policy)->size(), 1U);
  EXPECT_EQ(negatives.assets(policy)->size(), 1U);
  EXPECT_EQ(value.assets(policy)->size(), 3U);
}

TEST(MultiAsset, ChecksArithmeticAndPreservesInputs)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    first;
  first.set(id, 5);
  MultiAsset second;
  second.set(id, -5);

  // Act
  MultiAsset       sum        = first + second;
  const MultiAsset difference = first - second;
  sum                         += first;
  sum                         -= first;

  // Assert
  EXPECT_FALSE(sum.get(id));
  EXPECT_EQ(difference.get(id), 10);
  EXPECT_EQ(first.get(id), 5);
  EXPECT_EQ(second.get(id), -5);
}

TEST(MultiAsset, RejectsOverflowAndMissingKeyNegation)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    maximum;
  maximum.set(id, std::numeric_limits<std::int64_t>::max());
  MultiAsset one;
  one.set(id, 1);
  MultiAsset minimum;
  minimum.set(id, std::numeric_limits<std::int64_t>::min());

  // Act & Assert
  EXPECT_THROW(maximum += one, std::overflow_error);
  EXPECT_THROW(minimum -= one, std::overflow_error);
  EXPECT_THROW(MultiAsset() - minimum, std::overflow_error);
  EXPECT_EQ(maximum.get(id), std::numeric_limits<std::int64_t>::max());
  EXPECT_EQ(minimum.get(id), std::numeric_limits<std::int64_t>::min());
}

TEST(MultiAsset, SupportsMoveSwapAndAssignment)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    first;
  first.set(id, 5);
  MultiAsset copy(first);

  // Act
  MultiAsset moved(std::move(copy));
  copy = MultiAsset();
  swap(copy, moved);
  MultiAsset& alias = copy;
  copy              = alias;
  copy              = std::move(alias);

  // Assert
  EXPECT_EQ(copy, first);
  EXPECT_NE(copy, moved);
  EXPECT_EQ(moved.policyCount(), 0U);
}

TEST(MultiAsset, NestedMappedEditsReachSerialization)
{
  // Arrange
  const Blake2bHash policy = Blake2bHash::fromHex(std::string(56, '1'));
  const AssetName   name   = AssetName::fromString("Cometa");
  MultiAsset        assets;
  assets.setAssets(policy, AssetNameMap { { name, 1 } });
  AssetNameMap& amounts = assets.begin()->second;

  // Act
  amounts[name] = 2;
  assets.toCbor();
  amounts[name]            = 3;
  const MultiAsset decoded = MultiAsset::fromCbor(assets.toCbor());

  // Assert
  EXPECT_EQ(decoded.get(policy, name), 3);
}
