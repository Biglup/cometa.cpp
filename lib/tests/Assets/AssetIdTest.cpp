/**
 * @file AssetIdTest.cpp
 *
 * @brief Unit tests for AssetId.
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

#include <Cometa/Assets/AssetId.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <map>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(AssetId, CombinesPolicyAndBinaryName)
{
  // Arrange
  const Blake2bHash policy = Blake2bHash::fromHex(std::string(56, 'a'));
  const AssetName   name   = AssetName::fromHex("6100ff");

  // Act
  const AssetId id(policy, name);

  // Assert
  EXPECT_FALSE(id.isLovelace());
  EXPECT_EQ(id.toHex(), std::string(56, 'a') + "6100ff");
  EXPECT_EQ(id.policyId(), policy);
  EXPECT_EQ(id.assetName(), name);
  EXPECT_EQ(AssetId(id.toBytes()), id);
  EXPECT_EQ(AssetId::fromHex(id.toHex()), id);
}

TEST(AssetId, LovelaceHasEmptyRepresentation)
{
  // Act
  const AssetId id = AssetId::lovelace();

  // Assert
  EXPECT_TRUE(id.isLovelace());
  EXPECT_FALSE(id.policyId().has_value());
  EXPECT_FALSE(id.assetName().has_value());
  EXPECT_TRUE(id.toBytes().empty());
  EXPECT_TRUE(id.toHex().empty());
  EXPECT_EQ(AssetId::fromHex(""), id);
  EXPECT_EQ(AssetId(std::vector<std::uint8_t> {}), id);
}

TEST(AssetId, EmptyNameIsNotLovelace)
{
  // Act
  const AssetId id = AssetId::fromHex(std::string(56, '0'));

  // Assert
  EXPECT_FALSE(id.isLovelace());
  ASSERT_TRUE(id.assetName().has_value());
  EXPECT_TRUE(id.assetName()->toBytes().empty());
  EXPECT_NE(id, AssetId::lovelace());
}

TEST(AssetId, RejectsInvalidLengthsAndHex)
{
  // Arrange
  const AssetName name = AssetName::fromString("test");

  // Act & Assert
  EXPECT_THROW((void)AssetId(Blake2bHash(std::vector<std::uint8_t>(32, 1)), name), Error);
  EXPECT_THROW((void)AssetId(std::vector<std::uint8_t>(27, 0)), Error);
  EXPECT_THROW((void)AssetId(std::vector<std::uint8_t>(61, 0)), Error);
  EXPECT_NO_THROW((void)AssetId(std::vector<std::uint8_t>(60, 0)));
  EXPECT_THROW(AssetId::fromHex("abc"), std::invalid_argument);
  EXPECT_THROW(AssetId::fromHex("zz"), std::invalid_argument);
  EXPECT_THROW(AssetId::fromHex("0x00"), std::invalid_argument);
  EXPECT_THROW(AssetId::fromHex(std::string("aa\0a", 4)), std::invalid_argument);
}

TEST(AssetId, ParsesBoundedMixedCaseHex)
{
  // Arrange
  const std::string text = "xx" + std::string(56, 'A') + "aB" + "yy";

  // Act
  const AssetId id = AssetId::fromHex(std::string_view(text).substr(2, 58));

  // Assert
  EXPECT_EQ(id.toHex(), std::string(56, 'a') + "ab");
}

TEST(AssetId, CopiesComponentsAndReturnedValues)
{
  // Arrange
  Blake2bHash                policy(std::vector<std::uint8_t>(28, 1));
  AssetName                  name = AssetName::fromString("first");
  const AssetId              id(policy, name);
  std::optional<Blake2bHash> returnedPolicy = id.policyId();
  std::optional<AssetName>   returnedName   = id.assetName();
  std::vector<std::uint8_t>  bytes          = id.toBytes();

  // Act
  policy         = Blake2bHash(std::vector<std::uint8_t>(28, 2));
  name           = AssetName::fromString("second");
  returnedPolicy = policy;
  returnedName   = name;
  bytes[0]       = 3;

  // Assert
  EXPECT_EQ(id.policyId()->toBytes(), std::vector<std::uint8_t>(28, 1));
  EXPECT_EQ(id.assetName()->toString(), "first");
  EXPECT_NE(id.policyId(), returnedPolicy);
  EXPECT_NE(id.assetName(), returnedName);
  EXPECT_NE(id.toBytes(), bytes);
}

TEST(AssetId, OrdersIdentifiersForAssociativeContainers)
{
  // Arrange
  const AssetId          first  = AssetId::fromHex(std::string(56, '0') + "01");
  const AssetId          second = AssetId::fromHex(std::string(56, '0') + "0100");
  std::map<AssetId, int> quantities;

  // Act
  quantities.emplace(second, 2);
  quantities.emplace(first, 1);
  quantities.emplace(AssetId::lovelace(), 3);

  // Assert
  EXPECT_LT(first, second);
  EXPECT_FALSE(first < first);
  EXPECT_EQ(quantities.begin()->first, AssetId::lovelace());
  EXPECT_EQ(quantities.at(AssetId(first.toBytes())), 1);
}

TEST(AssetId, CopiesMovesAndSwaps)
{
  // Arrange
  const AssetId source = AssetId::fromHex(std::string(56, '1'));
  AssetId       copy(source);

  // Act
  AssetId moved(std::move(copy));
  copy = AssetId::lovelace();
  swap(copy, moved);
  AssetId& alias = copy;
  copy           = alias;
  copy           = std::move(alias);

  // Assert
  EXPECT_EQ(copy, source);
  EXPECT_EQ(moved, AssetId::lovelace());
  EXPECT_EQ(source.toHex(), std::string(56, '1'));
}
