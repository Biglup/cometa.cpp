/**
 * @file VoterTypeTest.cpp
 *
 * @brief Tests for governance voter discriminators.
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

#include <cardano/voting_procedures/voter_type.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(VoterType, MatchesNativeDiscriminatorsAndNames)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(VoterType::ConstitutionalCommitteeKeyHash), CARDANO_VOTER_TYPE_CONSTITUTIONAL_COMMITTEE_KEY_HASH);
  EXPECT_EQ(toString(VoterType::ConstitutionalCommitteeKeyHash), cardano_voter_type_to_string(CARDANO_VOTER_TYPE_CONSTITUTIONAL_COMMITTEE_KEY_HASH));
  EXPECT_EQ(static_cast<int>(VoterType::ConstitutionalCommitteeScriptHash), CARDANO_VOTER_TYPE_CONSTITUTIONAL_COMMITTEE_SCRIPT_HASH);
  EXPECT_EQ(toString(VoterType::ConstitutionalCommitteeScriptHash), cardano_voter_type_to_string(CARDANO_VOTER_TYPE_CONSTITUTIONAL_COMMITTEE_SCRIPT_HASH));
  EXPECT_EQ(static_cast<int>(VoterType::DRepKeyHash), CARDANO_VOTER_TYPE_DREP_KEY_HASH);
  EXPECT_EQ(toString(VoterType::DRepKeyHash), cardano_voter_type_to_string(CARDANO_VOTER_TYPE_DREP_KEY_HASH));
  EXPECT_EQ(static_cast<int>(VoterType::DRepScriptHash), CARDANO_VOTER_TYPE_DREP_SCRIPT_HASH);
  EXPECT_EQ(toString(VoterType::DRepScriptHash), cardano_voter_type_to_string(CARDANO_VOTER_TYPE_DREP_SCRIPT_HASH));
  EXPECT_EQ(static_cast<int>(VoterType::StakePoolKeyHash), CARDANO_VOTER_TYPE_STAKE_POOL_KEY_HASH);
  EXPECT_EQ(toString(VoterType::StakePoolKeyHash), cardano_voter_type_to_string(CARDANO_VOTER_TYPE_STAKE_POOL_KEY_HASH));
}
