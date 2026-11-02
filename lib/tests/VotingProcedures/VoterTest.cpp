/**
 * @file VoterTest.cpp
 *
 * @brief Unit tests for governance voters.
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

TEST(Voter, MatchesNativeCbor)
{
  // Arrange
  const Credential  credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const Voter       voter(VoterType::ConstitutionalCommitteeKeyHash, credential);
  const std::string cbor = "8200581c" + std::string(56, '0');
  CborReader        reader(cbor);

  // Act
  const Voter decoded = Voter::fromCbor(cbor);

  // Assert
  EXPECT_EQ(voter.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, voter);
  EXPECT_EQ(voter.type(), VoterType::ConstitutionalCommitteeKeyHash);
  EXPECT_EQ(voter.credential(), credential);
  EXPECT_FALSE(voter.toJson().empty());
}

TEST(Voter, CopiesAndOrdersIndependently)
{
  // Arrange
  const Credential original(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const Credential replacement(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  Voter            voter(VoterType::ConstitutionalCommitteeKeyHash, original);
  const Voter      copied(voter);

  // Act
  voter.setType(VoterType::DRepKeyHash);
  voter.setCredential(replacement);

  // Assert
  EXPECT_EQ(copied.type(), VoterType::ConstitutionalCommitteeKeyHash);
  EXPECT_EQ(copied.credential(), original);
  EXPECT_NE(voter, copied);
  EXPECT_TRUE(copied < voter);
  EXPECT_EQ(Voter::fromCbor(voter.toCbor()), voter);
}
