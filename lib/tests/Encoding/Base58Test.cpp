/**
 * @file Base58Test.cpp
 *
 * @brief Tests for Base58.
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

#include <Cometa/Encoding/Base58.h>
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

TEST(Base58, MatchesKnownVectors)
{
  // Act & Assert
  EXPECT_EQ(encodeBase58({}), "");
  EXPECT_EQ(decodeBase58(""), std::vector<std::uint8_t>());
  EXPECT_EQ(encodeBase58({ 0, 0, 0 }), "111");
  EXPECT_EQ(decodeBase58("111"), (std::vector<std::uint8_t> { 0, 0, 0 }));
  EXPECT_EQ(encodeBase58({ 58 }), "21");
  EXPECT_EQ(decodeBase58("21"), (std::vector<std::uint8_t> { 58 }));
  EXPECT_EQ(encodeBase58({ 0, 0x0d, 0x24 }), "1211");
  EXPECT_EQ(decodeBase58("1211"), (std::vector<std::uint8_t> { 0, 0x0d, 0x24 }));
  EXPECT_EQ(encodeBase58({ 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd' }), "JxF12TrwUP45BMd");
  EXPECT_EQ(decodeBase58("JxF12TrwUP45BMd"), (std::vector<std::uint8_t> { 'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd' }));
}

TEST(Base58, RoundTripsEveryByte)
{
  // Act & Assert
  for (std::uint32_t value = 0; value < 256; ++value)
  {
    const std::vector<std::uint8_t> bytes { 0, static_cast<std::uint8_t>(value), 0 };

    EXPECT_EQ(decodeBase58(encodeBase58(bytes)), bytes);
  }
}

TEST(Base58, RespectsStringViewBounds)
{
  // Arrange
  const std::string text = "21ignored";

  // Act
  const std::vector<std::uint8_t> bytes = decodeBase58(std::string_view(text.data(), 2));

  // Assert
  EXPECT_EQ(bytes, (std::vector<std::uint8_t> { 58 }));
}

TEST(Base58, RejectsInvalidCharacters)
{
  // Act & Assert
  EXPECT_THROW(decodeBase58("0"), std::invalid_argument);
  EXPECT_THROW(decodeBase58("O"), std::invalid_argument);
  EXPECT_THROW(decodeBase58("I"), std::invalid_argument);
  EXPECT_THROW(decodeBase58("l"), std::invalid_argument);
  EXPECT_THROW(decodeBase58(" 2"), std::invalid_argument);
  EXPECT_THROW(decodeBase58("2\n"), std::invalid_argument);
  EXPECT_THROW(decodeBase58(std::string("2\0", 2)), std::invalid_argument);
}
