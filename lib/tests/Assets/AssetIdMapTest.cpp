/**
 * @file AssetIdMapTest.cpp
 *
 * @brief Unit tests for AssetIdMap.
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

#include <Cometa/Assets/AssetIdMap.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <limits>
#include <stdexcept>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(AssetIdMap, InsertsReplacesAndLooksUpAmounts)
{
  // Arrange
  const AssetId key    = AssetId::lovelace();
  const AssetId absent = AssetId::fromHex(std::string(56, '1'));
  AssetIdMap    map;

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

TEST(AssetIdMap, InitializerReplacesDuplicateKeys)
{
  // Arrange
  const AssetId key = AssetId::lovelace();

  // Act
  const AssetIdMap map { { key, 1 }, { key, 2 } };

  // Assert
  EXPECT_EQ(map.size(), 1U);
  EXPECT_EQ(map.at(key), 2);
}

TEST(AssetIdMap, CopiesKeysAndEntriesIndependently)
{
  // Arrange
  const AssetId                                 key = AssetId::lovelace();
  const AssetIdMap                              map { { key, 7 } };
  AssetIdList                                   keys    = map.keys();
  std::vector<std::pair<AssetId, std::int64_t>> entries = map.entries();

  // Act
  keys.front()           = AssetId::fromHex(std::string(56, '1'));
  entries.front().second = 9;

  // Assert
  EXPECT_EQ(map.at(key), 7);
  EXPECT_NE(keys.front(), map.keys().front());
  EXPECT_NE(entries.front().second, map.entries().front().second);
}

TEST(AssetIdMap, PerformsSignedArithmeticAndRemovesZeroResults)
{
  // Arrange
  const AssetId    first  = AssetId::lovelace();
  const AssetId    second = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap left { { first, 4 }, { second, -3 } };
  const AssetIdMap right { { first, -4 }, { second, 2 } };

  // Act
  AssetIdMap       sum        = left + right;
  const AssetIdMap difference = left - right;
  sum                         += right;
  sum                         -= right;

  // Assert
  EXPECT_FALSE(sum.get(first));
  EXPECT_EQ(sum.at(second), -1);
  EXPECT_EQ(difference.at(first), 8);
  EXPECT_EQ(difference.at(second), -5);
  EXPECT_EQ(left.at(first), 4);
  EXPECT_TRUE((left - left).empty());
}

TEST(AssetIdMap, RejectsAllSignedOverflowDirections)
{
  // Arrange
  const AssetId    key = AssetId::lovelace();
  const AssetIdMap maximum { { key, std::numeric_limits<std::int64_t>::max() } };
  const AssetIdMap minimum { { key, std::numeric_limits<std::int64_t>::min() } };
  const AssetIdMap one { { key, 1 } };
  const AssetIdMap negativeOne { { key, -1 } };
  const AssetIdMap empty;
  AssetIdMap       unchanged(maximum);

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

TEST(AssetIdMap, CopiesMovesAndSwapsIndependentMaps)
{
  // Arrange
  const AssetId    key = AssetId::lovelace();
  const AssetIdMap source { { key, 1 } };
  AssetIdMap       copy(source);

  // Act
  copy.set(key, 2);
  AssetIdMap moved(std::move(copy));
  copy = source;
  swap(copy, moved);
  AssetIdMap& alias = copy;
  copy              = alias;
  copy              = std::move(alias);

  // Assert
  EXPECT_EQ(source.at(key), 1);
  EXPECT_EQ(moved, source);
  EXPECT_EQ(copy.at(key), 2);
  EXPECT_NE(copy, source);
}
