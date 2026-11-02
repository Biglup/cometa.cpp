/**
 * @file ScriptPubkeyTest.cpp
 *
 * @brief Unit tests for ScriptPubkey.
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

#include <limits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ScriptPubkey, MatchesCborAndCliJson)
{
  // Arrange
  const ScriptPubkey              expected = ScriptPubkey(Blake2bHash::fromHex(std::string(56, '1')));
  const std::string               cbor     = "8200581c" + std::string(56, '1');
  const std::string               json     = "{\"type\":\"sig\",\"keyHash\":\"" + std::string(56, '1') + "\"}";
  CborReader                      reader(cbor);
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ScriptPubkey decoded = ScriptPubkey::fromCbor(cbor);
  const ScriptPubkey parsed  = ScriptPubkey::fromJson(json);

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(ScriptPubkey::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(ScriptPubkey, RejectsInvalidInputAndWrongVariants)
{
  // Arrange
  const std::string cbor = "8200581c" + std::string(56, '1');

  // Act & Assert
  EXPECT_THROW(ScriptPubkey::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromCbor(cbor + "00"), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromCbor("00"), Error);
  EXPECT_THROW(ScriptPubkey::fromJson("{}"), std::exception);
  EXPECT_THROW(ScriptPubkey::fromJson("{\"type\":\"unknown\"}"), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromJson("{"), std::exception);
}

TEST(ScriptPubkey, SupportsIndependentCopyAndMoveOperations)
{
  // Arrange
  const ScriptPubkey original = ScriptPubkey(Blake2bHash::fromHex(std::string(56, '1')));
  ScriptPubkey       copy(original);

  // Act
  ScriptPubkey moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  ScriptPubkey& alias = copy;
  copy                = alias;
  copy                = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(ScriptPubkey, ValidatesHashLengthOnEveryInput)
{
  // Arrange
  const Blake2bHash  invalid = Blake2bHash::fromHex(std::string(64, '0'));
  ScriptPubkey       script(Blake2bHash::fromHex(std::string(56, '1')));
  const ScriptPubkey original(script);

  // Act & Assert
  EXPECT_THROW((void)ScriptPubkey(invalid), std::invalid_argument);
  EXPECT_THROW(script.setKeyHash(invalid), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromCbor("82005820" + std::string(64, '0')), std::invalid_argument);
  EXPECT_THROW(ScriptPubkey::fromJson("{\"type\":\"sig\",\"keyHash\":\"" + std::string(64, '0') + "\"}"), std::invalid_argument);
  EXPECT_EQ(script, original);
}

TEST(ScriptPubkey, MutationUpdatesEncodingAndPreservesCopies)
{
  // Arrange
  const Blake2bHash  first  = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash  second = Blake2bHash::fromHex(std::string(56, '2'));
  ScriptPubkey       script(first);
  const ScriptPubkey original(script);

  // Act
  script.setKeyHash(second);
  Blake2bHash extracted = script.keyHash();

  // Assert
  EXPECT_EQ(script.keyHash(), second);
  EXPECT_EQ(original.keyHash(), first);
  EXPECT_EQ(extracted, second);
  EXPECT_EQ(ScriptPubkey::fromCbor(script.toCbor()), script);
  EXPECT_NE(script, original);
}
