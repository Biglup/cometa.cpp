/**
 * @file CredentialTest.cpp
 *
 * @brief Unit tests for Credential.
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

#include <Cometa/Credential.h>
#include <Cometa/Error.h>

#include <cardano/common/credential_type.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
const std::string HASH_HEX(56, '0');

Blake2bHash
keyHash()
{
  return Blake2bHash::fromHex(HASH_HEX);
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(Credential, CopyMutationDoesNotChangeOriginal)
{
  // Arrange
  const Credential original(CredentialType::KeyHash, keyHash());
  Credential       copy = original;

  // Act
  copy.setType(CredentialType::ScriptHash);
  copy.setHash(Blake2bHash::fromHex(std::string(56, '1')));

  // Assert
  EXPECT_EQ(original.type(), CredentialType::KeyHash);
  EXPECT_EQ(original.hash().toHex(), HASH_HEX);
  EXPECT_NE(copy, original);
  EXPECT_EQ(Credential::fromCbor(copy.toCbor()), copy);
}

TEST(Credential, SerializesToCbor)
{
  // Arrange
  const Credential value(CredentialType::KeyHash, keyHash());

  // Act
  const std::vector<std::uint8_t> cbor = value.toCbor();

  // Assert
  ASSERT_EQ(cbor.size(), 32);
  EXPECT_EQ(cbor[0], 0x82);
  EXPECT_EQ(cbor[1], 0);
  EXPECT_EQ(cbor[2], 0x58);
  EXPECT_EQ(cbor[3], 28);
}

TEST(Credential, RejectsInvalidValuesWithoutChangingDestination)
{
  // Arrange
  Credential       value(CredentialType::KeyHash, keyHash());
  const Credential before = value;

  // Act & Assert
  EXPECT_THROW(value.setHash(Blake2bHash::compute({ 1 })), std::invalid_argument);
  EXPECT_THROW(value.setType(static_cast<CredentialType>(42)), std::invalid_argument);
  EXPECT_EQ(value, before);
}

TEST(Credential, RejectsMalformedCbor)
{
  // Arrange
  const std::vector<std::uint8_t> cbor { 0x82, 0 };

  // Act & Assert
  EXPECT_THROW(Credential::fromCbor(cbor), Error);
}

TEST(Credential, CopiesSurviveSourceDestruction)
{
  // Arrange
  std::optional<Credential> copy;

  // Act
  {
    const Credential source(CredentialType::KeyHash, keyHash());
    copy.emplace(source);
  }

  // Assert
  EXPECT_EQ(copy->type(), CredentialType::KeyHash);
  EXPECT_EQ(copy->hash().toHex(), HASH_HEX);
}

TEST(Credential, CopyAssignmentIsIndependent)
{
  // Arrange
  Credential source(CredentialType::KeyHash, keyHash());
  Credential assigned(CredentialType::ScriptHash, keyHash());

  // Act
  assigned = source;
  source.setType(CredentialType::ScriptHash);

  // Assert
  EXPECT_EQ(assigned.type(), CredentialType::KeyHash);
}

TEST(Credential, SerializesToJson)
{
  // Arrange
  const Credential value = Credential(CredentialType::KeyHash, keyHash());

  // Act
  const std::string json = value.toJson();

  // Assert
  EXPECT_EQ(json, "{\"tag\":\"pubkey_hash\",\"value\":\"" + HASH_HEX + "\"}");
}

TEST(Credential, ParsesHexCbor)
{
  // Arrange
  const std::string hex      = "8200581c00000000000000000000000000000000000000000000000000000000";
  const Credential  expected = Credential(CredentialType::KeyHash, keyHash());

  // Act
  const Credential decoded = Credential::fromCbor(hex);

  // Assert
  EXPECT_EQ(decoded, expected);
}

TEST(Credential, RejectsInvalidHexCbor)
{
  // Arrange
  const std::string trailing = "8200581c0000000000000000000000000000000000000000000000000000000000";

  // Act & Assert
  EXPECT_THROW(Credential::fromCbor("0"), std::invalid_argument);
  EXPECT_THROW(Credential::fromCbor("gg"), std::invalid_argument);
  EXPECT_THROW(Credential::fromCbor("0x40"), std::invalid_argument);
  EXPECT_THROW(Credential::fromCbor(trailing), std::invalid_argument);
  EXPECT_THROW(Credential::fromCbor("01"), Error);
  EXPECT_THROW(Credential::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Credential::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(Credential, RejectsInvalidConstructorArguments)
{
  // Arrange
  const Blake2bHash hash      = keyHash();
  const Blake2bHash wrongSize = Blake2bHash::compute({ 1 });

  // Act & Assert
  EXPECT_THROW(Credential(static_cast<CredentialType>(42), hash), std::invalid_argument);
  EXPECT_THROW(Credential(CredentialType::KeyHash, wrongSize), std::invalid_argument);
}

TEST(Credential, HashResultSurvivesReplacement)
{
  // Arrange
  Credential        value(CredentialType::KeyHash, keyHash());
  const Blake2bHash hash = value.hash();

  // Act
  value.setHash(Blake2bHash::fromHex(std::string(56, '1')));

  // Assert
  EXPECT_EQ(hash.toHex(), HASH_HEX);
  EXPECT_NE(hash, value.hash());
}

TEST(Credential, ComparesValues)
{
  // Arrange
  const Credential first(CredentialType::KeyHash, keyHash());
  const Credential same(CredentialType::KeyHash, keyHash());
  const Credential second(CredentialType::ScriptHash, keyHash());

  // Act & Assert
  EXPECT_EQ(first, same);
  EXPECT_NE(first, second);
  EXPECT_LT(first, second);
  EXPECT_FALSE(second < first);
}

TEST(CredentialType, MatchesCardanoDiscriminators)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(CredentialType::KeyHash), CARDANO_CREDENTIAL_TYPE_KEY_HASH);
  EXPECT_EQ(static_cast<int>(CredentialType::ScriptHash), CARDANO_CREDENTIAL_TYPE_SCRIPT_HASH);
}
