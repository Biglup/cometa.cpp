/**
 * @file DirectDepositMapTest.cpp
 *
 * @brief Unit tests for Dijkstra reward account direct deposits.
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

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(DirectDepositMap, MatchesNativeDijkstraCbor)
{
  // Arrange
  const std::string_view cbor = "a2581de013cf55d175ea848b87deb3e914febd7e028e2bf6534475d52fb9c3d01903e8581de0404b5a4088ae9abcf486a7e7b8f82069e6fcfe1bf226f1851ce725701907d0";
  CborReader             reader(cbor);

  // Act
  const DirectDepositMap map = DirectDepositMap::fromCbor(cbor);

  // Assert
  EXPECT_EQ(map.size(), 2U);
  EXPECT_EQ(map.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(map.entryAt(0).second, 1000U);
  EXPECT_EQ(map.entryAt(1).second, 2000U);
  EXPECT_EQ(map.keys().size(), 2U);
  EXPECT_EQ(map.entries().size(), 2U);
  EXPECT_FALSE(map.toJson().empty());
}

TEST(DirectDepositMap, SortsEntriesAndRetainsUnsignedAmounts)
{
  // Arrange
  const RewardAddress       first = RewardAddress::fromBech32("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s");
  std::vector<std::uint8_t> bytes = first.toBytes();
  ++bytes[1];
  const RewardAddress second(bytes);
  DirectDepositMap    map;

  // Act
  map.insert(second, std::numeric_limits<std::uint64_t>::max());
  const DirectDepositMap copy(map);
  map.insert(first.toString(), 0);

  // Assert
  EXPECT_EQ(map.size(), 2U);
  EXPECT_EQ(map.entryAt(0).first, first);
  EXPECT_EQ(map.entryAt(1).first, second);
  EXPECT_EQ(map.get(first), 0U);
  EXPECT_EQ(map.get(second), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_FALSE(copy.get(first).has_value());
  EXPECT_EQ(DirectDepositMap::fromCbor(map.toCbor()), map);
  EXPECT_THROW(map.insert(first, 1), Error);
  EXPECT_THROW(map.entryAt(2), std::out_of_range);
}
