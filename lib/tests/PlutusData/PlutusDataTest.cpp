/**
 * @file PlutusDataTest.cpp
 *
 * @brief Unit tests for PlutusData.
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

#include <fstream>
#include <iterator>
#include <limits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(PlutusData, DecodesBytesAndHexWithoutTrailingData)
{
  // Arrange
  const PlutusData                expected = PlutusData(42);
  CborReader                      reader("182a");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const PlutusData fromHex   = PlutusData::fromCbor("182a");
  const PlutusData fromBytes = PlutusData::fromCbor(bytes);

  // Assert
  EXPECT_EQ(fromHex, expected);
  EXPECT_EQ(fromBytes, expected);
  EXPECT_EQ(fromHex.toCbor(), bytes);
  EXPECT_EQ(fromBytes.toJson(), expected.toJson());
}

TEST(PlutusData, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(PlutusData::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(PlutusData::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusData::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(PlutusData::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(PlutusData::fromCbor("182a00"), std::invalid_argument);
  EXPECT_THROW(PlutusData::fromCbor("f5"), Error);
}

TEST(PlutusData, SupportsCopyMoveAssignmentAndSwap)
{
  // Arrange
  const PlutusData original = PlutusData(42);
  PlutusData       copy(original);

  // Act
  PlutusData moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  PlutusData& alias = copy;
  copy              = alias;
  copy              = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(PlutusData, HandlesIntegerAndByteBoundaries)
{
  // Arrange
  const BigInt large = BigInt::fromString("-18446744073709551617");

  // Act
  const PlutusData integer(large);
  const PlutusData bytes = PlutusData::fromHex("00ff");

  // Assert
  EXPECT_EQ(integer.kind(), PlutusDataKind::Integer);
  EXPECT_EQ(integer.asInteger(), large);
  EXPECT_EQ(PlutusData::fromCbor(integer.toCbor()), integer);
  EXPECT_EQ(PlutusData::fromUnsigned(std::numeric_limits<std::uint64_t>::max()).asInteger(), BigInt::fromString("18446744073709551615"));
  EXPECT_EQ(PlutusData::fromCbor("3bffffffffffffffff").asInteger(), BigInt::fromString("-18446744073709551616"));
  EXPECT_EQ(bytes.kind(), PlutusDataKind::Bytes);
  EXPECT_EQ(bytes.asBytes(), (std::vector<std::uint8_t> { 0, 255 }));
  EXPECT_TRUE(PlutusData::fromHex("").asBytes().empty());
  EXPECT_EQ(PlutusData(std::vector<std::uint8_t>(130, 1)).asBytes().size(), 130U);
}

TEST(PlutusData, RejectsWrongVariantsAndOversizedTags)
{
  // Arrange
  const PlutusData integer(1);

  // Act & Assert
  EXPECT_THROW(integer.asBytes(), Error);
  EXPECT_THROW(integer.asList(), Error);
  EXPECT_THROW(integer.asMap(), Error);
  EXPECT_THROW(integer.asConstructor(), Error);
  EXPECT_THROW(PlutusData::fromHex("x0"), std::invalid_argument);
  EXPECT_THROW(PlutusData::fromCbor("db000000010000007980"), Error);
  EXPECT_THROW(PlutusData::fromCbor("81db00000001000000024101"), Error);
}

TEST(PlutusData, PreservesOriginalEncodingUntilNormalization)
{
  // Arrange
  PlutusData       data = PlutusData::fromCbor("1817");
  const PlutusData copy(data);

  // Act
  data.normalizeEncoding();

  // Assert
  EXPECT_EQ(data.asInteger(), BigInt(23));
  EXPECT_EQ(data.toCbor(), (std::vector<std::uint8_t> { 0x17 }));
  EXPECT_EQ(copy.toCbor(), (std::vector<std::uint8_t> { 0x18, 0x17 }));
  EXPECT_EQ(data, copy);
}

TEST(PlutusData, ReturnsIndependentCompositeValues)
{
  // Arrange
  PlutusList list;
  list.pushBack(PlutusData(1));
  const PlutusData data(list);

  // Act
  list.pushBack(PlutusData(2));
  PlutusList extracted = data.asList();
  extracted.pushBack(PlutusData(3));

  // Assert
  EXPECT_EQ(data.asList().size(), 1U);
  EXPECT_EQ(data.kind(), PlutusDataKind::List);
  EXPECT_EQ(list.size(), 2U);
  EXPECT_EQ(extracted.size(), 2U);
}

TEST(PlutusData, MatchesSharedNativeSerializationAndInspectionFixtures)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/serialization.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0U;

  // Act & Assert
  for (std::size_t index = 0U; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);
    if (item.at("type").asString() != "plutus_data")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const PlutusData value = PlutusData::fromCbor(item.at("input").at("cbor").asString());
    CborReader       encoded(item.at("expected").at("cbor").asString());

    EXPECT_EQ(value.toCbor(), encoded.readEncodedValue());
    EXPECT_EQ(value.toJson(), item.at("expected").at("inspection").toJson());
    ++checked;
  }

  EXPECT_EQ(checked, 13U);
}

TEST(PlutusData, ImplicitListConversionCopiesMutableElements)
{
  // Arrange
  PlutusList  list { PlutusData(1) };
  PlutusData& element = list.front();

  // Act
  const PlutusData converted = list;
  element                    = PlutusData(2);

  // Assert
  EXPECT_EQ(converted.asList().front(), PlutusData(1));
  EXPECT_EQ(list.front(), PlutusData(2));
}

TEST(PlutusData, MovingAListIncludesIteratorEdits)
{
  // Arrange
  PlutusList list { PlutusData(1) };
  list.front() = PlutusData(2);

  // Act
  const PlutusData converted = std::move(list);

  // Assert
  EXPECT_EQ(converted.asList().front(), PlutusData(2));
  EXPECT_THROW(list.toCbor(), std::logic_error);
}
