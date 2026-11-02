/**
 * @file Ipv6Test.cpp
 *
 * @brief Unit tests for Ipv6.
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

TEST(Ipv6, MatchesKnownBytesTextAndCbor)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  CborReader                      encoded("5000000000000000000000000000000001");

  // Act
  const Ipv6 address(bytes);

  // Assert
  EXPECT_EQ(address.toBytes(), bytes);
  EXPECT_EQ(address.toString(), "0000:0000:0000:0000:0000:0000:0000:0001");
  EXPECT_EQ(Ipv6::fromString("0000:0000:0000:0000:0000:0000:0000:0001"), address);
  EXPECT_EQ(Ipv6::fromCbor("5000000000000000000000000000000001"), address);
  EXPECT_EQ(Ipv6::fromCbor(address.toCbor()), address);
  EXPECT_EQ(address.toCbor(), encoded.readEncodedValue());
}

TEST(Ipv6, CopiesBytesAndSupportsMoveSwapAndAssignment)
{
  // Arrange
  std::vector<std::uint8_t> bytes { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
  const Ipv6                original(bytes);
  Ipv6                      copy(original);

  // Act
  bytes.at(0)                        = 42;
  std::vector<std::uint8_t> returned = copy.toBytes();
  returned.at(0)                     = 33;
  Ipv6 other(bytes);
  swap(copy, other);
  copy = std::move(other);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_NE(copy.toBytes(), returned);
  EXPECT_NE(Ipv6(bytes), original);
}

TEST(Ipv6, RejectsInvalidSizesTextAndCbor)
{
  // Act & Assert
  EXPECT_THROW((void)Ipv6(std::vector<std::uint8_t> {}), Error);
  EXPECT_THROW((void)Ipv6(std::vector<std::uint8_t>(15U)), Error);
  EXPECT_THROW((void)Ipv6(std::vector<std::uint8_t>(17U)), Error);
  EXPECT_THROW(Ipv6::fromString(""), Error);
  EXPECT_THROW(Ipv6::fromString("bad"), Error);
  EXPECT_THROW(Ipv6::fromString(std::string("0000:0000:0000:0000:0000:0000:0000:0001") + "x"), Error);
  EXPECT_THROW(Ipv6::fromCbor("40"), Error);
  EXPECT_THROW(Ipv6::fromCbor("00"), Error);
  EXPECT_THROW(Ipv6::fromCbor("500000000000000000000000000000000100"), std::invalid_argument);
  EXPECT_THROW(Ipv6::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(Ipv6::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(Ipv6, ComparesHexadecimalTextCaseByAddressBytes)
{
  // Arrange
  const Ipv6 upper = Ipv6::fromString("ABCD:0000:0000:0000:0000:0000:0000:FFFF");

  // Act
  const Ipv6 lower = Ipv6::fromString("abcd:0000:0000:0000:0000:0000:0000:ffff");

  // Assert
  EXPECT_EQ(upper, lower);
  EXPECT_EQ(upper.toCbor(), lower.toCbor());
  EXPECT_THROW(Ipv6::fromString("::1"), Error);
  EXPECT_THROW(Ipv6::fromString("zzzz:0000:0000:0000:0000:0000:0000:0001"), Error);
}
