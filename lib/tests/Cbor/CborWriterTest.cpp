/**
 * @file CborWriterTest.cpp
 *
 * @brief Streaming CBOR tests.
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

#include <Cometa/Cbor/CborReader.h>
#include <Cometa/Cbor/CborWriter.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CborWriterTest, EncodesKnownIntegerBoundaries)
{
  // Arrange
  CborWriter writer;

  // Act
  writer.writeStartArray(4);
  writer.writeUInt(std::numeric_limits<std::uint64_t>::max());
  writer.writeInt(std::numeric_limits<std::int64_t>::min());
  writer.writeNegativeInt(std::numeric_limits<std::uint64_t>::max());
  writer.writeInt(0);

  // Assert
  EXPECT_EQ(writer.toHex(), "841bffffffffffffffff3b7fffffffffffffff3bffffffffffffffff00");
}

TEST(CborWriterTest, EncodesDefiniteAndIndefiniteContainers)
{
  // Arrange
  CborWriter writer;

  // Act
  writer.writeStartMap(std::nullopt);
  writer.writeText("a");
  writer.writeStartArray(std::nullopt);
  writer.writeBytes({ 1, 2 });
  writer.writeText("ab");
  writer.writeEndArray();
  writer.writeText("b");
  writer.writeNull();
  writer.writeEndMap();

  // Assert
  EXPECT_EQ(writer.toHex(), "bf61619f420102626162ff6162f6ff");
}

TEST(CborWriterTest, EncodesEmptyAndEmbeddedNullStrings)
{
  // Arrange
  CborWriter writer;

  // Act
  writer.writeStartArray(3);
  writer.writeBytes({});
  writer.writeText({});
  writer.writeText(std::string_view("a\0b", 3));

  // Assert
  EXPECT_EQ(writer.toHex(), "83406063610062");
}

TEST(CborWriterTest, EncodesBigIntegersAndSemanticTags)
{
  // Arrange
  CborWriter writer;

  // Act
  writer.writeStartArray(5);
  writer.writeBigInt(BigInt::fromString("18446744073709551616"));
  writer.writeBigInt(BigInt::fromString("-18446744073709551617"));
  writer.writeBigInt(BigInt(0));
  writer.writeBigInt(BigInt(-1));
  writer.writeTag(CborTag::Set);
  writer.writeStartArray(0);

  // Assert
  EXPECT_EQ(writer.toHex(), "85c249010000000000000000c349010000000000000000c240c340d9010280");
}

TEST(CborWriterTest, EncodesBooleansUndefinedAndRawValues)
{
  // Arrange
  CborWriter writer;

  // Act
  writer.writeStartArray(5);
  writer.writeBool(false);
  writer.writeBool(true);
  writer.writeUndefined();
  writer.writeNull();
  writer.writeEncoded({ 0x81, 0x01 });
  writer.writeEncoded({});

  // Assert
  EXPECT_EQ(writer.toHex(), "85f4f5f7f68101");
}

TEST(CborWriterTest, RejectsUnsupportedContainerSizesAndTags)
{
  // Arrange
  CborWriter writer;

  // Act & Assert
  EXPECT_THROW(writer.writeStartArray(std::numeric_limits<std::uint64_t>::max()), std::out_of_range);
  EXPECT_THROW(writer.writeStartMap(std::numeric_limits<std::uint64_t>::max()), std::out_of_range);
  EXPECT_THROW(writer.writeTag(static_cast<CborTag>(0xffffffffU)), std::out_of_range);
  EXPECT_TRUE(writer.toBytes().empty());
}

TEST(CborWriterTest, CopiesIndependentlyAndResets)
{
  // Arrange
  CborWriter writer;
  writer.writeStartArray(2);
  writer.writeUInt(1);

  // Act
  CborWriter copy(writer);
  CborWriter assigned;
  assigned = writer;
  writer.writeUInt(2);
  copy.writeUInt(3);
  assigned.reset();

  // Assert
  EXPECT_EQ(writer.toHex(), "820102");
  EXPECT_EQ(copy.toBytes(), (std::vector<std::uint8_t> { 0x82, 1, 3 }));
  EXPECT_TRUE(assigned.toBytes().empty());
  EXPECT_TRUE(assigned.toHex().empty());
}

TEST(CborWriterTest, SupportsMoveSwapAndSelfAssignment)
{
  // Arrange
  CborWriter writer;
  writer.writeUInt(1);
  CborWriter other;
  other.writeUInt(2);
  CborWriter* alias = &writer;

  // Act
  writer = *alias;
  writer = std::move(*alias);
  CborWriter moved(std::move(writer));
  swap(moved, other);
  writer = std::move(moved);

  // Assert
  EXPECT_EQ(writer.toHex(), "02");
  EXPECT_EQ(other.toHex(), "01");
  EXPECT_THROW(moved.toBytes(), std::logic_error);
  EXPECT_THROW(CborWriter copy(moved), std::logic_error);
}
