/**
 * @file JsonWriterTest.cpp
 *
 * @brief JSON value and writer tests.
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

#include <Cometa/Error.h>
#include <Cometa/Json/JsonValue.h>
#include <Cometa/Json/JsonWriter.h>

#include <gtest/gtest.h>

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(JsonWriterTest, BuildsNestedDocumentWithAllValueKinds)
{
  // Arrange
  JsonWriter writer;

  // Act
  writer.writeStartObject();
  writer.writePropertyName("items");
  writer.writeStartArray();
  writer.writeUInt(std::numeric_limits<std::uint64_t>::max());
  writer.writeInt(std::numeric_limits<std::int64_t>::min());
  writer.writeDouble(1.5);
  writer.writeBool(true);
  writer.writeNull();
  writer.writeString(std::string_view("a\0b", 3));
  writer.writeBigInt(BigInt::fromString("18446744073709551616"));
  writer.writeEndArray();
  writer.writePropertyName("raw");
  writer.writeRaw("[1, 2]");
  writer.writePropertyName("value");
  writer.writeValue(JsonValue::parse("{\"x\":1.2300}"));
  writer.writeEndObject();

  // Assert
  EXPECT_EQ(writer.context(), JsonContext::Root);
  EXPECT_EQ(writer.toString(), "{\"items\":[18446744073709551615,-9223372036854775808,1.5,true,null,\"a\\u0000b\",18446744073709551616],\"raw\":[1, 2],\"value\":{\"x\":1.2300}}");
}

TEST(JsonWriterTest, WritesNumericAndBinaryStrings)
{
  // Arrange
  JsonWriter writer;

  // Act
  writer.writeStartArray();
  writer.writeBigIntAsString(BigInt(7));
  writer.writeUIntAsString(42);
  writer.writeIntAsString(-42);
  writer.writeDoubleAsString(1.5);
  writer.writeBytesAsHex({ 0, 255 });
  writer.writeBytesAsHex({});
  writer.writeBytesAsBech32("a", {});
  writer.writeEndArray();

  // Assert
  EXPECT_EQ(writer.toString(), "[\"7\",\"42\",\"-42\",\"1.5\",\"00ff\",\"\",\"a12uel5l\"]");
}

TEST(JsonWriterTest, CopiesPartialContextAndRetainsIndependentBuffers)
{
  // Arrange
  JsonWriter writer;
  writer.writeStartObject();
  writer.writePropertyName("a");

  // Act
  JsonWriter copy(writer);
  JsonWriter assigned;
  assigned = writer;
  writer.writeUInt(1);
  writer.writeEndObject();
  copy.writeUInt(2);
  copy.writeEndObject();
  assigned.writeUInt(3);
  assigned.writeEndObject();

  // Assert
  EXPECT_EQ(writer.toString(), "{\"a\":1}");
  EXPECT_EQ(copy.toString(), "{\"a\":2}");
  EXPECT_EQ(assigned.toString(), "{\"a\":3}");
}

TEST(JsonWriterTest, SupportsMovesSwapSelfAssignmentAndPrettyReset)
{
  // Arrange
  JsonWriter writer(JsonFormat::Pretty);
  writer.writeStartArray();
  JsonWriter* alias = &writer;

  // Act
  writer = *alias;
  writer = std::move(*alias);
  JsonWriter moved(std::move(writer));
  JsonWriter other;
  swap(moved, other);
  writer = std::move(other);
  writer.reset();
  writer.writeStartArray();
  writer.writeUInt(42);
  writer.writeEndArray();

  // Assert
  EXPECT_NE(writer.toString().find('\n'), std::string::npos);
  EXPECT_THROW(other.toString(), std::logic_error);
  EXPECT_THROW(JsonWriter invalid(other), std::logic_error);
  EXPECT_THROW(moved.toString(), std::logic_error);
}

TEST(JsonWriterTest, ReportsIncompleteDocumentsAndInvalidWriteOrder)
{
  // Arrange
  JsonWriter writer;

  // Act & Assert
  EXPECT_THROW(writer.toString(), std::logic_error);
  writer.writeStartArray();
  EXPECT_EQ(writer.context(), JsonContext::Array);
  EXPECT_THROW(writer.toString(), std::logic_error);
  EXPECT_THROW(writer.writeEndObject(), Error);
  EXPECT_THROW(writer.toString(), Error);
  writer.reset();
  writer.writeNull();
  EXPECT_EQ(writer.toString(), "null");
  EXPECT_THROW(writer.writeNull(), Error);
}

TEST(JsonWriterTest, RejectsInvalidInputsBeforeWriting)
{
  // Arrange
  JsonWriter writer;

  // Act & Assert
  EXPECT_THROW(JsonWriter(static_cast<JsonFormat>(42)), std::invalid_argument);
  EXPECT_THROW(writer.writeDouble(std::numeric_limits<double>::infinity()), std::invalid_argument);
  EXPECT_THROW(writer.writeDoubleAsString(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
  EXPECT_THROW(writer.writeRaw("[1,]"), Error);
  EXPECT_THROW(writer.writeBytesAsBech32("", { 1 }), std::invalid_argument);
  writer.writeString("");
  EXPECT_EQ(writer.toString(), "\"\"");
}

TEST(JsonWriter, NativeTransferPreservesPrettyFormattingAfterReset)
{
  // Arrange
  JsonWriter writer(JsonFormat::Pretty);
  writer.writeStartObject();
  writer.writePropertyName("value");
  writer.writeUInt(42);
  writer.writeEndObject();
  const std::string      expected = writer.toString();
  const JsonWriter&      view     = writer;
  cardano_json_writer_t* pointer  = writer.nativeHandle();

  // Act
  JsonWriter adopted = JsonWriter::adopt(writer.release(), JsonFormat::Pretty);

  // Assert
  EXPECT_EQ(adopted.nativeHandle(), pointer);
  EXPECT_THROW(JsonWriter::adopt(pointer, static_cast<JsonFormat>(-1)), std::invalid_argument);
  EXPECT_EQ(writer.nativeHandle(), nullptr);
  EXPECT_EQ(view.nativeHandle(), nullptr);
  EXPECT_EQ(writer.release(), nullptr);
  EXPECT_EQ(adopted.toString(), expected);
  EXPECT_THROW(JsonWriter::adopt(nullptr, JsonFormat::Pretty), std::invalid_argument);

  // Act
  adopted.reset();
  adopted.writeStartObject();
  adopted.writePropertyName("value");
  adopted.writeUInt(42);
  adopted.writeEndObject();

  // Assert
  EXPECT_EQ(adopted.toString(), expected);
}
