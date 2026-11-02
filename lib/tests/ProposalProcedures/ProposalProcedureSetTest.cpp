/**
 * @file ProposalProcedureSetTest.cpp
 *
 * @brief Unit tests for proposal procedure collections.
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

TEST(ProposalProcedureSet, MatchesNativeCborAndPreservesOrder)
{
  // Arrange
  const Credential                credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const RewardAddress             address(NetworkId::Testnet, credential);
  const Anchor                    anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  const ProposalProcedure         first(500, address, anchor, InfoAction());
  const ProposalProcedure         second(600, address, anchor, NoConfidenceAction());
  ProposalProcedureSet            set;
  std::vector<std::uint8_t>       expected { 0xd9, 0x01, 0x02, 0x82 };
  const std::vector<std::uint8_t> firstCbor  = first.toCbor();
  const std::vector<std::uint8_t> secondCbor = second.toCbor();
  expected.insert(expected.end(), firstCbor.begin(), firstCbor.end());
  expected.insert(expected.end(), secondCbor.begin(), secondCbor.end());

  // Act
  set.add(first);
  set.add(second);

  // Assert
  EXPECT_EQ(set.size(), 2U);
  EXPECT_EQ(set.toCbor(), expected);
  EXPECT_EQ(set.at(0), first);
  EXPECT_EQ(set.at(1), second);
  EXPECT_EQ(ProposalProcedureSet::fromCbor(expected), set);
  EXPECT_FALSE(set.toJson().empty());
}

TEST(ProposalProcedureSet, CopiesAndRejectsInvalidIndex)
{
  // Arrange
  const Credential        credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const RewardAddress     address(NetworkId::Testnet, credential);
  const Anchor            anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  const ProposalProcedure first(500, address, anchor, InfoAction());
  ProposalProcedureSet    set;
  set.add(first);
  const ProposalProcedureSet copied(set);

  // Act
  set.add(ProposalProcedure(600, address, anchor, InfoAction()));

  // Assert
  EXPECT_EQ(copied.size(), 1U);
  EXPECT_EQ(set.size(), 2U);
  EXPECT_EQ(copied.at(0), first);
  EXPECT_THROW(set.at(2), std::out_of_range);
  EXPECT_EQ(ProposalProcedureSet::fromCbor(copied.toCbor()), copied);
}
