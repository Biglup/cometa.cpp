/**
 * @file PlutusV1ScriptTest.cpp
 *
 * @brief Unit tests for PlutusV1Script.
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

TEST(PlutusV1Script, CopiesCompiledBytesAndEncodesOuterCbor)
{
  // Arrange
  std::vector<std::uint8_t> bytes { 0x45, 0x01, 0x00, 0x00, 0x24, 0x99 };
  const PlutusV1Script      script(bytes);

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
  EXPECT_EQ(script.languageVersion(), PlutusLanguageVersion::V1);
  EXPECT_EQ(script.hash().toHex(), "83497f63170b2119e6100fcd767a28894a250f577ba0418b1d43f084");
  EXPECT_EQ(script.toJson(), R"({"language":"plutus_v1","bytes":"450100002499"})");
}

TEST(PlutusV1Script, DecodesBytesHexAndCbor)
{
  // Arrange
  const PlutusV1Script            expected = PlutusV1Script::fromHex("450100002499");
  const std::vector<std::uint8_t> encoded  = expected.toCbor();

  // Act & Assert
  EXPECT_EQ(PlutusV1Script::fromCbor(encoded), expected);
  EXPECT_EQ(PlutusV1Script::fromCbor("46450100002499"), expected);
  EXPECT_EQ(PlutusV1Script::fromHex("AABB"), PlutusV1Script(std::vector<std::uint8_t> { 0xaa, 0xbb }));
  EXPECT_NE(PlutusV1Script::fromHex("01"), expected);
}

TEST(PlutusV1Script, CanonicalizesOuterCborWithoutChangingScriptBytes)
{
  // Arrange
  const PlutusV1Script expected = PlutusV1Script::fromHex("450100002499");

  // Act
  const PlutusV1Script decoded = PlutusV1Script::fromCbor("5806450100002499");

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(decoded.toCbor(), expected.toCbor());
  EXPECT_EQ(decoded.hash(), expected.hash());
}

TEST(PlutusV1Script, RejectsInvalidRepresentations)
{
  // Act & Assert
  EXPECT_THROW(PlutusV1Script(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusV1Script::fromHex(""), std::invalid_argument);
  EXPECT_THROW(PlutusV1Script::fromHex("f"), std::invalid_argument);
  EXPECT_THROW(PlutusV1Script::fromHex("xz"), std::invalid_argument);
  EXPECT_THROW(PlutusV1Script::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(PlutusV1Script::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(PlutusV1Script::fromCbor("41"), Error);
  EXPECT_THROW(PlutusV1Script::fromCbor("40"), Error);
  EXPECT_THROW(PlutusV1Script::fromCbor("f5"), Error);
  EXPECT_THROW(PlutusV1Script::fromCbor("414100"), std::invalid_argument);
}

TEST(PlutusV1Script, MatchesSharedParameterApplicationFixtures)
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
    const PlutusV1Script script     = PlutusV1Script::fromHex(original);
    const PlutusList     parameters = PlutusList::fromCbor(item.at("input").at("parameters_cbor").asString());
    const PlutusV1Script applied    = script.applyParameters(parameters);

    EXPECT_EQ(applied.toHex(), item.at("expected").at("script").asString());
    EXPECT_EQ(script.toHex(), original);
    EXPECT_EQ(applied.languageVersion(), PlutusLanguageVersion::V1);
    ++checked;
  }

  EXPECT_EQ(checked, 4U);
}

TEST(PlutusV1Script, RejectsMalformedProgramsWhenApplyingParameters)
{
  // Arrange
  const PlutusV1Script script = PlutusV1Script::fromHex("ff");
  const PlutusList     parameters;

  // Act & Assert
  EXPECT_THROW(script.applyParameters(parameters), Error);
  EXPECT_EQ(script.toHex(), "ff");
}

TEST(PlutusV1Script, ReplacesCopiesIndependently)
{
  // Arrange
  const PlutusV1Script source = PlutusV1Script::fromHex("450100002499");
  PlutusV1Script       copy(source);
  EXPECT_EQ(copy, source);

  // Act
  copy = PlutusV1Script::fromHex("01");

  // Assert
  EXPECT_NE(copy, source);
  EXPECT_EQ(source.toHex(), "450100002499");
}
