/**
 * @file PlutusDataSetTest.cpp
 *
 * @brief Unit tests for Plutus data witness sets.
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

TEST(PlutusDataSet, MatchesNativeCborAndSupportsSetTag)
{
  // Arrange
  PlutusDataSet set;
  CborReader    reader("d901028101");

  // Act
  set.add(PlutusData(1));
  const PlutusDataSet decoded = PlutusDataSet::fromCbor(set.toCbor());

  // Assert
  EXPECT_EQ(set.size(), 1U);
  EXPECT_EQ(set.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, set);
  EXPECT_EQ(set.at(0), PlutusData(1));
  EXPECT_TRUE(set.useTag());
  EXPECT_FALSE(set.toJson().empty());

  // Act
  set.setUseTag(false);

  // Assert
  EXPECT_FALSE(set.useTag());
  EXPECT_EQ(set.toCbor().front(), 0x81);

  PlutusDataSet parsed = PlutusDataSet::fromCbor("d901028101");
  parsed.setUseTag(false);
  EXPECT_EQ(parsed.toCbor().front(), 0x81);
}

TEST(PlutusDataSet, ClearsDecodedCborAfterMutationAndDeduplicates)
{
  // Arrange
  PlutusDataSet       set = PlutusDataSet::fromCbor("d901028101");
  const PlutusDataSet copy(set);

  // Act
  set.add(PlutusData(1));
  set.add(PlutusData(2));

  // Assert
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_EQ(set.size(), 2U);
  EXPECT_EQ(set.at(0), PlutusData(1));
  EXPECT_EQ(set.at(1), PlutusData(2));
  EXPECT_EQ(set.toCbor(), CborReader("d90102820102").readEncodedValue());
  EXPECT_THROW(set.at(2), std::out_of_range);
  EXPECT_EQ(PlutusDataSet::fromCbor(set.toCbor()), set);
}
