/**
 * @file PlutusListTest.cpp
 *
 * @brief Unit tests for PlutusList.
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

TEST(PlutusList, DecodesBytesAndHexWithoutTrailingData)
{
  // Arrange
  const PlutusList                expected = PlutusList();
  CborReader                      reader("80");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const PlutusList fromHex   = PlutusList::fromCbor("80");
  const PlutusList fromBytes = PlutusList::fromCbor(bytes);

  // Assert
  EXPECT_EQ(fromHex, expected);
  EXPECT_EQ(fromBytes, expected);
  EXPECT_EQ(fromHex.toCbor(), bytes);
  EXPECT_EQ(fromBytes.toJson(), expected.toJson());
}

TEST(PlutusList, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(PlutusList::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(PlutusList::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusList::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(PlutusList::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(PlutusList::fromCbor("8000"), std::invalid_argument);
  EXPECT_THROW(PlutusList::fromCbor("f5"), Error);
}

TEST(PlutusList, SupportsCopyMoveAssignmentAndSwap)
{
  // Arrange
  const PlutusList original = PlutusList();
  PlutusList       copy(original);

  // Act
  PlutusList moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  PlutusList& alias = copy;
  copy              = alias;
  copy              = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(PlutusList, MutationUpdatesEncodingAndLeavesCopiesIndependent)
{
  // Arrange
  PlutusList       list = PlutusList::fromCbor("811801");
  const PlutusList original(list);

  // Act
  list.pushBack(PlutusData(2));
  const std::vector<PlutusData> values = list.values();

  // Assert
  EXPECT_FALSE(list.empty());
  EXPECT_EQ(list.size(), 2U);
  EXPECT_EQ(values, (std::vector<PlutusData> { PlutusData(1), PlutusData(2) }));
  EXPECT_EQ(PlutusList::fromCbor(list.toCbor()), list);
  EXPECT_EQ(original.toCbor(), (std::vector<std::uint8_t> { 0x81, 0x18, 0x01 }));
  EXPECT_THROW(list.at(2), std::out_of_range);
  EXPECT_TRUE(PlutusList().empty());
}

TEST(PlutusList, ElementNormalizationDoesNotChangeTheList)
{
  // Arrange
  const PlutusList list = PlutusList::fromCbor("811801");

  // Act
  PlutusData element = list.at(0);
  element.normalizeEncoding();

  // Assert
  EXPECT_EQ(element.toCbor(), (std::vector<std::uint8_t> { 1 }));
  EXPECT_EQ(list.toCbor(), (std::vector<std::uint8_t> { 0x81, 0x18, 1 }));
}
