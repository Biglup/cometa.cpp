/**
 * @file RedeemerTest.cpp
 *
 * @brief Unit tests for script redeemers.
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

TEST(Redeemer, MatchesNativeCbor)
{
  // Arrange
  const Redeemer redeemer(RedeemerTag::Spend, 2, PlutusData(0), ExUnits(5, 7));
  CborReader     reader("84000200820507");

  // Act
  const Redeemer decoded = Redeemer::fromCbor("84000200820507");

  // Assert
  EXPECT_EQ(redeemer.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, redeemer);
  EXPECT_EQ(decoded.tag(), RedeemerTag::Spend);
  EXPECT_EQ(decoded.index(), 2U);
  EXPECT_EQ(decoded.data(), PlutusData(0));
  EXPECT_EQ(decoded.exUnits(), ExUnits(5, 7));
  EXPECT_FALSE(decoded.toJson().empty());
}

TEST(Redeemer, MutationClearsCachedCborAndPreservesCopy)
{
  // Arrange
  Redeemer       redeemer = Redeemer::fromCbor("84000200820507");
  const Redeemer copy(redeemer);
  const auto     before = redeemer.toCbor();

  // Act
  redeemer.setTag(RedeemerTag::Mint);
  redeemer.setIndex(3);
  redeemer.setData(PlutusData(9));
  redeemer.setExUnits(ExUnits(11, 13));

  // Assert
  EXPECT_NE(redeemer.toCbor(), before);
  EXPECT_EQ(copy.toCbor(), before);
  EXPECT_EQ(redeemer, Redeemer::fromCbor(redeemer.toCbor()));
  EXPECT_EQ(redeemer.tag(), RedeemerTag::Mint);
  EXPECT_EQ(redeemer.index(), 3U);
  EXPECT_EQ(redeemer.data(), PlutusData(9));
  EXPECT_EQ(redeemer.exUnits(), ExUnits(11, 13));
}
