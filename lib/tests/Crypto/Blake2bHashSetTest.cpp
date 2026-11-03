/**
 * @file Blake2bHashSetTest.cpp
 *
 * @brief Unit tests for Blake2bHashSet.
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

#include <Cometa/Cbor/CborReader.h>
#include <Cometa/Crypto/Blake2bHashSet.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Blake2bHashSet, InsertsUniqueValuesInSortedOrder)
{
  // Arrange
  const Blake2bHash first  = Blake2bHash::fromHex("01");
  const Blake2bHash second = Blake2bHash::fromHex("02");
  Blake2bHashSet    values;

  // Act
  const bool insertedSecond = values.insert(second).second;
  const bool insertedFirst  = values.insert(first).second;
  const bool duplicate      = values.insert(first).second;

  // Assert
  EXPECT_TRUE(insertedFirst);
  EXPECT_TRUE(insertedSecond);
  EXPECT_FALSE(duplicate);
  EXPECT_FALSE(values.empty());
  EXPECT_EQ(values.size(), 2U);
  EXPECT_TRUE(values.contains(first));
  EXPECT_EQ(values.at(0), first);
  EXPECT_EQ(values.at(1), second);
  EXPECT_EQ(values.values(), (std::vector<Blake2bHash> { first, second }));
}

TEST(Blake2bHashSet, InitializerIgnoresDuplicates)
{
  // Arrange
  const Blake2bHash first = Blake2bHash::fromHex("01");

  // Act
  const Blake2bHashSet values { first, first };

  // Assert
  EXPECT_EQ(values.size(), 1U);
  EXPECT_FALSE(values.contains(Blake2bHash::fromHex("02")));
}

TEST(Blake2bHashSet, EmptySetHasTaggedEncoding)
{
  // Arrange
  const Blake2bHashSet values;

  // Act & Assert
  EXPECT_TRUE(values.empty());
  EXPECT_EQ(values.toCbor(), (std::vector<std::uint8_t> { 0xd9, 0x01, 0x02, 0x80 }));
  EXPECT_EQ(values.toJson(), "[]");
  EXPECT_THROW(values.at(0), std::out_of_range);
  EXPECT_EQ(Blake2bHashSet::fromCbor("80"), values);
}

TEST(Blake2bHashSet, NormalizesUntaggedUnsortedAndIndefiniteInput)
{
  // Arrange
  const std::string untagged   = "8241024101";
  const std::string tagged     = "d901028241014102";
  const std::string indefinite = "9f41024101ff";

  // Act
  const Blake2bHashSet values   = Blake2bHashSet::fromCbor(untagged);
  const Blake2bHashSet expected = Blake2bHashSet::fromCbor(tagged);

  // Assert
  EXPECT_EQ(values, expected);
  CborReader expectedEncoding(tagged);
  EXPECT_EQ(values.toCbor(), expectedEncoding.readEncodedValue());
  EXPECT_EQ(Blake2bHashSet::fromCbor(indefinite), expected);
  EXPECT_EQ(Blake2bHashSet::fromCbor(values.toCbor()), expected);
  EXPECT_EQ(values.at(0), Blake2bHash::fromHex("01"));
}

TEST(Blake2bHashSet, RejectsDuplicatesWrongTagsAndMalformedInput)
{
  // Act & Assert
  EXPECT_THROW(Blake2bHashSet::fromCbor("8241014101"), std::invalid_argument);
  EXPECT_THROW(Blake2bHashSet::fromCbor("d9010380"), Error);
  EXPECT_THROW(Blake2bHashSet::fromCbor("db000000010000010280"), std::out_of_range);
  EXPECT_THROW(Blake2bHashSet::fromCbor("8180"), Error);
  EXPECT_THROW(Blake2bHashSet::fromCbor("81"), Error);
  EXPECT_THROW(Blake2bHashSet::fromCbor("a0"), Error);
  EXPECT_THROW(Blake2bHashSet::fromCbor("8000"), std::invalid_argument);
  EXPECT_THROW(Blake2bHashSet::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Blake2bHashSet::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(Blake2bHashSet::fromCbor("z0"), std::invalid_argument);
  EXPECT_THROW(Blake2bHashSet::fromCbor("0"), std::invalid_argument);
}

TEST(Blake2bHashSet, CopiesMovesAndSwapsIndependentSets)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  Blake2bHashSet       copy(source);

  // Act
  copy.insert(Blake2bHash::fromHex("02"));
  Blake2bHashSet moved(std::move(copy));
  copy = source;
  swap(copy, moved);
  Blake2bHashSet& alias = copy;
  copy                  = alias;
  copy                  = std::move(alias);

  // Assert
  EXPECT_EQ(source.size(), 1U);
  EXPECT_EQ(copy.size(), 2U);
  EXPECT_EQ(moved, source);
  EXPECT_NE(copy, source);
  EXPECT_THROW(copy.at(2), std::out_of_range);
}

TEST(Blake2bHashSet, ReturnsIndependentSnapshots)
{
  // Arrange
  const Blake2bHash        first = Blake2bHash::fromHex("01");
  const Blake2bHashSet     source { first };
  std::vector<Blake2bHash> snapshot = source.values();

  // Act
  snapshot.front() = Blake2bHash::fromHex("02");

  // Assert
  EXPECT_EQ(source.at(0), first);
  EXPECT_NE(source.at(0), snapshot.front());
}

TEST(Blake2bHashSet, SerializesKnownJsonAndCbor)
{
  // Arrange
  const Blake2bHashSet set { Blake2bHash::fromHex("02"), Blake2bHash::fromHex("01") };

  // Act & Assert
  EXPECT_EQ(set.toJson(), "[\"01\",\"02\"]");
  EXPECT_EQ(set.toCbor(), (std::vector<std::uint8_t> { 0xd9, 0x01, 0x02, 0x82, 0x41, 1, 0x41, 2 }));
}
