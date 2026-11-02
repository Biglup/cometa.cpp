/**
 * @file MetadatumMapTest.cpp
 *
 * @brief Unit tests for MetadatumMap.
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

TEST(MetadatumMap, RetainsDuplicateKeysAndReturnsFirstMatch)
{
  // Arrange
  MetadatumMap    map;
  const Metadatum key("key");
  map.insert(key, Metadatum(std::int64_t { 1 }));
  map.insert(key, Metadatum(std::int64_t { 2 }));

  // Act & Assert
  EXPECT_EQ(map.size(), 2U);
  EXPECT_FALSE(map.empty());
  ASSERT_TRUE(map.get(key).has_value());
  EXPECT_EQ(map.get(key)->asInteger(), BigInt(1));
  EXPECT_FALSE(map.get(Metadatum("missing")).has_value());
  EXPECT_EQ(map.keys(), (std::vector<Metadatum> { key, key }));
  EXPECT_EQ(map.values().at(1).asInteger(), BigInt(2));
  EXPECT_EQ(map.entries().size(), 2U);
  EXPECT_EQ(map.entryAt(1).second.asInteger(), BigInt(2));
  EXPECT_THROW(map.entryAt(2), std::out_of_range);
  EXPECT_EQ(MetadatumMap::fromCbor(map.toCbor()), map);
}

TEST(MetadatumMap, CopiesNestedValuesAndRetainsLengthForm)
{
  // Arrange
  MetadatumMap    map = MetadatumMap::fromCbor("bf616b8101ff");
  const Metadatum value(map);
  MetadatumList   list { Metadatum(std::int64_t { 2 }) };
  CborReader      expected("bf616b810161788102ff");

  // Act
  map.insert(Metadatum("x"), Metadatum(list));
  list.pushBack(Metadatum(std::int64_t { 3 }));
  MetadatumMap returned = value.asMap();
  returned.insert(Metadatum("y"), Metadatum(std::int64_t { 4 }));

  // Assert
  EXPECT_EQ(map.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(value.asMap().size(), 1U);
  EXPECT_EQ(returned.size(), 2U);
  EXPECT_EQ(map.get(Metadatum("x"))->asList().size(), 1U);
  EXPECT_EQ(list.size(), 2U);
  EXPECT_TRUE(MetadatumMap().empty());
  EXPECT_THROW(MetadatumMap::fromCbor("a000"), std::invalid_argument);
  EXPECT_THROW(MetadatumMap::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(MetadatumMap::fromCbor("00"), Error);
}
