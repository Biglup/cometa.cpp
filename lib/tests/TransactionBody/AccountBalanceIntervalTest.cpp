/**
 * @file AccountBalanceIntervalTest.cpp
 *
 * @brief Unit tests for Dijkstra account balance requirements.
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

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(AccountBalanceInterval, ExactBalanceMatchesNativeCbor)
{
  // Arrange
  const AccountBalanceInterval interval = AccountBalanceInterval::exact(1000);
  CborReader                   reader("1903e8");

  // Act
  const AccountBalanceInterval decoded = AccountBalanceInterval::fromCbor(interval.toCbor());

  // Assert
  EXPECT_EQ(interval.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, interval);
  EXPECT_TRUE(interval.isExact());
  EXPECT_EQ(interval.exactBalance(), 1000U);
  EXPECT_FALSE(interval.inclusiveLowerBound().has_value());
  EXPECT_FALSE(interval.exclusiveUpperBound().has_value());
  EXPECT_FALSE(interval.toJson().empty());
}

TEST(AccountBalanceInterval, RangeMatchesNativeCbor)
{
  // Arrange
  const AccountBalanceInterval interval(100, 5000);
  CborReader                   reader("821864191388");

  // Act
  const AccountBalanceInterval decoded = AccountBalanceInterval::fromCbor(interval.toCbor());

  // Assert
  EXPECT_EQ(interval.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, interval);
  EXPECT_FALSE(interval.isExact());
  EXPECT_EQ(interval.inclusiveLowerBound(), 100U);
  EXPECT_EQ(interval.exclusiveUpperBound(), 5000U);
  EXPECT_FALSE(interval.exactBalance().has_value());
}

TEST(AccountBalanceInterval, OpenEndedRangesAndInvalidEmptyRange)
{
  // Arrange
  const AccountBalanceInterval lower(500, std::nullopt);
  const AccountBalanceInterval upper(std::nullopt, 10000);

  // Act
  const AccountBalanceInterval copied(lower);

  // Assert
  EXPECT_EQ(lower, copied);
  EXPECT_EQ(AccountBalanceInterval::fromCbor("821901f4f6"), lower);
  EXPECT_EQ(AccountBalanceInterval::fromCbor("82f6192710"), upper);
  EXPECT_THROW(AccountBalanceInterval(std::nullopt, std::nullopt), Error);
}
