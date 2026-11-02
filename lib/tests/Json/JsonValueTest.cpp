/**
 * @file JsonValueTest.cpp
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

#include <gtest/gtest.h>

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(JsonValueTest, ReadsNestedValuesAndPropertyOrder)
{
  // Arrange
  const JsonValue value = JsonValue::parse(R"({"name":"Cometa","items":[42,true,null,{"x":1.5}]})");

  // Act & Assert
  EXPECT_EQ(value.type(), JsonType::Object);
  EXPECT_EQ(value.size(), 2U);
  EXPECT_EQ(value.keyAt(0), "name");
  EXPECT_EQ(value.valueAt(0).asString(), "Cometa");
  EXPECT_TRUE(value.contains("items"));
  EXPECT_FALSE(value.contains("missing"));
  EXPECT_EQ(value.at("items").size(), 4U);
  EXPECT_EQ(value.at("items").at(0).asUInt(), 42U);
  EXPECT_TRUE(value.at("items").at(1).asBool());
  EXPECT_EQ(value.at("items").at(2).type(), JsonType::Null);
  EXPECT_DOUBLE_EQ(value.at("items").at(3).at("x").asDouble(), 1.5);
}

TEST(JsonValueTest, PreservesExactNumberTextAndChecksConversions)
{
  // Arrange
  const JsonValue large   = JsonValue::parse("18446744073709551616");
  const JsonValue real    = JsonValue::parse("-1.2300e+4");
  const JsonValue maximum = JsonValue::parse("18446744073709551615");
  const JsonValue minimum = JsonValue::parse("-9223372036854775808");

  // Act & Assert
  EXPECT_EQ(large.toJson(), "18446744073709551616");
  EXPECT_THROW(large.asUInt(), Error);
  EXPECT_EQ(real.toJson(), "-1.2300e+4");
  EXPECT_TRUE(real.isNegative());
  EXPECT_TRUE(real.isReal());
  EXPECT_FALSE(maximum.isReal());
  EXPECT_FALSE(maximum.isNegative());
  EXPECT_EQ(maximum.asUInt(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(minimum.asInt(), std::numeric_limits<std::int64_t>::min());
  EXPECT_THROW(maximum.asInt(), Error);
  EXPECT_THROW(minimum.asUInt(), Error);
  EXPECT_THROW(real.asInt(), Error);
  EXPECT_DOUBLE_EQ(real.asDouble(), -12300.0);
  EXPECT_EQ(JsonValue::parse("\"42\"").asUInt(), 42U);
  EXPECT_EQ(JsonValue::parse("\"-42\"").asInt(), -42);
  EXPECT_DOUBLE_EQ(JsonValue::parse("\"1.5\"").asDouble(), 1.5);
}

TEST(JsonValueTest, PreservesUnicodeEmbeddedNullsAndEmptyKeys)
{
  // Arrange
  const JsonValue value = JsonValue::parse(R"({"":"","a\u0000b":"a\u0000b","emoji":"\ud83d\ude00"})");

  // Act & Assert
  EXPECT_TRUE(value.contains(""));
  EXPECT_TRUE(value.at("").asString().empty());
  EXPECT_EQ(value.keyAt(1), std::string("a\0b", 3));
  EXPECT_EQ(value.at(std::string_view("a\0b", 3)).asString(), std::string("a\0b", 3));
  EXPECT_EQ(value.at("emoji").asString(), "\xf0\x9f\x98\x80");
}

TEST(JsonValueTest, RespectsStringViewBoundsAndFormattingRequests)
{
  // Arrange
  const std::string text  = "{\"a\":1}trailing";
  const JsonValue   value = JsonValue::parse(std::string_view(text.data(), 7));

  // Act
  const std::string pretty  = value.toJson(JsonFormat::Pretty);
  const std::string compact = value.toJson();

  // Assert
  EXPECT_NE(pretty.find('\n'), std::string::npos);
  EXPECT_EQ(compact, "{\"a\":1}");
}

TEST(JsonValueTest, ChildValuesSurviveParentReplacement)
{
  // Arrange
  JsonValue       parent = JsonValue::parse("{\"a\":[1,2]}");
  const JsonValue child  = parent.at("a");

  // Act
  parent = JsonValue::parse("null");

  // Assert
  EXPECT_EQ(child.toJson(), "[1,2]");
  EXPECT_EQ(parent.type(), JsonType::Null);
}

TEST(JsonValueTest, SupportsCopiesMovesAndSwap)
{
  // Arrange
  JsonValue  value = JsonValue::parse("{\"a\":1.2300}");
  JsonValue  copy(value);
  JsonValue  assigned = JsonValue::parse("null");
  JsonValue* alias    = &value;

  // Act
  assigned = value;
  value    = *alias;
  value    = std::move(*alias);
  JsonValue moved(std::move(value));
  swap(moved, assigned);
  value = std::move(moved);
  copy  = JsonValue::parse("42");

  // Assert
  EXPECT_EQ(value.toJson(), "{\"a\":1.2300}");
  EXPECT_EQ(assigned.toJson(), value.toJson());
  EXPECT_EQ(copy.asInt(), 42);
  EXPECT_THROW(moved.type(), std::logic_error);
  EXPECT_THROW(JsonValue invalid(moved), std::logic_error);
}

TEST(JsonValueTest, RejectsWrongTypesMissingChildrenAndMalformedInput)
{
  // Arrange
  const JsonValue object = JsonValue::parse("{}");
  const JsonValue array  = JsonValue::parse("[]");
  const JsonValue number = JsonValue::parse("1");

  // Act & Assert
  EXPECT_THROW(object.at("missing"), std::out_of_range);
  EXPECT_THROW(object.keyAt(0), std::out_of_range);
  EXPECT_THROW(object.valueAt(0), std::out_of_range);
  EXPECT_THROW(array.at(0), std::out_of_range);
  EXPECT_THROW(number.size(), std::invalid_argument);
  EXPECT_THROW(number.asString(), std::invalid_argument);
  EXPECT_THROW(number.asBool(), Error);
  EXPECT_THROW(array.contains("a"), std::invalid_argument);
  EXPECT_THROW(object.at(0), std::invalid_argument);
  EXPECT_THROW(array.at("a"), std::invalid_argument);
  EXPECT_THROW(array.isReal(), std::invalid_argument);
  EXPECT_THROW(JsonValue::parse(""), Error);
  EXPECT_THROW(JsonValue::parse("01"), Error);
  EXPECT_THROW(JsonValue::parse("1e"), Error);
  EXPECT_THROW(JsonValue::parse("[1,]"), Error);
  EXPECT_THROW(JsonValue::parse("true false"), Error);
  EXPECT_THROW(JsonValue::parse("\"unfinished"), Error);
}
