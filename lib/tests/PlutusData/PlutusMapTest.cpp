/**
 * @file PlutusMapTest.cpp
 *
 * @brief Unit tests for PlutusMap.
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

TEST(PlutusMap, DecodesBytesAndHexWithoutTrailingData)
{
  // Arrange
  const PlutusMap                 expected = PlutusMap();
  CborReader                      reader("a0");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const PlutusMap fromHex   = PlutusMap::fromCbor("a0");
  const PlutusMap fromBytes = PlutusMap::fromCbor(bytes);

  // Assert
  EXPECT_EQ(fromHex, expected);
  EXPECT_EQ(fromBytes, expected);
  EXPECT_EQ(fromHex.toCbor(), bytes);
  EXPECT_EQ(fromBytes.toJson(), expected.toJson());
}

TEST(PlutusMap, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(PlutusMap::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(PlutusMap::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusMap::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(PlutusMap::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(PlutusMap::fromCbor("a000"), std::invalid_argument);
  EXPECT_THROW(PlutusMap::fromCbor("f5"), Error);
}

TEST(PlutusMap, SupportsCopyMoveAssignmentAndSwap)
{
  // Arrange
  const PlutusMap original = PlutusMap();
  PlutusMap       copy(original);

  // Act
  PlutusMap moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  PlutusMap& alias = copy;
  copy             = alias;
  copy             = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(PlutusMap, RetainsDuplicateKeysAndReturnsFirstMatchingValue)
{
  // Arrange
  PlutusMap map;

  // Act
  map.insert(PlutusData(1), PlutusData(2));
  map.insert(PlutusData(1), PlutusData(3));
  const std::vector<std::pair<PlutusData, PlutusData>> pairs = map.entries();

  // Assert
  EXPECT_EQ(map.size(), 2U);
  EXPECT_FALSE(map.empty());
  EXPECT_EQ(map.get(PlutusData(1)), PlutusData(2));
  EXPECT_FALSE(map.get(PlutusData(9)).has_value());
  EXPECT_EQ(map.keys(), (std::vector<PlutusData> { PlutusData(1), PlutusData(1) }));
  EXPECT_EQ(map.values(), (std::vector<PlutusData> { PlutusData(2), PlutusData(3) }));
  ASSERT_EQ(pairs.size(), 2U);
  EXPECT_EQ(pairs[1].second, PlutusData(3));
  EXPECT_EQ(PlutusMap::fromCbor(map.toCbor()), map);
  EXPECT_TRUE(PlutusMap().empty());
}

TEST(PlutusMap, MutationUpdatesEncodingWithoutChangingCopies)
{
  // Arrange
  PlutusMap        map = PlutusMap::fromCbor("a118011802");
  const PlutusMap  original(map);
  const PlutusData data(map);

  // Act
  map.insert(PlutusData(3), PlutusData(4));
  PlutusMap extracted = data.asMap();
  extracted.insert(PlutusData(5), PlutusData(6));
  PlutusData value = *original.get(PlutusData(1));
  value.normalizeEncoding();

  // Assert
  EXPECT_EQ(PlutusMap::fromCbor(map.toCbor()).size(), 2U);
  EXPECT_EQ(data.asMap().size(), 1U);
  EXPECT_EQ(data.kind(), PlutusDataKind::Map);
  EXPECT_EQ(original.toCbor(), (std::vector<std::uint8_t> { 0xa1, 0x18, 1, 0x18, 2 }));
  EXPECT_EQ(value.toCbor(), (std::vector<std::uint8_t> { 2 }));
}

TEST(PlutusMap, IteratorsPreserveDuplicateKeysAndPermitEntryEdits)
{
  // Arrange
  PlutusMap                       values   = PlutusMap::fromCbor("a201020103");
  const std::vector<std::uint8_t> original = values.toCbor();
  PlutusMap::iterator             first    = values.begin();

  // Act & Assert
  EXPECT_EQ(values.toCbor(), original);
  ASSERT_EQ(std::distance(values.begin(), values.end()), 2);
  EXPECT_EQ(first->first, std::next(first)->first);

  // Act
  first->second = PlutusData(4);
  values.toCbor();
  first->second           = PlutusData(5);
  const PlutusMap decoded = PlutusMap::fromCbor(values.toCbor());

  // Assert
  EXPECT_EQ(decoded.size(), 2U);
  EXPECT_EQ(decoded.front().second, PlutusData(5));
  EXPECT_EQ(decoded.back().second, PlutusData(3));
}
