/**
 * @file ScriptInvalidAfterTest.cpp
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

#include <limits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ScriptInvalidAfter, MatchesCborAndCliJson)
{
  // Arrange
  const ScriptInvalidAfter        expected = ScriptInvalidAfter(42);
  const std::string               cbor     = "8205182a";
  const std::string               json     = "{\"type\":\"before\",\"slot\":42}";
  CborReader                      reader(cbor);
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ScriptInvalidAfter decoded = ScriptInvalidAfter::fromCbor(cbor);
  const ScriptInvalidAfter parsed  = ScriptInvalidAfter::fromJson(json);

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(ScriptInvalidAfter::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(ScriptInvalidAfter, RejectsInvalidInputAndWrongVariants)
{
  // Arrange
  const std::string cbor = "8205182a";

  // Act & Assert
  EXPECT_THROW(ScriptInvalidAfter::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidAfter::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidAfter::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidAfter::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidAfter::fromCbor(cbor + "00"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidAfter::fromCbor("00"), Error);
  EXPECT_THROW(ScriptInvalidAfter::fromJson("{}"), std::exception);
  EXPECT_THROW(ScriptInvalidAfter::fromJson("{\"type\":\"unknown\"}"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidAfter::fromJson("{"), std::exception);
}

TEST(ScriptInvalidAfter, SupportsIndependentCopyAndMoveOperations)
{
  // Arrange
  const ScriptInvalidAfter original = ScriptInvalidAfter(42);
  ScriptInvalidAfter       copy(original);

  // Act
  ScriptInvalidAfter moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  ScriptInvalidAfter& alias = copy;
  copy                      = alias;
  copy                      = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(ScriptInvalidAfter, MutationUpdatesEncodingAndPreservesCopies)
{
  // Arrange
  ScriptInvalidAfter       value = ScriptInvalidAfter::fromCbor("82051801");
  const ScriptInvalidAfter original(value);

  // Act
  value.setSlot(std::numeric_limits<std::uint64_t>::max());
  ScriptInvalidAfter normalized(original);
  normalized.normalizeEncoding();

  // Assert
  EXPECT_EQ(value.slot(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(ScriptInvalidAfter::fromCbor(value.toCbor()), value);
  EXPECT_EQ(original.slot(), 1U);
  EXPECT_EQ(original.toCbor(), (std::vector<std::uint8_t> { 0x82, 0x05, 0x18, 1 }));
  EXPECT_EQ(normalized.toCbor(), (std::vector<std::uint8_t> { 0x82, 0x05, 1 }));
}

TEST(ScriptInvalidAfter, ParsesExactUnsignedSlotBounds)
{
  // Act & Assert
  EXPECT_EQ(ScriptInvalidAfter::fromJson("{\"type\":\"before\",\"slot\":18446744073709551615}").slot(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_THROW(ScriptInvalidAfter::fromJson("{\"type\":\"before\",\"slot\":-1}"), std::exception);
  EXPECT_THROW(ScriptInvalidAfter::fromJson("{\"type\":\"before\",\"slot\":1.5}"), std::exception);
  EXPECT_THROW(ScriptInvalidAfter::fromJson("{\"type\":\"before\",\"slot\":18446744073709551616}"), std::exception);
  EXPECT_THROW(ScriptInvalidAfter::fromJson(R"({"type":"before\u0000bad","slot":1})"), std::invalid_argument);
}
