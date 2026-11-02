/**
 * @file WithdrawalMapTest.cpp
 *
 * @brief Unit tests for WithdrawalMap.
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

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(WithdrawalMap, StoresFullUnsignedAmountsAndRetainsZero)
{
  // Arrange
  const RewardAddress key = RewardAddress::fromBech32("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s");
  WithdrawalMap       map;
  EXPECT_TRUE(map.empty());
  EXPECT_FALSE(map.get(key));
  EXPECT_THROW(map.at(key), std::out_of_range);

  // Act
  map.set(key, std::numeric_limits<std::uint64_t>::max());
  const WithdrawalMap copy(map);
  map.set(key.toString(), 0);

  // Assert
  EXPECT_EQ(map.size(), 1U);
  EXPECT_EQ(map.at(key), 0U);
  EXPECT_EQ(copy.at(key), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(copy.entryAt(0).first, key);
  EXPECT_EQ(copy.entryAt(0).second, std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(copy.keys().at(0), key);
  EXPECT_EQ(copy.entries().at(0).second, std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(WithdrawalMap::fromCbor(copy.toCbor()), copy);
  EXPECT_EQ(JsonValue::parse(copy.toJson()).at(0).at("value").asString(), "18446744073709551615");
  EXPECT_THROW(map.entryAt(1), std::out_of_range);
}

TEST(WithdrawalMap, RejectsNegativeAmountsAndDuplicateAddresses)
{
  // Arrange
  const std::string key = "581de0" + std::string(56, '1');

  // Act & Assert
  EXPECT_THROW(WithdrawalMap::fromCbor("a1" + key + "20"), Error);
  EXPECT_THROW(WithdrawalMap::fromCbor("a2" + key + "00" + key + "01"), std::invalid_argument);
}

TEST(WithdrawalMap, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const WithdrawalMap       value = WithdrawalMap();
  std::vector<std::uint8_t> bytes = value.toCbor();
  EXPECT_EQ(WithdrawalMap::fromCbor(bytes), value);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(WithdrawalMap::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(WithdrawalMap::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(WithdrawalMap::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(WithdrawalMap::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(WithdrawalMap::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(WithdrawalMap::fromCbor("f5"), Error);
}
