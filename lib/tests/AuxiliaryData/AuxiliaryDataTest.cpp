/**
 * @file AuxiliaryDataTest.cpp
 *
 * @brief Unit tests for AuxiliaryData.
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

TEST(AuxiliaryData, DistinguishesAbsentAndEmptyFields)
{
  // Arrange
  AuxiliaryData value;
  CborReader    empty("d90103a0");

  // Act & Assert
  EXPECT_FALSE(value.metadata().has_value());
  EXPECT_FALSE(value.nativeScripts().has_value());
  EXPECT_FALSE(value.plutusV1Scripts().has_value());
  EXPECT_FALSE(value.plutusV2Scripts().has_value());
  EXPECT_FALSE(value.plutusV3Scripts().has_value());
  EXPECT_FALSE(value.plutusV4Scripts().has_value());
  EXPECT_EQ(value.toCbor(), empty.readEncodedValue());
  EXPECT_EQ(value.toJson(), "{}");

  // Act
  value.setMetadata(TransactionMetadata());
  value.setPlutusV4Scripts(PlutusV4ScriptList());

  // Assert
  ASSERT_TRUE(value.metadata().has_value());
  ASSERT_TRUE(value.plutusV4Scripts().has_value());
  EXPECT_TRUE(value.metadata()->empty());
  EXPECT_TRUE(value.plutusV4Scripts()->empty());

  // Act
  value.setMetadata(std::nullopt);
  value.setPlutusV4Scripts(std::nullopt);

  // Assert
  EXPECT_EQ(value, AuxiliaryData());
}

TEST(AuxiliaryData, OwnsIndependentMetadataAndScripts)
{
  // Arrange
  TransactionMetadata metadata;
  metadata.set(1, Metadatum("first"));
  NativeScriptList scripts { NativeScript(ScriptInvalidBefore(5)) };
  AuxiliaryData    value;
  value.setMetadata(metadata);
  value.setNativeScripts(scripts);

  // Act
  metadata.set(1, Metadatum("changed"));
  scripts.pushBack(NativeScript(ScriptInvalidAfter(10)));
  TransactionMetadata returnedMetadata = *value.metadata();
  returnedMetadata.set(2, Metadatum("new"));
  NativeScriptList returnedScripts = *value.nativeScripts();
  returnedScripts.pushBack(NativeScript(ScriptInvalidAfter(20)));

  // Assert
  EXPECT_EQ(value.metadata()->at(1).asText(), "first");
  EXPECT_EQ(value.metadata()->size(), 1U);
  EXPECT_EQ(value.nativeScripts()->size(), 1U);
  EXPECT_EQ(metadata.at(1).asText(), "changed");
  EXPECT_EQ(scripts.size(), 2U);
  EXPECT_EQ(returnedMetadata.size(), 2U);
  EXPECT_EQ(returnedScripts.size(), 2U);
}

TEST(AuxiliaryData, IncludesAllPlutusVersionsInInspection)
{
  // Arrange
  AuxiliaryData            value;
  const PlutusV1ScriptList v1 { PlutusV1Script::fromHex("450100002499") };
  const PlutusV2ScriptList v2 { PlutusV2Script::fromHex("450100002499") };
  const PlutusV3ScriptList v3 { PlutusV3Script::fromHex("450100002499") };
  const PlutusV4ScriptList v4 { PlutusV4Script::fromHex("450100002499") };

  // Act
  value.setPlutusV1Scripts(v1);
  value.setPlutusV2Scripts(v2);
  value.setPlutusV3Scripts(v3);
  value.setPlutusV4Scripts(v4);
  const JsonValue inspection = JsonValue::parse(value.toJson());

  // Assert
  EXPECT_EQ(value.plutusV1Scripts(), v1);
  EXPECT_EQ(value.plutusV2Scripts(), v2);
  EXPECT_EQ(value.plutusV3Scripts(), v3);
  EXPECT_EQ(value.plutusV4Scripts(), v4);
  EXPECT_EQ(inspection.at("plutus_scripts").size(), 4U);
  EXPECT_EQ(inspection.at("plutus_scripts").at(3).at("language").asString(), "plutus_v4");
  EXPECT_EQ(AuxiliaryData::fromCbor(value.toCbor()), value);

  // Act
  value.setPlutusV1Scripts(std::nullopt);
  value.setPlutusV2Scripts(std::nullopt);
  value.setPlutusV3Scripts(std::nullopt);
  value.setPlutusV4Scripts(std::nullopt);
  value.setNativeScripts(std::nullopt);

  // Assert
  EXPECT_EQ(value, AuxiliaryData());
}

TEST(AuxiliaryData, EditsReplaceOriginalSerializationAndHash)
{
  // Arrange
  AuxiliaryData     value   = AuxiliaryData::fromCbor("a101636f6c64");
  const Blake2bHash oldHash = value.hash();
  CborReader        original("a101636f6c64");
  CborReader        changed("d90103a100a101636e6577");
  EXPECT_EQ(value.toCbor(), original.readEncodedValue());
  TransactionMetadata metadata = *value.metadata();

  // Act
  metadata.set(1, Metadatum("new"));
  value.setMetadata(metadata);

  // Assert
  EXPECT_EQ(value.toCbor(), changed.readEncodedValue());
  EXPECT_NE(value.hash(), oldHash);
  EXPECT_EQ(value.hash(), Blake2bHash::compute(value.toCbor()));
  EXPECT_EQ(AuxiliaryData::fromCbor(value.toCbor()).metadata()->at(1).asText(), "new");

  // Act
  value.setMetadata(std::nullopt);

  // Assert
  EXPECT_EQ(value.toCbor(), AuxiliaryData().toCbor());
}

TEST(AuxiliaryData, RejectsDuplicateAndInvalidFields)
{
  // Act & Assert
  EXPECT_THROW(AuxiliaryData::fromCbor("d90103a200a000a0"), Error);
  EXPECT_THROW(AuxiliaryData::fromCbor("d90103a205800580"), Error);
  EXPECT_THROW(AuxiliaryData::fromCbor("d90103a10680"), Error);
  EXPECT_THROW(AuxiliaryData::fromCbor("d90102a0"), Error);
  EXPECT_THROW(AuxiliaryData::fromCbor("d90103a100a200000001"), std::invalid_argument);
  EXPECT_THROW(AuxiliaryData::fromCbor("a000"), std::invalid_argument);
  EXPECT_THROW(AuxiliaryData::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(AuxiliaryData::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(AuxiliaryData::fromCbor("xx"), std::invalid_argument);
  EXPECT_THROW(AuxiliaryData::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(AuxiliaryData, MatchesNativeEncodingAndHashVectors)
{
  // Arrange
  const std::vector<std::pair<std::string, std::string>> vectors {
    { "d90103a500a11902d5a4187b1904d2636b65796576616c7565646b65793246000102030405a1190237656569676874a119029a6463616b6501848204038205098202818200581c3542acb3a64d80c29302260d62c3b87a742ad14abf855ebc6733081e830300818200581cb5ae663aaea8e500157bdf4baafd6f5ba0ce5759f7cd4101fc132f5402844746010000220010474601000022001147460100002200124746010000220013038447460100002200104746010000220011474601000022001247460100002200130483474601000022001047460100002200114746010000220012", "d24e84d8dbf6f880b04f64ad919bb618bf66ce834b3c901b1efe2ce6b44beb7b" },
    { "82a11902d5a4187b1904d2636b65796576616c7565646b65793246000102030405a1190237656569676874a119029a6463616b65828202818200581c3542acb3a64d80c29302260d62c3b87a742ad14abf855ebc6733081e830300818200581cb5ae663aaea8e500157bdf4baafd6f5ba0ce5759f7cd4101fc132f54", "a02cace10f1fc93061cd0dcc31ccfafb9599eba245ae3f03a2ee69928f73d3ed" },
    { "a11902d5a4187b1904d2636b65796576616c7565646b65793246000102030405a1190237656569676874a119029a6463616b65", "3bed6c134ce51ea7cfccec5ae44acbcb995b568c6408f2a1302f0e1c76d4ae63" }
  };

  for (const std::pair<std::string, std::string>& vector: vectors)
  {
    CborReader expected(vector.first);

    // Act
    AuxiliaryData value = AuxiliaryData::fromCbor(vector.first);

    // Assert
    EXPECT_EQ(value.toCbor(), expected.readEncodedValue());
    EXPECT_EQ(value.hash().toHex(), vector.second);
    const AuxiliaryData copy(value);
    value.normalizeEncoding();
    EXPECT_EQ(value, copy);
    EXPECT_EQ(copy.hash().toHex(), vector.second);
    EXPECT_EQ(AuxiliaryData::fromCbor(value.toCbor()), value);
  }
}
