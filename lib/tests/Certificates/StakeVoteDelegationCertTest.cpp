/**
 * @file StakeVoteDelegationCertTest.cpp
 *
 * @brief Unit tests for combined stake and vote delegation certificates.
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

TEST(StakeVoteDelegationCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string cbor =
    "840a8200581c00000000000000000000000000000000000000000000000000000000581c000000000000000000000000000000000000000000000000000000008200581c00000000000000000000000000000000000000000000000000000000";
  const Blake2bHash hash = Blake2bHash::fromHex(std::string(56, '0'));
  const Credential  stake(CredentialType::KeyHash, hash);
  const DRep        drep(stake);
  CborReader        reader(cbor);

  // Act
  const StakeVoteDelegationCert cert(stake, hash, drep);

  // Assert
  EXPECT_EQ(cert.credential(), stake);
  EXPECT_EQ(cert.poolKeyHash(), hash);
  EXPECT_EQ(cert.drep(), drep);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(StakeVoteDelegationCert::fromCbor(cbor), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(StakeVoteDelegationCert, CopiesAndReplacesFields)
{
  // Arrange
  const Blake2bHash             firstHash  = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash             secondHash = Blake2bHash::fromHex(std::string(56, '2'));
  const Credential              first(CredentialType::KeyHash, firstHash);
  const Credential              second(CredentialType::ScriptHash, secondHash);
  const DRep                    firstDRep(first);
  const DRep                    secondDRep(DRepType::Abstain);
  StakeVoteDelegationCert       cert(first, firstHash, firstDRep);
  const StakeVoteDelegationCert original(cert);
  DRep                          returned = cert.drep();

  // Act
  returned.setType(DRepType::NoConfidence);
  cert.setCredential(second);
  cert.setPoolKeyHash(secondHash);
  cert.setDRep(secondDRep);

  // Assert
  EXPECT_EQ(original.credential(), first);
  EXPECT_EQ(original.poolKeyHash(), firstHash);
  EXPECT_EQ(original.drep(), firstDRep);
  EXPECT_EQ(cert.credential(), second);
  EXPECT_EQ(cert.poolKeyHash(), secondHash);
  EXPECT_EQ(cert.drep(), secondDRep);
  EXPECT_NE(cert, original);
  EXPECT_EQ(StakeVoteDelegationCert::fromCbor(cert.toCbor()), cert);
}

TEST(StakeVoteDelegationCert, RejectsInvalidPoolHashAndTrailingCbor)
{
  // Arrange
  const Blake2bHash         hash      = Blake2bHash::fromHex(std::string(56, '0'));
  const Blake2bHash         shortHash = Blake2bHash::fromHex("00");
  const Credential          stake(CredentialType::KeyHash, hash);
  StakeVoteDelegationCert   cert(stake, hash, DRep(DRepType::Abstain));
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(StakeVoteDelegationCert(stake, shortHash, DRep(DRepType::Abstain)), std::invalid_argument);
  EXPECT_THROW(cert.setPoolKeyHash(shortHash), std::invalid_argument);
  EXPECT_THROW(StakeVoteDelegationCert::fromCbor(trailing), std::invalid_argument);
}
