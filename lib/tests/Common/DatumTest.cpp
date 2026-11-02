/**
 * @file DatumTest.cpp
 *
 * @brief Unit tests for Datum.
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

TEST(Datum, DecodesBytesAndHexWithoutTrailingData)
{
  // Arrange
  const Datum                     expected = Datum(PlutusData(42));
  CborReader                      reader("8201d81842182a");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const Datum fromHex   = Datum::fromCbor("8201d81842182a");
  const Datum fromBytes = Datum::fromCbor(bytes);

  // Assert
  EXPECT_EQ(fromHex, expected);
  EXPECT_EQ(fromBytes, expected);
  EXPECT_EQ(fromHex.toCbor(), bytes);
  EXPECT_EQ(fromBytes.toJson(), expected.toJson());
}

TEST(Datum, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(Datum::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Datum::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(Datum::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(Datum::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(Datum::fromCbor("8201d81842182a00"), std::invalid_argument);
  EXPECT_THROW(Datum::fromCbor("f5"), Error);
}

TEST(Datum, SupportsCopyMoveAssignmentAndSwap)
{
  // Arrange
  const Datum original = Datum(PlutusData(42));
  Datum       copy(original);

  // Act
  Datum moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  Datum& alias = copy;
  copy         = alias;
  copy         = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(Datum, HashTracksPreservedInlineDataEncoding)
{
  // Arrange
  PlutusData  data = PlutusData::fromCbor("1817");
  const Datum original(data);

  // Act
  data.normalizeEncoding();
  const Datum normalized(data);
  PlutusData  extracted = *original.inlineData();
  extracted.normalizeEncoding();

  // Assert
  EXPECT_EQ(original.type(), DatumType::InlineData);
  EXPECT_EQ(original.hash(), Blake2bHash::compute({ 0x18, 0x17 }));
  EXPECT_EQ(normalized.hash(), Blake2bHash::compute({ 0x17 }));
  EXPECT_NE(original.hash(), normalized.hash());
  EXPECT_EQ(original.inlineData()->toCbor(), (std::vector<std::uint8_t> { 0x18, 0x17 }));
}

TEST(Datum, ValidatesAndReplacesHashValues)
{
  // Arrange
  const Blake2bHash first  = Blake2bHash::fromHex(std::string(64, '1'));
  const Blake2bHash second = Blake2bHash::fromHex(std::string(64, '2'));
  Datum             datum(first);
  const Datum       copy(datum);
  Datum             inlineDatum(PlutusData(0));

  // Act
  datum.setHash(second);

  // Assert
  EXPECT_EQ(datum.type(), DatumType::DataHash);
  EXPECT_EQ(datum.hash(), second);
  EXPECT_EQ(copy.hash(), first);
  EXPECT_FALSE(datum.inlineData().has_value());
  EXPECT_EQ(Datum::fromCbor(datum.toCbor()), datum);
  EXPECT_THROW(Datum(Blake2bHash::fromHex(std::string(56, '0'))), std::invalid_argument);
  EXPECT_THROW(inlineDatum.setHash(first), std::logic_error);
}
