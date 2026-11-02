/**
 * @file RequiredGuardsMapTest.cpp
 *
 * @brief Unit tests for required guards and their optional datums.
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

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(RequiredGuardsMap, MatchesNativeDijkstraCborAndNullableDatums)
{
  // Arrange
  const std::string_view cbor   = "a28200581c00112233445566778899aabbccddeeff00112233445566778899aabbf68201581caabbccddeeff00112233445566778899aabbccddeeff001122334455d87980";
  const Credential       key    = Credential::fromCbor("8200581c00112233445566778899aabbccddeeff00112233445566778899aabb");
  const Credential       script = Credential::fromCbor("8201581caabbccddeeff00112233445566778899aabbccddeeff001122334455");
  CborReader             reader(cbor);

  // Act
  const RequiredGuardsMap map = RequiredGuardsMap::fromCbor(cbor);

  // Assert
  EXPECT_EQ(map.size(), 2U);
  EXPECT_EQ(map.toCbor(), reader.readEncodedValue());
  EXPECT_TRUE(map.contains(key));
  EXPECT_TRUE(map.contains(script));
  ASSERT_TRUE(map.get(key).has_value());
  EXPECT_FALSE(map.get(key)->has_value());
  EXPECT_FALSE(map.at(key).has_value());
  ASSERT_TRUE(map.get(script).has_value());
  ASSERT_TRUE(map.get(script)->has_value());
  EXPECT_EQ(map.get(script)->value(), PlutusData::fromCbor("d87980"));
  EXPECT_EQ(map.at(script).value(), PlutusData::fromCbor("d87980"));
  EXPECT_EQ(map.entryAt(0).first, key);
  EXPECT_EQ(map.entryAt(1).first, script);
  EXPECT_EQ(map.keys().size(), 2U);
  EXPECT_EQ(map.entries().size(), 2U);
  EXPECT_FALSE(map.toJson().empty());
}

TEST(RequiredGuardsMap, SortsEntriesAndRejectsDuplicateCredentials)
{
  // Arrange
  const Credential  key    = Credential::fromCbor("8200581c00112233445566778899aabbccddeeff00112233445566778899aabb");
  const Credential  script = Credential::fromCbor("8201581caabbccddeeff00112233445566778899aabbccddeeff001122334455");
  RequiredGuardsMap map;

  // Act
  map.insert(script, PlutusData::fromCbor("d87980"));
  const RequiredGuardsMap copy(map);
  map.insert(key);

  // Assert
  EXPECT_EQ(map.entryAt(0).first, key);
  EXPECT_EQ(map.entryAt(1).first, script);
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_FALSE(copy.contains(key));
  EXPECT_FALSE(copy.get(key).has_value());
  EXPECT_THROW(copy.at(key), std::out_of_range);
  EXPECT_EQ(RequiredGuardsMap::fromCbor(map.toCbor()), map);
  EXPECT_THROW(map.insert(key), Error);
  EXPECT_THROW(map.entryAt(2), std::out_of_range);
}
