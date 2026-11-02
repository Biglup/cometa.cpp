/**
 * @file PlutusV2ScriptTest.cpp
 *
 * @brief Unit tests for PlutusV2Script.
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

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(PlutusV2Script, CopiesCompiledBytesAndEncodesOuterCbor)
{
  // Arrange
  std::vector<std::uint8_t> bytes { 0x45, 0x01, 0x00, 0x00, 0x24, 0x99 };
  const PlutusV2Script      script(bytes);

  // Act
  bytes[0]                         = 0;
  std::vector<std::uint8_t> output = script.toBytes();
  output[0]                        = 0;
  CborReader reader("46450100002499");

  // Assert
  EXPECT_EQ(bytes[0], 0U);
  EXPECT_EQ(output[0], 0U);
  EXPECT_EQ(script.toHex(), "450100002499");
  EXPECT_EQ(script.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(script.languageVersion(), PlutusLanguageVersion::V2);
  EXPECT_EQ(script.hash().toHex(), "b003687053ba29e4e9667d34839714a53138140fe7b929363bd2bb2f");
  EXPECT_EQ(script.toJson(), R"({"language":"plutus_v2","bytes":"450100002499"})");
}

TEST(PlutusV2Script, DecodesBytesHexAndCbor)
{
  // Arrange
  const PlutusV2Script            expected = PlutusV2Script::fromHex("450100002499");
  const std::vector<std::uint8_t> encoded  = expected.toCbor();

  // Act & Assert
  EXPECT_EQ(PlutusV2Script::fromCbor(encoded), expected);
  EXPECT_EQ(PlutusV2Script::fromCbor("46450100002499"), expected);
  EXPECT_EQ(PlutusV2Script::fromHex("AABB"), PlutusV2Script(std::vector<std::uint8_t> { 0xaa, 0xbb }));
  EXPECT_NE(PlutusV2Script::fromHex("01"), expected);
}

TEST(PlutusV2Script, CanonicalizesOuterCborWithoutChangingScriptBytes)
{
  // Arrange
  const PlutusV2Script expected = PlutusV2Script::fromHex("450100002499");

  // Act
  const PlutusV2Script decoded = PlutusV2Script::fromCbor("5806450100002499");

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(decoded.toCbor(), expected.toCbor());
  EXPECT_EQ(decoded.hash(), expected.hash());
}

TEST(PlutusV2Script, RejectsInvalidRepresentations)
{
  // Act & Assert
  EXPECT_THROW(PlutusV2Script(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusV2Script::fromHex(""), std::invalid_argument);
  EXPECT_THROW(PlutusV2Script::fromHex("f"), std::invalid_argument);
  EXPECT_THROW(PlutusV2Script::fromHex("xz"), std::invalid_argument);
  EXPECT_THROW(PlutusV2Script::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(PlutusV2Script::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusV2Script::fromCbor("41"), Error);
  EXPECT_THROW(PlutusV2Script::fromCbor("40"), Error);
  EXPECT_THROW(PlutusV2Script::fromCbor("f5"), Error);
  EXPECT_THROW(PlutusV2Script::fromCbor("414100"), std::invalid_argument);
}

TEST(PlutusV2Script, MatchesSharedParameterApplicationFixtures)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/primitives.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0U;

  // Act & Assert
  for (std::size_t index = 0U; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);
    if (item.at("operation").asString() != "apply_params")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const std::string    original   = item.at("input").at("script").asString();
    const PlutusV2Script script     = PlutusV2Script::fromHex(original);
    const PlutusList     parameters = PlutusList::fromCbor(item.at("input").at("parameters_cbor").asString());
    const PlutusV2Script applied    = script.applyParameters(parameters);

    EXPECT_EQ(applied.toHex(), item.at("expected").at("script").asString());
    EXPECT_EQ(script.toHex(), original);
    EXPECT_EQ(applied.languageVersion(), PlutusLanguageVersion::V2);
    ++checked;
  }

  EXPECT_EQ(checked, 4U);
}

TEST(PlutusV2Script, RejectsMalformedProgramsWhenApplyingParameters)
{
  // Arrange
  const PlutusV2Script script = PlutusV2Script::fromHex("ff");
  const PlutusList     parameters;

  // Act & Assert
  EXPECT_THROW(script.applyParameters(parameters), Error);
  EXPECT_EQ(script.toHex(), "ff");
}

TEST(PlutusV2Script, ReplacesCopiesIndependently)
{
  // Arrange
  const PlutusV2Script source = PlutusV2Script::fromHex("450100002499");
  PlutusV2Script       copy(source);
  EXPECT_EQ(copy, source);

  // Act
  copy = PlutusV2Script::fromHex("01");

  // Assert
  EXPECT_NE(copy, source);
  EXPECT_EQ(source.toHex(), "450100002499");
}
