/**
 * @file ScriptAnyTest.cpp
 *
 * @brief Unit tests for ScriptAny.
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

TEST(ScriptAny, MatchesCborAndCliJson)
{
  // Arrange
  const ScriptAny                 expected = ScriptAny(NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) });
  CborReader                      reader("820282820405820507");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ScriptAny decoded = ScriptAny::fromCbor("820282820405820507");
  const ScriptAny parsed  = ScriptAny::fromJson(R"({"type":"any","scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})");

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(ScriptAny::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(ScriptAny, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(ScriptAny::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ScriptAny::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ScriptAny::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ScriptAny::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ScriptAny::fromCbor("82028282040582050700"), std::invalid_argument);
  EXPECT_THROW(ScriptAny::fromCbor("f5"), Error);
  EXPECT_THROW(ScriptAny::fromJson("{}"), std::exception);
  EXPECT_THROW(ScriptAny::fromJson("{"), std::exception);
}

TEST(ScriptAny, CopiesChildrenOnConstructionMutationAndAccess)
{
  // Arrange
  NativeScriptList children { NativeScript(ScriptInvalidBefore(5)) };
  ScriptAny        script(children);
  const ScriptAny  original(script);

  // Act
  children.pushBack(NativeScript(ScriptInvalidAfter(7)));
  script.setScripts(children);
  NativeScriptList extracted = script.scripts();
  extracted.pushBack(NativeScript(ScriptInvalidBefore(9)));

  // Assert
  EXPECT_EQ(original.size(), 1U);
  EXPECT_EQ(script.size(), 2U);
  EXPECT_EQ(extracted.size(), 3U);
  EXPECT_EQ(ScriptAny::fromCbor(script.toCbor()), script);
  EXPECT_NE(original, script);
}
