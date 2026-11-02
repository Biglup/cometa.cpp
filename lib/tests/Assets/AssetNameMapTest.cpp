/**
 * @file AssetNameMapTest.cpp
 *
 * @brief Unit tests for AssetNameMap.
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

#include <Cometa/Assets/AssetNameMap.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(AssetNameMap, InsertsReplacesAndLooksUpAmounts)
{
  // Arrange
  const AssetName key    = AssetName::fromHex("00");
  const AssetName absent = AssetName::fromHex("0001");
  AssetNameMap    map;

  // Act
  map.set(key, -3);
  map.set(key, 0);

  // Assert
  EXPECT_FALSE(map.empty());
  EXPECT_EQ(map.size(), 1U);
  EXPECT_EQ(map.at(key), 0);
  EXPECT_EQ(map.get(key), 0);
  EXPECT_FALSE(map.get(absent));
  EXPECT_THROW(map.at(absent), std::out_of_range);
}

TEST(AssetNameMap, InitializerReplacesDuplicateKeys)
{
  // Arrange
  const AssetName key = AssetName::fromHex("00");

  // Act
  const AssetNameMap map { { key, 1 }, { key, 2 } };

  // Assert
  EXPECT_EQ(map.size(), 1U);
  EXPECT_EQ(map.at(key), 2);
}

TEST(AssetNameMap, CopiesKeysAndEntriesIndependently)
{
  // Arrange
  const AssetName                                 key = AssetName::fromHex("00");
  const AssetNameMap                              map { { key, 7 } };
  AssetNameList                                   keys    = map.keys();
  std::vector<std::pair<AssetName, std::int64_t>> entries = map.entries();

  // Act
  keys.front()           = AssetName::fromHex("0001");
  entries.front().second = 9;

  // Assert
  EXPECT_EQ(map.at(key), 7);
  EXPECT_NE(keys.front(), map.keys().front());
  EXPECT_NE(entries.front().second, map.entries().front().second);
}

TEST(AssetNameMap, PerformsSignedArithmeticAndRemovesZeroResults)
{
  // Arrange
  const AssetName    first  = AssetName::fromHex("00");
  const AssetName    second = AssetName::fromHex("0001");
  const AssetNameMap left { { first, 4 }, { second, -3 } };
  const AssetNameMap right { { first, -4 }, { second, 2 } };

  // Act
  AssetNameMap       sum        = left + right;
  const AssetNameMap difference = left - right;
  sum                           += right;
  sum                           -= right;

  // Assert
  EXPECT_FALSE(sum.get(first));
  EXPECT_EQ(sum.at(second), -1);
  EXPECT_EQ(difference.at(first), 8);
  EXPECT_EQ(difference.at(second), -5);
  EXPECT_EQ(left.at(first), 4);
  EXPECT_TRUE((left - left).empty());
}

TEST(AssetNameMap, RejectsAllSignedOverflowDirections)
{
  // Arrange
  const AssetName    key = AssetName::fromHex("00");
  const AssetNameMap maximum { { key, std::numeric_limits<std::int64_t>::max() } };
  const AssetNameMap minimum { { key, std::numeric_limits<std::int64_t>::min() } };
  const AssetNameMap one { { key, 1 } };
  const AssetNameMap negativeOne { { key, -1 } };
  const AssetNameMap empty;
  AssetNameMap       unchanged(maximum);

  // Act & Assert
  EXPECT_THROW(maximum + one, std::overflow_error);
  EXPECT_THROW(minimum + negativeOne, std::overflow_error);
  EXPECT_THROW(maximum - negativeOne, std::overflow_error);
  EXPECT_THROW(minimum - one, std::overflow_error);
  EXPECT_THROW(empty - minimum, std::overflow_error);
  EXPECT_THROW(unchanged += one, std::overflow_error);
  EXPECT_EQ(unchanged, maximum);
  EXPECT_EQ((minimum - minimum).size(), 0U);
  EXPECT_EQ((minimum + maximum).at(key), -1);
}

TEST(AssetNameMap, CopiesMovesAndSwapsIndependentMaps)
{
  // Arrange
  const AssetName    key = AssetName::fromHex("00");
  const AssetNameMap source { { key, 1 } };
  AssetNameMap       copy(source);

  // Act
  copy.set(key, 2);
  AssetNameMap moved(std::move(copy));
  copy = source;
  swap(copy, moved);
  AssetNameMap& alias = copy;
  copy                = alias;
  copy                = std::move(alias);

  // Assert
  EXPECT_EQ(source.at(key), 1);
  EXPECT_EQ(moved, source);
  EXPECT_EQ(copy.at(key), 2);
  EXPECT_NE(copy, source);
}

TEST(AssetNameMap, EncodesKnownCborAndBothJsonForms)
{
  // Arrange
  const AssetNameMap map { { AssetName::fromHex("00"), -1 }, { AssetName::fromHex(""), 2 } };

  // Act
  const std::vector<std::uint8_t> bytes = map.toCbor();

  // Assert
  EXPECT_EQ(bytes, (std::vector<std::uint8_t> { 0xa2, 0x40, 2, 0x41, 0, 0x20 }));
  EXPECT_EQ(AssetNameMap::fromCbor(bytes), map);
  EXPECT_EQ(AssetNameMap::fromCbor("a24002410020"), map);
  EXPECT_EQ(map.toJson(), "{\"\":\"2\",\"00\":\"-1\"}");
  EXPECT_EQ(map.toJsonEntries(), "[{\"asset_name\":\"\",\"amount\":\"2\"},{\"asset_name\":\"00\",\"amount\":\"-1\"}]");
}

TEST(AssetNameMap, RejectsInvalidAndDuplicateCbor)
{
  // Act & Assert
  EXPECT_THROW(AssetNameMap::fromCbor("a1401b8000000000000000"), Error);
  EXPECT_THROW(AssetNameMap::fromCbor("a1403b8000000000000000"), Error);
  EXPECT_THROW(AssetNameMap::fromCbor("a240014002"), std::invalid_argument);
  EXPECT_THROW(AssetNameMap::fromCbor("a0ff"), std::invalid_argument);
  EXPECT_THROW(AssetNameMap::fromCbor("a140"), Error);
  EXPECT_THROW(AssetNameMap::fromCbor("80"), Error);
  EXPECT_THROW(AssetNameMap::fromCbor("a"), std::invalid_argument);
  EXPECT_THROW(AssetNameMap::fromCbor("gg"), std::invalid_argument);
  EXPECT_THROW(AssetNameMap::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(AssetNameMap::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(AssetNameMap, AcceptsSignedLimitsAndIndefiniteMaps)
{
  // Act
  const AssetNameMap limits = AssetNameMap::fromCbor("bf401b7fffffffffffffff41003b7fffffffffffffffff");

  // Assert
  EXPECT_EQ(limits.at(AssetName::fromHex("")), std::numeric_limits<std::int64_t>::max());
  EXPECT_EQ(limits.at(AssetName::fromHex("00")), std::numeric_limits<std::int64_t>::min());
  EXPECT_EQ(AssetNameMap::fromCbor(limits.toCbor()), limits);
}
