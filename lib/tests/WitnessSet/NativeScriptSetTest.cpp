/**
 * @file NativeScriptSetTest.cpp
 *
 * @brief Unit tests for native script witness sets.
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

TEST(NativeScriptSet, MatchesNativeCborAndSupportsSetTag)
{
  // Arrange
  const NativeScript script(ScriptInvalidBefore(5));
  NativeScriptSet    set;
  CborReader         reader("d9010281820405");

  // Act
  set.add(script);
  const NativeScriptSet decoded = NativeScriptSet::fromCbor(set.toCbor());

  // Assert
  EXPECT_EQ(set.size(), 1U);
  EXPECT_EQ(set.toCbor(), reader.readEncodedValue());
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

TEST(NativeScriptSet, KeepsOneScriptPerHashAndCopiesIndependently)
{
  // Arrange
  const NativeScript first(ScriptInvalidBefore(5));
  const NativeScript second(ScriptInvalidBefore(6));
  NativeScriptSet    set;
  set.add(first);
  const NativeScriptSet copy(set);

  // Act
  set.add(first);
  set.add(second);

  // Assert
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_EQ(set.size(), 2U);
  EXPECT_EQ(copy.at(0), first);
  EXPECT_EQ(set.at(1), second);
  EXPECT_THROW(set.at(2), std::out_of_range);
  EXPECT_EQ(NativeScriptSet::fromCbor(set.toCbor()), set);
}
