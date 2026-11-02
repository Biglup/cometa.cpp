/**
 * @file VotingProceduresTest.cpp
 *
 * @brief Unit tests for governance voting maps.
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

TEST(VotingProcedures, MatchesNativeCborAndEnumeratesVotes)
{
  // Arrange
  const Voter              voter(VoterType::ConstitutionalCommitteeKeyHash, Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0'))));
  const GovernanceActionId actionId(Blake2bHash::fromHex(std::string(64, '1')), 0);
  const VotingProcedure    vote(Vote::Yes);
  VotingProcedures         procedures;
  const std::string        cbor = "a18200581c" + std::string(56, '0') + "a1825820" + std::string(64, '1') + "008201f6";
  CborReader               reader(cbor);

  // Act
  procedures.insert(voter, actionId, vote);
  const VotingProcedures decoded = VotingProcedures::fromCbor(cbor);

  // Assert
  EXPECT_EQ(procedures.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, procedures);
  EXPECT_EQ(procedures.get(voter, actionId), vote);
  EXPECT_EQ(procedures.voters(), VoterList { voter });
  EXPECT_EQ(procedures.actionIds(voter), GovernanceActionIdList { actionId });
  EXPECT_FALSE(procedures.toJson().empty());
}

TEST(VotingProcedures, CopiesAndAddsIndependently)
{
  // Arrange
  const Voter              voter(VoterType::DRepKeyHash, Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0'))));
  const GovernanceActionId actionId(Blake2bHash::fromHex(std::string(64, '1')), 3);
  VotingProcedures         procedures;
  procedures.insert(voter, actionId, VotingProcedure(Vote::No));
  const VotingProcedures copy(procedures);

  // Act
  const GovernanceActionId secondId(actionId.hash(), 4);
  procedures.insert(voter, secondId, VotingProcedure(Vote::Abstain));

  // Assert
  EXPECT_EQ(copy.get(voter, actionId)->vote(), Vote::No);
  EXPECT_EQ(procedures.get(voter, secondId)->vote(), Vote::Abstain);
  EXPECT_NE(copy, procedures);
  EXPECT_FALSE(copy.get(voter, secondId).has_value());
  EXPECT_THROW(procedures.insert(voter, actionId, VotingProcedure(Vote::Yes)), std::invalid_argument);
}

TEST(VotingProcedures, ReturnsEmptyCollectionsForMissingVoter)
{
  // Arrange
  const Voter      voter(VoterType::DRepKeyHash, Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0'))));
  VotingProcedures procedures;

  // Act
  const VoterList              voters = procedures.voters();
  const GovernanceActionIdList ids    = procedures.actionIds(voter);

  // Assert
  EXPECT_TRUE(voters.empty());
  EXPECT_TRUE(ids.empty());
}
