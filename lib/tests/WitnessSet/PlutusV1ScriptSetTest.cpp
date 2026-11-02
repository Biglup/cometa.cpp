/**
 * @file PlutusV1ScriptSetTest.cpp
 *
 * @brief Unit tests for Plutus V1 script witness sets.
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

TEST(PlutusV1ScriptSet, MatchesNativeCborAndSupportsSetTag)
{
  // Arrange
  const PlutusV1Script            script(std::vector<std::uint8_t> { 0x01, 0x02 });
  PlutusV1ScriptSet               set;
  std::vector<std::uint8_t>       expected { 0xd9, 0x01, 0x02, 0x81 };
  const std::vector<std::uint8_t> scriptCbor = script.toCbor();
  expected.insert(expected.end(), scriptCbor.begin(), scriptCbor.end());

  // Act
  set.add(script);
  const PlutusV1ScriptSet decoded = PlutusV1ScriptSet::fromCbor(expected);

  // Assert
  EXPECT_EQ(set.size(), 1U);
  EXPECT_EQ(set.toCbor(), expected);
  EXPECT_EQ(decoded, set);
  EXPECT_EQ(set.at(0), script);
  EXPECT_TRUE(set.useTag());
  EXPECT_FALSE(set.toJson().empty());

  // Act
  set.setUseTag(false);

  // Assert
  EXPECT_FALSE(set.useTag());
  EXPECT_EQ(set.toCbor().front(), 0x81);
}

TEST(PlutusV1ScriptSet, DeduplicatesByHashAndCopiesIndependently)
{
  // Arrange
  const PlutusV1Script first(std::vector<std::uint8_t> { 0x01, 0x02 });
  const PlutusV1Script second(std::vector<std::uint8_t> { 0x03 });
  PlutusV1ScriptSet    set;
  set.add(first);
  const PlutusV1ScriptSet copy(set);

  // Act
  set.add(first);
  set.add(second);

  // Assert
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_EQ(set.size(), 2U);
  EXPECT_EQ(copy.at(0), first);
  EXPECT_EQ(set.at(1), second);
  EXPECT_THROW(set.at(2), std::out_of_range);
  EXPECT_EQ(PlutusV1ScriptSet::fromCbor(set.toCbor()), set);
}
