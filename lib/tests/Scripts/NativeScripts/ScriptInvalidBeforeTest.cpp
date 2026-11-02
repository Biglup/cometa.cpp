/**
 * @file ScriptInvalidBeforeTest.cpp
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

TEST(ScriptInvalidBefore, MatchesCborAndCliJson)
{
  // Arrange
  const ScriptInvalidBefore       expected = ScriptInvalidBefore(42);
  const std::string               cbor     = "8204182a";
  const std::string               json     = "{\"type\":\"after\",\"slot\":42}";
  CborReader                      reader(cbor);
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ScriptInvalidBefore decoded = ScriptInvalidBefore::fromCbor(cbor);
  const ScriptInvalidBefore parsed  = ScriptInvalidBefore::fromJson(json);

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(ScriptInvalidBefore::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(ScriptInvalidBefore, RejectsInvalidInputAndWrongVariants)
{
  // Arrange
  const std::string cbor = "8204182a";

  // Act & Assert
  EXPECT_THROW(ScriptInvalidBefore::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidBefore::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidBefore::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidBefore::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidBefore::fromCbor(cbor + "00"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidBefore::fromCbor("00"), Error);
  EXPECT_THROW(ScriptInvalidBefore::fromJson("{}"), std::exception);
  EXPECT_THROW(ScriptInvalidBefore::fromJson("{\"type\":\"unknown\"}"), std::invalid_argument);
  EXPECT_THROW(ScriptInvalidBefore::fromJson("{"), std::exception);
}

TEST(ScriptInvalidBefore, SupportsIndependentCopyAndMoveOperations)
{
  // Arrange
  const ScriptInvalidBefore original = ScriptInvalidBefore(42);
  ScriptInvalidBefore       copy(original);

  // Act
  ScriptInvalidBefore moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  ScriptInvalidBefore& alias = copy;
  copy                       = alias;
  copy                       = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(ScriptInvalidBefore, MutationUpdatesEncodingAndPreservesCopies)
{
  // Arrange
  ScriptInvalidBefore       value = ScriptInvalidBefore::fromCbor("82041801");
  const ScriptInvalidBefore original(value);

  // Act
  value.setSlot(std::numeric_limits<std::uint64_t>::max());
  ScriptInvalidBefore normalized(original);
  normalized.normalizeEncoding();

  // Assert
  EXPECT_EQ(value.slot(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(ScriptInvalidBefore::fromCbor(value.toCbor()), value);
  EXPECT_EQ(original.slot(), 1U);
  EXPECT_EQ(original.toCbor(), (std::vector<std::uint8_t> { 0x82, 0x04, 0x18, 1 }));
  EXPECT_EQ(normalized.toCbor(), (std::vector<std::uint8_t> { 0x82, 0x04, 1 }));
}

TEST(ScriptInvalidBefore, ParsesExactUnsignedSlotBounds)
{
  // Act & Assert
  EXPECT_EQ(ScriptInvalidBefore::fromJson("{\"type\":\"after\",\"slot\":18446744073709551615}").slot(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_THROW(ScriptInvalidBefore::fromJson("{\"type\":\"after\",\"slot\":-1}"), std::exception);
  EXPECT_THROW(ScriptInvalidBefore::fromJson("{\"type\":\"after\",\"slot\":1.5}"), std::exception);
  EXPECT_THROW(ScriptInvalidBefore::fromJson("{\"type\":\"after\",\"slot\":18446744073709551616}"), std::exception);
  EXPECT_THROW(ScriptInvalidBefore::fromJson(R"({"type":"after\u0000bad","slot":1})"), std::invalid_argument);
}
