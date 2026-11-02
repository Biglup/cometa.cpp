/**
 * @file ScriptTest.cpp
 *
 * @brief Unit tests for Script.
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

TEST(Script, SupportsEveryLanguageAndRejectsWrongAccessors)
{
  // Arrange
  const NativeScript   native(ScriptInvalidBefore(5));
  const PlutusV1Script v1       = PlutusV1Script::fromHex("450100002499");
  const PlutusV2Script v2       = PlutusV2Script::fromHex("450100002499");
  const PlutusV3Script v3       = PlutusV3Script::fromHex("450100002499");
  const PlutusV4Script v4       = PlutusV4Script::fromHex("450100002499");
  const Script         values[] = { Script(native), Script(v1), Script(v2), Script(v3), Script(v4) };

  // Act & Assert
  EXPECT_EQ(values[0].language(), ScriptLanguage::Native);
  EXPECT_EQ(values[0].asNative(), native);
  EXPECT_EQ(values[0].hash(), native.hash());
  EXPECT_THROW(values[1].asNative(), Error);
  EXPECT_THROW(values[2].asNative(), Error);
  EXPECT_THROW(values[3].asNative(), Error);
  EXPECT_THROW(values[4].asNative(), Error);

  EXPECT_EQ(values[1].language(), ScriptLanguage::PlutusV1);
  EXPECT_EQ(values[1].asPlutusV1(), v1);
  EXPECT_EQ(values[1].hash(), v1.hash());
  EXPECT_THROW(values[0].asPlutusV1(), Error);
  EXPECT_THROW(values[2].asPlutusV1(), Error);
  EXPECT_THROW(values[3].asPlutusV1(), Error);
  EXPECT_THROW(values[4].asPlutusV1(), Error);

  EXPECT_EQ(values[2].language(), ScriptLanguage::PlutusV2);
  EXPECT_EQ(values[2].asPlutusV2(), v2);
  EXPECT_EQ(values[2].hash(), v2.hash());
  EXPECT_THROW(values[0].asPlutusV2(), Error);
  EXPECT_THROW(values[1].asPlutusV2(), Error);
  EXPECT_THROW(values[3].asPlutusV2(), Error);
  EXPECT_THROW(values[4].asPlutusV2(), Error);

  EXPECT_EQ(values[3].language(), ScriptLanguage::PlutusV3);
  EXPECT_EQ(values[3].asPlutusV3(), v3);
  EXPECT_EQ(values[3].hash(), v3.hash());
  EXPECT_THROW(values[0].asPlutusV3(), Error);
  EXPECT_THROW(values[1].asPlutusV3(), Error);
  EXPECT_THROW(values[2].asPlutusV3(), Error);
  EXPECT_THROW(values[4].asPlutusV3(), Error);

  EXPECT_EQ(values[4].language(), ScriptLanguage::PlutusV4);
  EXPECT_EQ(values[4].asPlutusV4(), v4);
  EXPECT_EQ(values[4].hash(), v4.hash());
  EXPECT_THROW(values[0].asPlutusV4(), Error);
  EXPECT_THROW(values[1].asPlutusV4(), Error);
  EXPECT_THROW(values[2].asPlutusV4(), Error);
  EXPECT_THROW(values[3].asPlutusV4(), Error);
}

TEST(Script, CopiesInputAndReturnedVariantsIndependently)
{
  // Arrange
  NativeScript source(ScriptInvalidBefore(5));
  const Script script(source);
  NativeScript component = script.asNative();
  EXPECT_EQ(component, source);

  // Act
  source    = NativeScript(ScriptInvalidBefore(8));
  component = NativeScript(ScriptInvalidAfter(10));

  // Assert
  EXPECT_EQ(script.asNative().asInvalidBefore().slot(), 5U);
  EXPECT_EQ(source.asInvalidBefore().slot(), 8U);
  EXPECT_EQ(component.asInvalidAfter().slot(), 10U);
}

TEST(Script, PreservesNativePayloadEncoding)
{
  // Arrange
  const Script       script = Script::fromCbor("820082041805");
  const NativeScript native = NativeScript::fromCbor("82041805");
  CborReader         expected("820082041805");

  // Act
  NativeScript normalized = script.asNative();
  normalized.normalizeEncoding();

  // Assert
  EXPECT_EQ(script.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(script.hash(), native.hash());
  EXPECT_NE(Script(normalized).hash(), script.hash());
  EXPECT_EQ(script.toJson(), R"({"tag":"native_script","value":{"tag":"timelock_start","slot":"5"}})");
}

TEST(Script, MatchesByteAndHexFactories)
{
  // Arrange
  const Script                    expected(NativeScript(ScriptInvalidAfter(7)));
  const std::vector<std::uint8_t> cbor = expected.toCbor();

  // Act & Assert
  EXPECT_EQ(Script::fromCbor(cbor), expected);
  EXPECT_EQ(Script::fromCbor("8200820507"), expected);
  EXPECT_NE(expected, Script(NativeScript(ScriptInvalidAfter(8))));
}

TEST(Script, RejectsMalformedCborAndInvalidNativeHashes)
{
  // Act & Assert
  EXPECT_THROW(Script::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Script::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(Script::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(Script::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(Script::fromCbor("820082050700"), std::invalid_argument);
  EXPECT_THROW(Script::fromCbor("820082004101"), std::invalid_argument);
  EXPECT_THROW(Script::fromCbor("82054100"), Error);
  EXPECT_THROW(Script::fromCbor("820140"), Error);
}

TEST(Script, MatchesSharedSerializationInspectionAndHashFixtures)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/serialization.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0U;

  // Act & Assert
  for (std::size_t index = 0U; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);
    if (item.at("type").asString() != "script")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const Script script = Script::fromCbor(item.at("input").at("cbor").asString());
    CborReader   expected(item.at("expected").at("cbor").asString());

    EXPECT_EQ(script.toCbor(), expected.readEncodedValue());
    EXPECT_EQ(script.toJson(), item.at("expected").at("inspection").toJson());
    EXPECT_EQ(script.hash().toHex(), item.at("expected").at("hash").asString());
    ++checked;
  }

  EXPECT_EQ(checked, 4U);
}

TEST(Script, RejectsSharedInvalidFixtures)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/invalid.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0U;

  // Act & Assert
  for (std::size_t index = 0U; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);
    if (item.at("type").asString() != "script")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    EXPECT_THROW(Script::fromCbor(item.at("input").at("cbor").asString()), Error);
    ++checked;
  }

  EXPECT_EQ(checked, 2U);
}
