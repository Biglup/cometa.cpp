/**
 * @file NativeScriptListTest.cpp
 *
 * @brief Unit tests for PlutusData.
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

TEST(NativeScriptList, MatchesCborAndCliJson)
{
  // Arrange
  const NativeScriptList          expected = NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) };
  CborReader                      reader("82820405820507");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const NativeScriptList decoded = NativeScriptList::fromCbor("82820405820507");
  const NativeScriptList parsed  = NativeScriptList::fromJson(R"({"scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})");

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(NativeScriptList::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(NativeScriptList, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(NativeScriptList::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(NativeScriptList::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(NativeScriptList::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(NativeScriptList::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(NativeScriptList::fromCbor("8282040582050700"), std::invalid_argument);
  EXPECT_THROW(NativeScriptList::fromCbor("f5"), Error);
  EXPECT_THROW(NativeScriptList::fromJson("{}"), std::exception);
  EXPECT_THROW(NativeScriptList::fromJson("{"), std::exception);
}

TEST(NativeScriptList, RetainsChildEncodingsAndNormalizesArrayHeaders)
{
  // Arrange
  NativeScriptList       list = NativeScriptList::fromCbor("980182041805");
  const NativeScriptList original(list);

  // Act
  NativeScript child = list.at(0);
  child.normalizeEncoding();
  list.pushBack(NativeScript(ScriptInvalidAfter(7)));

  // Assert
  EXPECT_EQ(original.toCbor(), (std::vector<std::uint8_t> { 0x81, 0x82, 4, 0x18, 5 }));
  EXPECT_EQ(child.toCbor(), (std::vector<std::uint8_t> { 0x82, 4, 5 }));
  EXPECT_EQ(list.at(0).toCbor(), original.at(0).toCbor());
  EXPECT_EQ(list.values().size(), 2U);
  EXPECT_THROW(list.at(2), std::out_of_range);
  EXPECT_FALSE(list.empty());
  EXPECT_TRUE(NativeScriptList().empty());
}

TEST(NativeScriptList, PreservesIndefiniteArrayForm)
{
  // Arrange
  NativeScriptList list = NativeScriptList::fromCbor("9f82041805ff");

  // Act
  list.normalizeEncoding();

  // Assert
  EXPECT_EQ(list.toCbor(), (std::vector<std::uint8_t> { 0x9f, 0x82, 4, 5, 0xff }));
  EXPECT_THROW(NativeScriptList::fromJson(R"({"scripts":{}})"), std::invalid_argument);
}
