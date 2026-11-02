/**
 * @file ScriptNOfKTest.cpp
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

TEST(ScriptNOfK, MatchesCborAndCliJson)
{
  // Arrange
  const ScriptNOfK                expected = ScriptNOfK(1, NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) });
  CborReader                      reader("83030182820405820507");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ScriptNOfK decoded = ScriptNOfK::fromCbor("83030182820405820507");
  const ScriptNOfK parsed  = ScriptNOfK::fromJson(R"({"type":"atLeast","required":1,"scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})");

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(ScriptNOfK::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(ScriptNOfK, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(ScriptNOfK::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ScriptNOfK::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ScriptNOfK::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ScriptNOfK::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ScriptNOfK::fromCbor("8303018282040582050700"), std::invalid_argument);
  EXPECT_THROW(ScriptNOfK::fromCbor("f5"), Error);
  EXPECT_THROW(ScriptNOfK::fromJson("{}"), std::exception);
  EXPECT_THROW(ScriptNOfK::fromJson("{"), std::exception);
}

TEST(ScriptNOfK, CopiesChildrenOnConstructionMutationAndAccess)
{
  // Arrange
  NativeScriptList children { NativeScript(ScriptInvalidBefore(5)) };
  ScriptNOfK       script(1, children);
  const ScriptNOfK original(script);

  // Act
  children.pushBack(NativeScript(ScriptInvalidAfter(7)));
  script.setScripts(children);
  NativeScriptList extracted = script.scripts();
  extracted.pushBack(NativeScript(ScriptInvalidBefore(9)));

  // Assert
  EXPECT_EQ(original.size(), 1U);
  EXPECT_EQ(script.size(), 2U);
  EXPECT_EQ(extracted.size(), 3U);
  EXPECT_EQ(ScriptNOfK::fromCbor(script.toCbor()), script);
  EXPECT_NE(original, script);
}

TEST(ScriptNOfK, ComparesThresholdsAndUpdatesTheirEncoding)
{
  // Arrange
  const NativeScriptList children { NativeScript(ScriptInvalidBefore(5)) };
  ScriptNOfK             first(1, children);
  const ScriptNOfK       second(2, children);
  const NativeScript     original(first);

  // Act
  first.setRequired(2);

  // Assert
  EXPECT_EQ(first.required(), 2U);
  EXPECT_EQ(first, second);
  EXPECT_NE(original, NativeScript(second));
  EXPECT_EQ(ScriptNOfK::fromCbor(first.toCbor()), first);
  EXPECT_EQ(ScriptNOfK(0, NativeScriptList()).required(), 0U);
  EXPECT_EQ(ScriptNOfK(std::numeric_limits<std::size_t>::max(), children).required(), std::numeric_limits<std::size_t>::max());
}

TEST(ScriptNOfK, RejectsInvalidThresholdJson)
{
  // Act & Assert
  EXPECT_THROW(ScriptNOfK::fromJson(R"({"type":"atLeast","required":-1,"scripts":[]})"), std::exception);
  EXPECT_THROW(ScriptNOfK::fromJson(R"({"type":"atLeast","required":1.5,"scripts":[]})"), std::exception);
  EXPECT_THROW(ScriptNOfK::fromJson(R"({"type":"atLeast","required":18446744073709551616,"scripts":[]})"), std::exception);
}
