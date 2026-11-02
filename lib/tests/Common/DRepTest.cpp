/**
 * @file DRepTest.cpp
 *
 * @brief Unit tests for delegated representative values.
 *
 * @author angel.castillo
 * @date   Nov 02, 2026
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

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(DRep, MatchesNativeCborFixtures)
{
  // Arrange
  const Credential key(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const Credential script(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '0')));
  const DRep       keyDrep(key);
  const DRep       scriptDrep(script);
  const DRep       abstain(DRepType::Abstain);
  const DRep       noConfidence(DRepType::NoConfidence);
  CborReader       keyCbor("8200581c00000000000000000000000000000000000000000000000000000000");
  CborReader       scriptCbor("8201581c00000000000000000000000000000000000000000000000000000000");
  CborReader       abstainCbor("8102");
  CborReader       noConfidenceCbor("8103");

  // Act & Assert
  EXPECT_EQ(keyDrep.toCbor(), keyCbor.readEncodedValue());
  EXPECT_EQ(scriptDrep.toCbor(), scriptCbor.readEncodedValue());
  EXPECT_EQ(abstain.toCbor(), abstainCbor.readEncodedValue());
  EXPECT_EQ(noConfidence.toCbor(), noConfidenceCbor.readEncodedValue());
  EXPECT_EQ(DRep::fromCbor(keyDrep.toCbor()), keyDrep);
  EXPECT_EQ(DRep::fromCbor(scriptDrep.toCbor()), scriptDrep);
  EXPECT_EQ(DRep::fromCbor(abstain.toCbor()), abstain);
  EXPECT_EQ(DRep::fromCbor(noConfidence.toCbor()), noConfidence);
  EXPECT_EQ(keyDrep.credential().value(), key);
  EXPECT_FALSE(abstain.credential().has_value());
  EXPECT_FALSE(keyDrep.toJson().empty());
}

TEST(DRep, Bech32RoundTripAndCopyIsolation)
{
  // Arrange
  const Credential first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential second(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '2')));
  DRep             drep(first);
  const DRep       original(drep);
  Credential       returned = drep.credential().value();

  // Act
  returned.setHash(Blake2bHash::fromHex(std::string(56, '3')));
  drep.setCredential(second);
  const std::string encoded = drep.toString();

  // Assert
  EXPECT_EQ(original.credential().value(), first);
  EXPECT_EQ(drep.credential().value(), second);
  EXPECT_NE(original, drep);
  EXPECT_EQ(DRep::fromString(encoded), drep);
  EXPECT_THROW(DRep(DRepType::Abstain).toString(), std::logic_error);
}

TEST(DRep, ReplacesTypeAndChecksCredential)
{
  // Arrange
  const Credential          key(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential          script(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  DRep                      drep(key);
  std::vector<std::uint8_t> trailing = drep.toCbor();
  trailing.push_back(0);

  // Act
  drep.setType(DRepType::ScriptHash, script);

  // Assert
  EXPECT_EQ(drep.type(), DRepType::ScriptHash);
  EXPECT_EQ(drep.credential().value(), script);
  EXPECT_THROW(drep.setCredential(key), std::invalid_argument);
  EXPECT_THROW(drep.setType(DRepType::KeyHash), std::invalid_argument);
  EXPECT_THROW(DRep(DRepType::Abstain, key), std::invalid_argument);
  EXPECT_THROW(DRep(static_cast<DRepType>(4)), std::invalid_argument);
  EXPECT_THROW(DRep::fromCbor(trailing), std::invalid_argument);
  drep.setType(DRepType::NoConfidence);
  EXPECT_FALSE(drep.credential().has_value());
}
