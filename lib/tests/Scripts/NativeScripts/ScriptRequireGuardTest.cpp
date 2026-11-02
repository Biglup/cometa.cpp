/**
 * @file ScriptRequireGuardTest.cpp
 *
 * @brief Unit tests for ScriptRequireGuard.
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

TEST(ScriptRequireGuard, MatchesCborAndCliJson)
{
  // Arrange
  const ScriptRequireGuard        expected = ScriptRequireGuard(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
  const std::string               cbor     = "82068200581c" + std::string(56, '1');
  const std::string               json     = "{\"type\":\"guard\",\"keyHash\":\"" + std::string(56, '1') + "\"}";
  CborReader                      reader(cbor);
  const std::vector<std::uint8_t> bytes = reader.readEncodedValue();

  // Act
  const ScriptRequireGuard decoded = ScriptRequireGuard::fromCbor(cbor);
  const ScriptRequireGuard parsed  = ScriptRequireGuard::fromJson(json);

  // Assert
  EXPECT_EQ(decoded, expected);
  EXPECT_EQ(parsed, expected);
  EXPECT_EQ(ScriptRequireGuard::fromCbor(bytes), expected);
  EXPECT_EQ(decoded.toCbor(), bytes);
  EXPECT_EQ(decoded.toJson(), expected.toJson());
}

TEST(ScriptRequireGuard, RejectsInvalidInputAndWrongVariants)
{
  // Arrange
  const std::string cbor = "82068200581c" + std::string(56, '1');

  // Act & Assert
  EXPECT_THROW(ScriptRequireGuard::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromCbor(cbor + "00"), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromCbor("00"), Error);
  EXPECT_THROW(ScriptRequireGuard::fromJson("{}"), std::exception);
  EXPECT_THROW(ScriptRequireGuard::fromJson("{\"type\":\"unknown\"}"), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromJson("{"), std::exception);
}

TEST(ScriptRequireGuard, SupportsIndependentCopyAndMoveOperations)
{
  // Arrange
  const ScriptRequireGuard original = ScriptRequireGuard(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
  ScriptRequireGuard       copy(original);

  // Act
  ScriptRequireGuard moved(std::move(copy));
  copy = original;
  swap(copy, moved);
  ScriptRequireGuard& alias = copy;
  copy                      = alias;
  copy                      = std::move(alias);

  // Assert
  EXPECT_EQ(copy, original);
  EXPECT_EQ(moved, original);
  EXPECT_FALSE(copy != original);
}

TEST(ScriptRequireGuard, CopiesMutableCredentialsAndUpdatesEncoding)
{
  // Arrange
  Credential               credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  ScriptRequireGuard       guard(credential);
  const ScriptRequireGuard original(guard);

  // Act
  credential.setType(CredentialType::ScriptHash);
  guard.setCredential(credential);
  credential.setHash(Blake2bHash::fromHex(std::string(56, '2')));
  Credential extracted = guard.credential();
  extracted.setType(CredentialType::KeyHash);

  // Assert
  EXPECT_EQ(guard.credential().type(), CredentialType::ScriptHash);
  EXPECT_EQ(guard.credential().hash(), Blake2bHash::fromHex(std::string(56, '1')));
  EXPECT_EQ(original.credential().type(), CredentialType::KeyHash);
  EXPECT_EQ(extracted.type(), CredentialType::KeyHash);
  EXPECT_EQ(ScriptRequireGuard::fromCbor(guard.toCbor()), guard);
}

TEST(ScriptRequireGuard, AcceptsScriptCredentialsAndRejectsAmbiguousJson)
{
  // Arrange
  const std::string hash(56, '1');
  const std::string json = "{\"type\":\"guard\",\"scriptHash\":\"" + hash + "\"}";

  // Act
  const ScriptRequireGuard guard = ScriptRequireGuard::fromJson(json);

  // Assert
  EXPECT_EQ(guard.credential().type(), CredentialType::ScriptHash);
  EXPECT_THROW(ScriptRequireGuard::fromJson("{\"type\":\"guard\",\"scriptHash\":\"" + hash + "\",\"keyHash\":\"" + hash + "\"}"), std::invalid_argument);
  EXPECT_THROW(ScriptRequireGuard::fromJson("{\"type\":\"guard\"}"), std::invalid_argument);
  EXPECT_NE(guard.toJson().find("require_guard"), std::string::npos);
}
