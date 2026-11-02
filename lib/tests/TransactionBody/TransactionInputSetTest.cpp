/**
 * @file TransactionInputSetTest.cpp
 *
 * @brief Unit tests for TransactionInputSet.
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

TEST(TransactionInputSet, SortsDeduplicatesAndCopiesInputs)
{
  // Arrange
  const TransactionInput first = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  TransactionInput       second(first.id(), 8);
  TransactionInputSet    values { second, first, second };

  // Act
  second.setIndex(9);
  TransactionInput returned = values.at(0);
  returned.setIndex(10);

  // Assert
  EXPECT_EQ(values.size(), 2U);
  EXPECT_FALSE(values.empty());
  EXPECT_TRUE(values.wasTagged());
  EXPECT_TRUE(values.contains(first));
  EXPECT_FALSE(values.contains(second));
  EXPECT_FALSE(values.insert(first));
  EXPECT_EQ(values.at(0).index(), 5U);
  EXPECT_EQ(values.at(1).index(), 8U);
  EXPECT_EQ(returned.index(), 10U);
  EXPECT_THROW(values.at(2), std::out_of_range);
  EXPECT_EQ(TransactionInputSet::fromCbor(values.toCbor()), values);
}

TEST(TransactionInputSet, TracksOriginalTagAndRejectsDuplicates)
{
  // Arrange
  const std::string         input    = "825820010101010101010101010101010101010101010101010101010101010101010105";
  const TransactionInputSet tagged   = TransactionInputSet::fromCbor("d9010281" + input);
  const TransactionInputSet untagged = TransactionInputSet::fromCbor("81" + input);
  CborReader                expected("d9010281" + input);

  // Act & Assert
  EXPECT_TRUE(tagged.wasTagged());
  EXPECT_FALSE(untagged.wasTagged());
  EXPECT_EQ(tagged, untagged);
  EXPECT_EQ(untagged.toCbor(), expected.readEncodedValue());
  EXPECT_THROW(TransactionInputSet::fromCbor("82" + input + input), std::invalid_argument);
  EXPECT_THROW(TransactionInputSet::fromCbor("d9010380"), Error);
}

TEST(TransactionInputSet, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const TransactionInputSet value = TransactionInputSet { TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5) };
  std::vector<std::uint8_t> bytes = value.toCbor();
  EXPECT_EQ(TransactionInputSet::fromCbor(bytes), value);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(TransactionInputSet::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(TransactionInputSet::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(TransactionInputSet::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(TransactionInputSet::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(TransactionInputSet::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(TransactionInputSet::fromCbor("f5"), Error);
}
