/**
 * @file VoteDelegationCertTest.cpp
 *
 * @brief Unit tests for vote delegation certificates.
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

TEST(VoteDelegationCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string cbor =
    "83098200581c000000000000000000000000000000000000000000000000000000008200581c00000000000000000000000000000000000000000000000000000000";
  const Credential stake(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const DRep       drep(stake);
  CborReader       reader(cbor);

  // Act
  const VoteDelegationCert cert(stake, drep);

  // Assert
  EXPECT_EQ(cert.credential(), stake);
  EXPECT_EQ(cert.drep(), drep);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(VoteDelegationCert::fromCbor(cbor), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(VoteDelegationCert, CopiesAndReplacesFields)
{
  // Arrange
  const Credential         first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential         second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  const DRep               firstDRep(first);
  const DRep               secondDRep(DRepType::Abstain);
  VoteDelegationCert       cert(first, firstDRep);
  const VoteDelegationCert original(cert);
  Credential               returnedCredential = cert.credential();
  DRep                     returnedDRep       = cert.drep();

  // Act
  returnedCredential.setHash(Blake2bHash::fromHex(std::string(56, '3')));
  returnedDRep.setType(DRepType::NoConfidence);
  cert.setCredential(second);
  cert.setDRep(secondDRep);

  // Assert
  EXPECT_EQ(original.credential(), first);
  EXPECT_EQ(original.drep(), firstDRep);
  EXPECT_EQ(cert.credential(), second);
  EXPECT_EQ(cert.drep(), secondDRep);
  EXPECT_NE(cert, original);
  EXPECT_EQ(VoteDelegationCert::fromCbor(cert.toCbor()), cert);
}

TEST(VoteDelegationCert, RejectsTrailingCbor)
{
  // Arrange
  const Credential          stake(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const VoteDelegationCert  cert(stake, DRep(DRepType::Abstain));
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(VoteDelegationCert::fromCbor(trailing), std::invalid_argument);
}
