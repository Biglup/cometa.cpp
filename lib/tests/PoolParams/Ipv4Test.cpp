/**
 * @file Ipv4Test.cpp
 *
 * @brief Unit tests for Ipv4.
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

TEST(Ipv4, MatchesKnownBytesTextAndCbor)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 127, 0, 0, 1 };
  CborReader                      encoded("447f000001");

  // Act
  const Ipv4 address(bytes);

  // Assert
  EXPECT_EQ(address.toBytes(), bytes);
  EXPECT_EQ(address.toString(), "127.0.0.1");
  EXPECT_EQ(Ipv4::fromString("127.0.0.1"), address);
  EXPECT_EQ(Ipv4::fromCbor("447f000001"), address);
  EXPECT_EQ(Ipv4::fromCbor(address.toCbor()), address);
  EXPECT_EQ(address.toCbor(), encoded.readEncodedValue());
}

TEST(Ipv4, CopiesBytesAndSupportsMoveSwapAndAssignment)
{
  // Arrange
  std::vector<std::uint8_t> bytes { 127, 0, 0, 1 };
  const Ipv4                original(bytes);
  Ipv4                      copy(original);

  // Act
  bytes.at(0)                        = 42;
  std::vector<std::uint8_t> returned = copy.toBytes();
  returned.at(0)                     = 33;
  Ipv4 other(bytes);
  swap(copy, other);
  copy = std::move(other);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_NE(copy.toBytes(), returned);
  EXPECT_NE(Ipv4(bytes), original);
}

TEST(Ipv4, RejectsInvalidSizesTextAndCbor)
{
  // Act & Assert
  EXPECT_THROW((void)Ipv4(std::vector<std::uint8_t> {}), Error);
  EXPECT_THROW((void)Ipv4(std::vector<std::uint8_t>(3U)), Error);
  EXPECT_THROW((void)Ipv4(std::vector<std::uint8_t>(5U)), Error);
  EXPECT_THROW(Ipv4::fromString(""), Error);
  EXPECT_THROW(Ipv4::fromString("bad"), Error);
  EXPECT_THROW(Ipv4::fromString(std::string("127.0.0.1") + "x"), Error);
  EXPECT_THROW(Ipv4::fromCbor("40"), Error);
  EXPECT_THROW(Ipv4::fromCbor("00"), Error);
  EXPECT_THROW(Ipv4::fromCbor("447f00000100"), std::invalid_argument);
  EXPECT_THROW(Ipv4::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(Ipv4::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(Ipv4, ParsesBoundedTextAndRejectsOutOfRangeOctets)
{
  // Arrange
  const std::string text = "255.0.1.2suffix";

  // Act
  const Ipv4 address = Ipv4::fromString(std::string_view(text.data(), 9));

  // Assert
  EXPECT_EQ(address.toBytes(), (std::vector<std::uint8_t> { 255, 0, 1, 2 }));
  EXPECT_THROW(Ipv4::fromString("256.0.1.2"), Error);
  EXPECT_THROW(Ipv4::fromString("+1.0.1.2"), Error);
  EXPECT_THROW(Ipv4::fromString("1.0.1"), Error);
}
