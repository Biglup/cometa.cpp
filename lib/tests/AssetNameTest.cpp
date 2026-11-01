/**
 * @file AssetNameTest.cpp
 *
 * @brief Unit tests for AssetName.
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

#include <Cometa/AssetName.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(AssetName, PreservesBinaryNames)
{
  // Arrange
  const auto binary = AssetName::fromHex("610062ff");

  // Act
  const auto bytes   = binary.toBytes();
  const auto text    = binary.toString();
  const auto decoded = AssetName::fromCbor(binary.toCbor());

  // Assert
  EXPECT_EQ(bytes, (std::vector<std::uint8_t> { 0x61, 0, 0x62, 0xff }));
  EXPECT_EQ(text.size(), 4);
  EXPECT_EQ(decoded, binary);
}

TEST(AssetName, EncodesTextAndEmptyNames)
{
  // Act
  const auto text      = AssetName::fromString("Cometa");
  const auto emptyHex  = AssetName::fromHex("");
  const auto emptyText = AssetName::fromString("");

  // Assert
  EXPECT_EQ(text.toHex(), "436f6d657461");
  EXPECT_EQ(emptyHex.toCbor(), (std::vector<std::uint8_t> { 0x40 }));
  EXPECT_TRUE(emptyText.toBytes().empty());
}

TEST(AssetName, EnforcesMaximumLength)
{
  // Arrange
  const std::vector<std::uint8_t> valid(32, 0);
  const std::vector<std::uint8_t> oversized(33, 0);

  // Act & Assert
  EXPECT_NO_THROW(AssetName { valid });
  EXPECT_THROW(AssetName { oversized }, Error);
}

TEST(AssetName, RejectsMalformedCbor)
{
  // Arrange
  const std::vector<std::uint8_t> cbor { 0x41 };

  // Act & Assert
  EXPECT_THROW(AssetName::fromCbor(cbor), Error);
}

TEST(AssetName, ReturnedBytesAreIndependent)
{
  // Arrange
  const auto value = AssetName::fromString("test");
  auto       bytes = value.toBytes();

  // Act
  bytes[0] = 0;

  // Assert
  EXPECT_EQ(bytes[0], 0);
  EXPECT_EQ(value.toString(), "test");
}

TEST(AssetName, SerializesToJson)
{
  // Arrange
  const auto value = AssetName::fromString("Cometa");

  // Act
  const auto json = value.toJson();

  // Assert
  EXPECT_EQ(json, "\"436f6d657461\"");
}

TEST(AssetName, ParsesHexCbor)
{
  // Arrange
  const std::string hex      = "46436f6d657461";
  const auto        expected = AssetName::fromString("Cometa");

  // Act
  const auto decoded = AssetName::fromCbor(hex);

  // Assert
  EXPECT_EQ(decoded, expected);
}

TEST(AssetName, RejectsInvalidHexCbor)
{
  // Arrange
  const std::string trailing = "46436f6d65746100";

  // Act & Assert
  EXPECT_THROW(AssetName::fromCbor("0"), std::invalid_argument);
  EXPECT_THROW(AssetName::fromCbor("gg"), std::invalid_argument);
  EXPECT_THROW(AssetName::fromCbor("0x40"), std::invalid_argument);
  EXPECT_THROW(AssetName::fromCbor(trailing), std::invalid_argument);
  EXPECT_THROW(AssetName::fromCbor("01"), Error);
  EXPECT_THROW(AssetName::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(AssetName::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(AssetName, ParsesMixedCaseHexCbor)
{
  // Arrange
  const std::string      wrapped = "xx42aAbByy";
  const std::string_view hex(wrapped.data() + 2, 6);

  // Act
  const auto value = AssetName::fromCbor(hex);

  // Assert
  EXPECT_EQ(value.toHex(), "aabb");
}

TEST(AssetName, ByteConstructorCopiesInput)
{
  // Arrange
  std::vector<std::uint8_t> bytes { 0x61, 0x62 };

  // Act
  const AssetName value(bytes);
  bytes[0] = 0;

  // Assert
  EXPECT_EQ(value.toString(), "ab");
}

TEST(AssetName, ComparesValues)
{
  // Arrange
  const auto first  = AssetName::fromString("a");
  const auto same   = AssetName::fromHex("61");
  const auto second = AssetName::fromString("b");

  // Act & Assert
  EXPECT_EQ(first, same);
  EXPECT_NE(first, second);
  EXPECT_LT(first, second);
  EXPECT_FALSE(second < first);
}

TEST(AssetName, RejectsInvalidHex)
{
  // Act & Assert
  EXPECT_THROW(AssetName::fromHex("a"), std::invalid_argument);
  EXPECT_THROW(AssetName::fromHex("zz"), std::invalid_argument);
}

TEST(AssetName, StringFactoryPreservesEmbeddedNulls)
{
  // Arrange
  const std::string text("a\0b", 3);

  // Act
  const auto value = AssetName::fromString(text);

  // Assert
  EXPECT_EQ(value.toString(), text);
}
