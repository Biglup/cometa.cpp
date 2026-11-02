/**
 * @file NativeScriptTest.cpp
 *
 * @brief Unit tests for NativeScript.
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

TEST(NativeScript, MatchesCborAndCliJson)
{
  // Arrange
  const NativeScript              expected = NativeScript(ScriptInvalidBefore(5));
  CborReader                      reader("820405");
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const NativeScript decoded = NativeScript::fromCbor("820405");
  const NativeScript parsed  = NativeScript::fromJson(R"({"type":"after","slot":5})");

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(NativeScript::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(NativeScript, RejectsInvalidInput)
{
  // Act & Assert
  EXPECT_THROW(NativeScript::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromCbor("82040500"), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromCbor("f5"), Error);
  EXPECT_THROW(NativeScript::fromJson("{}"), std::exception);
  EXPECT_THROW(NativeScript::fromJson("{"), std::exception);
}

TEST(NativeScript, SupportsEveryVariantAndRejectsWrongAccessors)
{
  // Arrange
  const Blake2bHash      hash = Blake2bHash::fromHex(std::string(56, '1'));
  const Credential       credential(CredentialType::KeyHash, hash);
  const NativeScriptList children { NativeScript(ScriptPubkey(hash)) };

  // Act & Assert
  EXPECT_EQ(NativeScript(ScriptPubkey(hash)).asPubkey().keyHash(), hash);
  EXPECT_EQ(NativeScript(ScriptAll(children)).asAll().size(), 1U);
  EXPECT_EQ(NativeScript(ScriptAny(children)).asAny().size(), 1U);
  EXPECT_EQ(NativeScript(ScriptNOfK(1, children)).asNOfK().required(), 1U);
  EXPECT_EQ(NativeScript(ScriptInvalidBefore(5)).asInvalidBefore().slot(), 5U);
  EXPECT_EQ(NativeScript(ScriptInvalidAfter(7)).asInvalidAfter().slot(), 7U);
  EXPECT_EQ(NativeScript(ScriptRequireGuard(credential)).asRequireGuard().credential(), credential);
  EXPECT_THROW(NativeScript(ScriptAll(children)).asPubkey(), Error);
  EXPECT_THROW(NativeScript::fromJson(R"({"type":"unknown"})"), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromJson(R"({"type":"after\u0000bad","slot":1})"), std::invalid_argument);
}

TEST(NativeScript, DoesNotExposeMutableChildren)
{
  // Arrange
  const NativeScript original     = NativeScript::fromCbor("8201828204180582051807");
  const Blake2bHash  originalHash = original.hash();

  // Act
  ScriptAll        all      = original.asAll();
  NativeScriptList children = all.scripts();
  children.pushBack(NativeScript(ScriptInvalidBefore(9)));
  all.setScripts(children);
  NativeScript changed(all);
  changed.normalizeEncoding();

  // Assert
  EXPECT_EQ(original.asAll().size(), 2U);
  EXPECT_EQ(original.hash(), originalHash);
  EXPECT_EQ(changed.asAll().size(), 3U);
  EXPECT_NE(changed.hash(), originalHash);
}

TEST(NativeScript, RejectsInvalidHashesAtEveryNestingLevel)
{
  // Arrange
  const std::string invalid = "82005820" + std::string(64, '1');

  // Act & Assert
  EXPECT_THROW(NativeScript::fromCbor(invalid), std::invalid_argument);
  EXPECT_THROW(NativeScript::fromCbor("820181" + invalid), std::invalid_argument);
  EXPECT_THROW(NativeScriptList::fromCbor("81" + invalid), std::invalid_argument);
  EXPECT_THROW(ScriptAll::fromCbor("820181" + invalid), std::invalid_argument);
  EXPECT_THROW(ScriptAny::fromCbor("820281" + invalid), std::invalid_argument);
  EXPECT_THROW(ScriptNOfK::fromCbor("83030181" + invalid), std::invalid_argument);
}

TEST(NativeScript, MatchesSharedCborInspectionAndHashFixtures)
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
    if (item.at("type").asString() != "native_script")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const NativeScript script = NativeScript::fromCbor(item.at("input").at("cbor").asString());
    CborReader         encoded(item.at("expected").at("cbor").asString());

    EXPECT_EQ(script.toCbor(), encoded.readEncodedValue());
    EXPECT_EQ(script.toJson(), item.at("expected").at("inspection").toJson());
    EXPECT_EQ(script.hash().toHex(), item.at("expected").at("hash").asString());
    ++checked;
  }

  EXPECT_EQ(checked, 18U);
}

TEST(NativeScript, RejectsSharedMalformedFixtures)
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
    if (item.at("type").asString() != "native_script")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    EXPECT_THROW(NativeScript::fromCbor(item.at("input").at("cbor").asString()), Error);
    ++checked;
  }

  EXPECT_EQ(checked, 2U);
}
