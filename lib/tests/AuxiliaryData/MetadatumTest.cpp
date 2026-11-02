/**
 * @file MetadatumTest.cpp
 *
 * @brief Unit tests for Metadatum.
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

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Metadatum, RepresentsEveryScalarKind)
{
  // Arrange
  const Metadatum integer(std::int64_t { -42 });
  const Metadatum unsignedInteger = Metadatum::fromUnsigned(UINT64_MAX);
  const Metadatum bytes           = Metadatum::fromHex("001122");
  const Metadatum text(std::string("a\0b", 3));

  // Act & Assert
  EXPECT_EQ(integer.kind(), MetadatumKind::Integer);
  EXPECT_EQ(integer.asInteger(), BigInt(-42));
  EXPECT_EQ(unsignedInteger.asInteger().toString(), "18446744073709551615");
  EXPECT_EQ(bytes.kind(), MetadatumKind::Bytes);
  EXPECT_EQ(bytes.asBytes(), (std::vector<std::uint8_t> { 0, 17, 34 }));
  EXPECT_EQ(text.kind(), MetadatumKind::Text);
  EXPECT_EQ(text.asText(), std::string("a\0b", 3));
  EXPECT_EQ(Metadatum("").asText(), "");
  EXPECT_TRUE(Metadatum::fromHex("").asBytes().empty());
  EXPECT_THROW(integer.asText(), Error);
  EXPECT_THROW(integer.asBytes(), Error);
  EXPECT_THROW(text.asInteger(), Error);
  EXPECT_THROW(text.asList(), Error);
  EXPECT_THROW(text.asMap(), Error);
  EXPECT_THROW(Metadatum::fromHex("q0"), std::invalid_argument);
}

TEST(Metadatum, MatchesKnownCborAndInspection)
{
  // Arrange
  const Metadatum value = Metadatum::fromPlainJson(R"({"name":"Cometa","count":42})");
  CborReader      expected("a2646e616d6566436f6d65746165636f756e74182a");

  // Act & Assert
  EXPECT_EQ(value.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(Metadatum::fromCbor(value.toCbor()), value);
  EXPECT_EQ(Metadatum::fromCbor("3829"), Metadatum(std::int64_t { -42 }));
  EXPECT_EQ(Metadatum(std::int64_t { 42 }).toJson(), R"({"tag":"int","value":"42"})");
  EXPECT_EQ(Metadatum("hello").toJson(), R"({"tag":"string","value":"hello"})");
  EXPECT_EQ(Metadatum::fromHex("00ff").toJson(), R"({"tag":"bytes","value":"00ff"})");
}

TEST(Metadatum, PreservesExactPlainJsonValues)
{
  // Arrange
  const std::string json  = R"({"":[],"a\u0000b":[18446744073709551616,-18446744073709551616,"x\u0000y"]})";
  const Metadatum   value = Metadatum::fromPlainJson(json);

  // Act & Assert
  EXPECT_EQ(value.toPlainJson(), json);
  EXPECT_EQ(Metadatum::fromCbor(value.toCbor()), value);
  EXPECT_EQ(Metadatum::fromPlainJson("-0").toPlainJson(), "0");
}

TEST(Metadatum, RejectsUnsupportedPlainJsonKinds)
{
  // Act & Assert
  EXPECT_THROW(Metadatum::fromPlainJson("true"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromPlainJson("null"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromPlainJson("1.5"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromPlainJson("1e2"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromPlainJson("[false]"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromPlainJson("{"), Error);
  EXPECT_THROW(Metadatum::fromHex("00").toPlainJson(), std::invalid_argument);

  // Arrange
  MetadatumMap map;
  map.insert(Metadatum(std::int64_t { 1 }), Metadatum("value"));

  // Act & Assert
  EXPECT_THROW(Metadatum(map).toPlainJson(), std::invalid_argument);
}

TEST(Metadatum, KeepsLongValuesButEnforcesCborStringLimits)
{
  // Arrange
  const Metadatum text(std::string(65, 'a'));
  const Metadatum bytes(std::vector<std::uint8_t>(65, 1));
  const Metadatum validText(std::string(64, 'a'));
  const Metadatum validBytes(std::vector<std::uint8_t>(64, 1));

  // Act & Assert
  EXPECT_EQ(text.asText().size(), 65U);
  EXPECT_EQ(bytes.asBytes().size(), 65U);
  EXPECT_THROW(text.toCbor(), Error);
  EXPECT_THROW(bytes.toCbor(), Error);
  EXPECT_EQ(Metadatum::fromCbor(validText.toCbor()), validText);
  EXPECT_EQ(Metadatum::fromCbor(validBytes.toCbor()), validBytes);
}

TEST(Metadatum, OwnsIndependentNestedValues)
{
  // Arrange
  MetadatumList   list { Metadatum(std::int64_t { 1 }) };
  const Metadatum value(list);
  BigInt          integer(42);
  const Metadatum number(integer);

  // Act
  list.pushBack(Metadatum(std::int64_t { 2 }));
  MetadatumList returned = value.asList();
  returned.pushBack(Metadatum(std::int64_t { 3 }));
  integer                += BigInt(1);
  BigInt returnedInteger = number.asInteger();
  returnedInteger        += BigInt(2);

  // Assert
  EXPECT_EQ(value.asList().size(), 1U);
  EXPECT_EQ(list.size(), 2U);
  EXPECT_EQ(returned.size(), 2U);
  EXPECT_EQ(number.asInteger(), BigInt(42));
  EXPECT_EQ(integer, BigInt(43));
  EXPECT_EQ(returnedInteger, BigInt(44));
}

TEST(Metadatum, RejectsMalformedCbor)
{
  // Act & Assert
  EXPECT_THROW(Metadatum::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromCbor("zz"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromCbor("0000"), std::invalid_argument);
  EXPECT_THROW(Metadatum::fromCbor("f5"), Error);
  EXPECT_THROW(Metadatum::fromCbor("c000"), Error);
  EXPECT_THROW(Metadatum::fromCbor("62ff"), Error);
}
