/**
 * @file PlutusV4ScriptListTest.cpp
 *
 * @brief Unit tests for PlutusV4ScriptList.
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

TEST(PlutusV4ScriptList, RetainsOrderedIndependentScripts)
{
  // Arrange
  PlutusV4Script       first  = PlutusV4Script::fromHex("450100002499");
  const PlutusV4Script second = PlutusV4Script::fromHex("01");
  PlutusV4ScriptList   scripts { first, second };

  // Act
  first                   = second;
  PlutusV4Script returned = scripts.at(0);
  returned                = second;
  scripts.pushBack(first);

  // Assert
  EXPECT_EQ(scripts.size(), 3U);
  EXPECT_FALSE(scripts.empty());
  EXPECT_EQ(scripts.at(0).toHex(), "450100002499");
  EXPECT_EQ(scripts.at(1), second);
  EXPECT_EQ(scripts.at(2), second);
  EXPECT_EQ(returned, second);
  EXPECT_EQ(scripts.values().size(), 3U);
  EXPECT_THROW(scripts.at(3), std::out_of_range);
  EXPECT_EQ(PlutusV4ScriptList::fromCbor(scripts.toCbor()), scripts);
}

TEST(PlutusV4ScriptList, EncodesKnownOrderedSequence)
{
  // Arrange
  const PlutusV4ScriptList scripts { PlutusV4Script::fromHex("450100002499"), PlutusV4Script::fromHex("01") };
  CborReader               expected("82464501000024994101");

  // Act & Assert
  EXPECT_EQ(scripts.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(PlutusV4ScriptList::fromCbor("9f464501000024994101ff"), scripts);
  EXPECT_EQ(scripts.toJson(), R"([{"language":"plutus_v4","bytes":"450100002499"},{"language":"plutus_v4","bytes":"01"}])");
}

TEST(PlutusV4ScriptList, NormalizesEmptyIndefiniteArrays)
{
  // Arrange
  const PlutusV4ScriptList scripts = PlutusV4ScriptList::fromCbor("9fff");
  CborReader               expected("80");

  // Act & Assert
  EXPECT_TRUE(scripts.empty());
  EXPECT_EQ(scripts.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(scripts.toJson(), "[]");
}

TEST(PlutusV4ScriptList, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const PlutusV4ScriptList  scripts { PlutusV4Script::fromHex("01") };
  std::vector<std::uint8_t> bytes = scripts.toCbor();
  EXPECT_EQ(PlutusV4ScriptList::fromCbor(bytes), scripts);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(PlutusV4ScriptList::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(PlutusV4ScriptList::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(PlutusV4ScriptList::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusV4ScriptList::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(PlutusV4ScriptList::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(PlutusV4ScriptList::fromCbor("f5"), Error);
  EXPECT_THROW(PlutusV4ScriptList::fromCbor("8100"), Error);
}
