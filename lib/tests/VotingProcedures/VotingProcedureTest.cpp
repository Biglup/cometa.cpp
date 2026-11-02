/**
 * @file VotingProcedureTest.cpp
 *
 * @brief Unit tests for governance voting procedures.
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

TEST(VotingProcedure, MatchesNativeCborWithoutAnchor)
{
  // Arrange
  const VotingProcedure procedure(Vote::Yes);
  CborReader            reader("8201f6");

  // Act
  const VotingProcedure decoded = VotingProcedure::fromCbor("8201f6");

  // Assert
  EXPECT_EQ(procedure.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, procedure);
  EXPECT_EQ(procedure.vote(), Vote::Yes);
  EXPECT_FALSE(procedure.anchor().has_value());
  EXPECT_FALSE(procedure.toJson().empty());
}

TEST(VotingProcedure, CopiesAndSetsFieldsIndependently)
{
  // Arrange
  const Anchor          anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  VotingProcedure       procedure(Vote::No, anchor);
  const VotingProcedure copied(procedure);

  // Act
  procedure.setVote(Vote::Abstain);
  procedure.setAnchor(Anchor("https://b", Blake2bHash::fromHex(std::string(64, '1'))));

  // Assert
  EXPECT_EQ(copied.vote(), Vote::No);
  EXPECT_EQ(copied.anchor(), anchor);
  EXPECT_NE(copied, procedure);
  EXPECT_EQ(VotingProcedure::fromCbor(copied.toCbor()), copied);
}
