/**
 * @file RedeemerListTest.cpp
 *
 * @brief Unit tests for redeemer lists.
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

TEST(RedeemerList, MatchesNativeMapEncodingAndOrdersKeys)
{
  // Arrange
  const Redeemer first(RedeemerTag::Spend, 2, PlutusData(0), ExUnits(5, 7));
  const Redeemer second(RedeemerTag::Mint, 0, PlutusData(9), ExUnits(11, 13));
  RedeemerList   values;
  CborReader     reader("a282000282008205078201008209820b0d");

  // Act
  values.add(second);
  values.add(first);

  // Assert
  EXPECT_EQ(values.size(), 2U);
  EXPECT_EQ(values.at(0), first);
  EXPECT_EQ(values.at(1), second);
  EXPECT_EQ(values.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(RedeemerList::fromCbor(values.toCbor()), values);
  EXPECT_FALSE(values.toJson().empty());
}

TEST(RedeemerList, CopiesAndRejectsDuplicateKeys)
{
  // Arrange
  const Redeemer first(RedeemerTag::Spend, 2, PlutusData(0), ExUnits(5, 7));
  RedeemerList   values;
  values.add(first);
  const RedeemerList copy(values);

  // Act
  values.add(Redeemer(RedeemerTag::Mint, 0, PlutusData(9), ExUnits(11, 13)));

  // Assert
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_EQ(values.size(), 2U);
  EXPECT_EQ(copy.at(0), first);
  EXPECT_THROW(values.at(2), std::out_of_range);
  EXPECT_THROW(values.add(first), std::invalid_argument);
}

TEST(RedeemerList, ChangesExecutionBudgetAfterDecoding)
{
  // Arrange
  RedeemerList                    values = RedeemerList::fromCbor("a18200028200820507");
  const std::vector<std::uint8_t> before = values.toCbor();

  // Act
  values.setExUnits(RedeemerTag::Spend, 2, ExUnits(11, 13));

  // Assert
  EXPECT_NE(values.toCbor(), before);
  EXPECT_EQ(values.at(0).exUnits(), ExUnits(11, 13));
  EXPECT_THROW(values.setExUnits(RedeemerTag::Mint, 2, ExUnits(1, 2)), Error);
}
