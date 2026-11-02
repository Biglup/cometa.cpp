/**
 * @file VoteTest.cpp
 *
 * @brief Tests for voting choices.
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

#include <cardano/voting_procedures/vote.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Vote, MatchesNativeDiscriminatorsAndNames)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Vote::No), CARDANO_VOTE_NO);
  EXPECT_EQ(toString(Vote::No), cardano_vote_to_string(CARDANO_VOTE_NO));
  EXPECT_EQ(static_cast<int>(Vote::Yes), CARDANO_VOTE_YES);
  EXPECT_EQ(toString(Vote::Yes), cardano_vote_to_string(CARDANO_VOTE_YES));
  EXPECT_EQ(static_cast<int>(Vote::Abstain), CARDANO_VOTE_ABSTAIN);
  EXPECT_EQ(toString(Vote::Abstain), cardano_vote_to_string(CARDANO_VOTE_ABSTAIN));
}
