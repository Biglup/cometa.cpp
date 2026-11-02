/**
 * @file CborReaderTest.cpp
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
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CborReaderTest, ReadsSignedAndUnsignedBoundaries)
{
  // Arrange
  CborReader reader("841bffffffffffffffff3b7fffffffffffffff3bffffffffffffffff00");

  // Act & Assert
  EXPECT_EQ(reader.readStartArray(), 4U);
  EXPECT_EQ(reader.readUInt(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(reader.readInt(), std::numeric_limits<std::int64_t>::min());
  EXPECT_EQ(reader.readNegativeInt(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(reader.readInt(), 0);
  EXPECT_EQ(reader.peekState(), CborReaderState::EndArray);
  reader.readEndArray();
  EXPECT_EQ(reader.peekState(), CborReaderState::Finished);
  EXPECT_EQ(reader.bytesRemaining(), 0U);
  EXPECT_TRUE(reader.remainderBytes().empty());
}

TEST(CborReaderTest, ReadsIndefiniteContainersAndStrings)
{
  // Arrange
  CborReader reader("bf61619f5f41014102ff7f61616162ffff6162f6ff");

  // Act & Assert
  EXPECT_EQ(reader.readStartMap(), std::nullopt);
  EXPECT_EQ(reader.readText(), "a");
  EXPECT_EQ(reader.readStartArray(), std::nullopt);
  EXPECT_EQ(reader.readBytes(), (std::vector<std::uint8_t> { 1, 2 }));
  EXPECT_EQ(reader.readText(), "ab");
  reader.readEndArray();
  EXPECT_EQ(reader.readText(), "b");
  reader.readNull();
  reader.readEndMap();
  EXPECT_EQ(reader.peekState(), CborReaderState::Finished);
}

TEST(CborReaderTest, PreservesEmptyStringsAndEmbeddedNulls)
{
  // Arrange
  CborReader reader("83406063610062");

  // Act & Assert
  EXPECT_EQ(reader.readStartArray(), 3U);
  EXPECT_TRUE(reader.readBytes().empty());
  EXPECT_TRUE(reader.readText().empty());
  EXPECT_EQ(reader.readText(), std::string("a\0b", 3));
  reader.readEndArray();
}

TEST(CborReaderTest, ReadsBigIntegersAndTags)
{
  // Arrange
  CborReader reader("83c249010000000000000000c349010000000000000000d9010280");

  // Act & Assert
  EXPECT_EQ(reader.readStartArray(), 3U);
  EXPECT_EQ(reader.peekTag(), CborTag::UnsignedBigNum);
  EXPECT_EQ(reader.readBigInt().toString(), "18446744073709551616");
  EXPECT_EQ(reader.readBigInt().toString(), "-18446744073709551617");
  EXPECT_EQ(reader.readTag(), CborTag::Set);
  EXPECT_EQ(reader.readStartArray(), 0U);
  reader.readEndArray();
  reader.readEndArray();
}

TEST(CborReaderTest, RejectsUnrepresentableTagsWithoutAdvancing)
{
  // Arrange
  CborReader        reader("db0000000100000000f6");
  const std::size_t remaining = reader.bytesRemaining();

  // Act & Assert
  EXPECT_THROW(reader.peekTag(), std::out_of_range);
  EXPECT_THROW(reader.readTag(), std::out_of_range);
  EXPECT_EQ(reader.bytesRemaining(), remaining);
}

TEST(CborReaderTest, ReadsFloatingPointAndSimpleValues)
{
  // Arrange
  CborReader reader("86f93e00fa3fc00000fb3ff8000000000000f4f5f7");

  // Act & Assert
  EXPECT_EQ(reader.readStartArray(), 6U);
  EXPECT_DOUBLE_EQ(reader.readDouble(), 1.5);
  EXPECT_DOUBLE_EQ(reader.readDouble(), 1.5);
  EXPECT_DOUBLE_EQ(reader.readDouble(), 1.5);
  EXPECT_FALSE(reader.readBool());
  EXPECT_TRUE(reader.readBool());
  EXPECT_EQ(reader.readSimpleValue(), CborSimpleValue::Undefined);
  reader.readEndArray();
}

TEST(CborReaderTest, CopiesInputAndReturnsIndependentEncodedValues)
{
  // Arrange
  std::vector<std::uint8_t> bytes { 0x82, 0x81, 0x01, 0x02 };
  CborReader                reader(bytes);
  bytes[2] = 9;

  // Act
  const std::optional<std::uint64_t> size      = reader.readStartArray();
  std::vector<std::uint8_t>          encoded   = reader.readEncodedValue();
  const std::vector<std::uint8_t>    remainder = reader.remainderBytes();
  encoded[1]                                   = 8;
  reader.skipValue();
  reader.readEndArray();

  // Assert
  EXPECT_EQ(size, 2U);
  EXPECT_EQ(encoded, (std::vector<std::uint8_t> { 0x81, 8 }));
  EXPECT_EQ(remainder, (std::vector<std::uint8_t> { 2 }));
  EXPECT_EQ(reader.peekState(), CborReaderState::Finished);
}

TEST(CborReaderTest, CopyPreservesIndependentNestedCursor)
{
  // Arrange
  CborReader reader("8282010203");
  ASSERT_EQ(reader.readStartArray(), 2U);
  ASSERT_EQ(reader.readStartArray(), 2U);
  ASSERT_EQ(reader.readUInt(), 1U);

  // Act
  CborReader copy(reader);
  CborReader assigned("00");
  assigned                 = reader;
  const std::uint64_t next = reader.readUInt();
  reader.readEndArray();
  const std::uint64_t last = reader.readUInt();
  reader.readEndArray();

  // Assert
  EXPECT_EQ(next, 2U);
  EXPECT_EQ(last, 3U);
  EXPECT_EQ(copy.readUInt(), 2U);
  copy.readEndArray();
  EXPECT_EQ(copy.readUInt(), 3U);
  copy.readEndArray();
  EXPECT_EQ(assigned.readUInt(), 2U);
  EXPECT_EQ(reader.peekState(), CborReaderState::Finished);
}

TEST(CborReaderTest, SupportsMoveSwapAndSelfAssignment)
{
  // Arrange
  CborReader  reader("01");
  CborReader  other("02");
  CborReader* alias = &reader;

  // Act
  reader = *alias;
  reader = std::move(*alias);
  CborReader moved(std::move(reader));
  swap(moved, other);
  reader = std::move(moved);

  // Assert
  EXPECT_EQ(reader.readUInt(), 2U);
  EXPECT_EQ(other.readUInt(), 1U);
  EXPECT_THROW(moved.peekState(), std::logic_error);
  EXPECT_THROW(CborReader copy(moved), std::logic_error);
}

TEST(CborReaderTest, RejectsEmptyMalformedAndWrongTypeInput)
{
  // Arrange
  CborReader truncated("1b00");
  CborReader text("6161");
  CborReader oversized("1bffffffffffffffff");
  CborReader incomplete("8201");

  // Act & Assert
  EXPECT_THROW(CborReader(""), std::invalid_argument);
  EXPECT_THROW(CborReader("0"), std::invalid_argument);
  EXPECT_THROW(CborReader("zz"), std::invalid_argument);
  EXPECT_THROW(CborReader(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(truncated.readUInt(), Error);
  EXPECT_THROW(text.readUInt(), Error);
  EXPECT_THROW(oversized.readInt(), std::out_of_range);
  EXPECT_THROW(incomplete.skipValue(), Error);
}

TEST(CborReaderTest, RejectsSignedOverflowInBothDirections)
{
  // Arrange
  CborReader positive("1b8000000000000000");
  CborReader negative("3b8000000000000000");

  // Act & Assert
  EXPECT_THROW(positive.readInt(), std::out_of_range);
  EXPECT_THROW(negative.readInt(), std::out_of_range);
  EXPECT_EQ(positive.bytesRemaining(), 0U);
  EXPECT_EQ(negative.bytesRemaining(), 0U);
}
