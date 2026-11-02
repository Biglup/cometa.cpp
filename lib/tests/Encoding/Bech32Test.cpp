/**
 * @file Bech32Test.cpp
 *
 * @brief Tests for Bech32.
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

#include <Cometa/Encoding/Bech32.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Bech32, MatchesEmptyPayloadChecksumVector)
{
  // Act
  const Bech32Data decoded = decodeBech32("A12UEL5L");

  // Assert
  EXPECT_EQ(encodeBech32("a", {}), "a12uel5l");
  EXPECT_EQ(encodeBech32("A", {}), "a12uel5l");
  EXPECT_EQ(decoded.prefix, "a");
  EXPECT_TRUE(decoded.bytes.empty());
}

TEST(Bech32, MatchesGovernanceIdentifierVector)
{
  // Arrange
  std::vector<std::uint8_t> bytes(33, 0);
  bytes.back()               = 17;
  const std::string expected = "gov_action1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqpzklpgpf";

  // Act
  const Bech32Data decoded = decodeBech32(expected);

  // Assert
  EXPECT_EQ(encodeBech32("gov_action", bytes), expected);
  EXPECT_EQ(decoded.prefix, "gov_action");
  EXPECT_EQ(decoded.bytes, bytes);
}

TEST(Bech32, SupportsLongPayloadsAndPrefixesContainingSeparators)
{
  // Arrange
  std::vector<std::uint8_t> bytes;

  for (std::uint32_t i = 0; i < 256; ++i)
  {
    bytes.push_back(static_cast<std::uint8_t>(i));
  }

  // Act
  const std::string encoded = encodeBech32("test1prefix", bytes);
  const Bech32Data  decoded = decodeBech32(encoded);

  // Assert
  EXPECT_GT(encoded.size(), 90);
  EXPECT_EQ(decoded.prefix, "test1prefix");
  EXPECT_EQ(decoded.bytes, bytes);
}

TEST(Bech32, SupportsMaximumPrefixLength)
{
  // Arrange
  const std::string prefix(83, 'a');

  // Act
  const Bech32Data decoded = decodeBech32(encodeBech32(prefix, { 1 }));

  // Assert
  EXPECT_EQ(decoded.prefix, prefix);
  EXPECT_EQ(decoded.bytes, (std::vector<std::uint8_t> { 1 }));
}

TEST(Bech32, ReturnsIndependentValuesAndRespectsStringViewBounds)
{
  // Arrange
  const std::string text     = "a12uel5lignored";
  const Bech32Data  original = decodeBech32(std::string_view(text.data(), 8));
  Bech32Data        copy     = original;

  // Act
  copy.prefix = "changed";
  copy.bytes.push_back(1);

  // Assert
  EXPECT_EQ(original.prefix, "a");
  EXPECT_TRUE(original.bytes.empty());
  EXPECT_EQ(copy.prefix, "changed");
  EXPECT_EQ(copy.bytes, (std::vector<std::uint8_t> { 1 }));
  EXPECT_EQ(encodeBech32(std::string_view("aIgnored", 1), {}), "a12uel5l");
}

TEST(Bech32, RejectsInvalidPrefixesAndText)
{
  // Act & Assert
  EXPECT_THROW(encodeBech32("", {}), std::invalid_argument);
  EXPECT_THROW(encodeBech32(std::string(84, 'a'), {}), std::invalid_argument);
  EXPECT_THROW(encodeBech32(" ", {}), std::invalid_argument);
  EXPECT_THROW(encodeBech32(std::string("a\0", 2), {}), std::invalid_argument);
  EXPECT_THROW(decodeBech32(""), std::invalid_argument);
  EXPECT_THROW(decodeBech32("1qqqqqq"), std::invalid_argument);
  EXPECT_THROW(decodeBech32("a1qqq"), std::invalid_argument);
  EXPECT_THROW(decodeBech32("a12UEL5L"), Error);
  EXPECT_THROW(decodeBech32("a12uel5q"), Error);
  EXPECT_THROW(decodeBech32(std::string("a12uel5l\0", 9)), std::invalid_argument);
}

TEST(Bech32, RejectsChecksumValidPayloadsWithExcessPadding)
{
  // Act & Assert
  EXPECT_THROW(decodeBech32("a1q3g6mn3"), Error);
  EXPECT_THROW(decodeBech32("a1pv7wwwr"), Error);
}
