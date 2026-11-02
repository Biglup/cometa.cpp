/**
 * @file ProposalProcedureTest.cpp
 *
 * @brief Unit tests for governance proposal procedures.
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

TEST(ProposalProcedure, WrapsAllGovernanceActionKinds)
{
  // Arrange
  const Credential                credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const RewardAddress             address(NetworkId::Testnet, credential);
  const Anchor                    anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  const ParameterChangeAction     parameter { ProtocolParamUpdate() };
  const HardForkInitiationAction  hardFork(ProtocolVersion(12, 0));
  const TreasuryWithdrawalsAction treasury { WithdrawalMap() };
  const NoConfidenceAction        noConfidence;
  const UpdateCommitteeAction     committee { CredentialSet(), CommitteeMembersMap(), UnitInterval(1, 2) };
  const NewConstitutionAction     constitution { Constitution(anchor) };
  const InfoAction                info;

  // Act
  const ProposalProcedure parameterProposal(500, address, anchor, parameter);
  const ProposalProcedure hardForkProposal(500, address, anchor, hardFork);
  const ProposalProcedure treasuryProposal(500, address, anchor, treasury);
  const ProposalProcedure noConfidenceProposal(500, address, anchor, noConfidence);
  const ProposalProcedure committeeProposal(500, address, anchor, committee);
  const ProposalProcedure constitutionProposal(500, address, anchor, constitution);
  const ProposalProcedure infoProposal(500, address, anchor, info);

  // Assert
  EXPECT_EQ(parameterProposal.type(), GovernanceActionType::ParameterChange);
  EXPECT_EQ(parameterProposal.asParameterChangeAction(), parameter);
  EXPECT_EQ(hardForkProposal.type(), GovernanceActionType::HardForkInitiation);
  EXPECT_EQ(hardForkProposal.asHardForkInitiationAction(), hardFork);
  EXPECT_EQ(treasuryProposal.type(), GovernanceActionType::TreasuryWithdrawals);
  EXPECT_EQ(treasuryProposal.asTreasuryWithdrawalsAction(), treasury);
  EXPECT_EQ(noConfidenceProposal.type(), GovernanceActionType::NoConfidence);
  EXPECT_EQ(noConfidenceProposal.asNoConfidenceAction(), noConfidence);
  EXPECT_EQ(committeeProposal.type(), GovernanceActionType::UpdateCommittee);
  EXPECT_EQ(committeeProposal.asUpdateCommitteeAction(), committee);
  EXPECT_EQ(constitutionProposal.type(), GovernanceActionType::NewConstitution);
  EXPECT_EQ(constitutionProposal.asNewConstitutionAction(), constitution);
  EXPECT_EQ(infoProposal.type(), GovernanceActionType::Info);
  EXPECT_EQ(infoProposal.asInfoAction(), info);
  EXPECT_THROW(infoProposal.asParameterChangeAction(), std::logic_error);
  EXPECT_EQ(infoProposal.deposit(), 500U);
  EXPECT_EQ(infoProposal.rewardAddress(), address);
  EXPECT_EQ(infoProposal.anchor(), anchor);
  EXPECT_EQ(ProposalProcedure::fromCbor(infoProposal.toCbor()), infoProposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(parameterProposal.toCbor()), parameterProposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(hardForkProposal.toCbor()), hardForkProposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(treasuryProposal.toCbor()), treasuryProposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(noConfidenceProposal.toCbor()), noConfidenceProposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(committeeProposal.toCbor()), committeeProposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(constitutionProposal.toCbor()), constitutionProposal);
  EXPECT_FALSE(infoProposal.toJson().empty());
}

TEST(ProposalProcedure, CopiesFieldsIndependently)
{
  // Arrange
  const Credential        credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const RewardAddress     address(NetworkId::Testnet, credential);
  const Anchor            anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  ProposalProcedure       proposal(500, address, anchor, InfoAction());
  const ProposalProcedure copied(proposal);
  const RewardAddress     another(NetworkId::Mainnet, credential);

  // Act
  proposal.setDeposit(600);
  proposal.setRewardAddress(another);
  proposal.setAnchor(anchor);

  // Assert
  EXPECT_EQ(copied.deposit(), 500U);
  EXPECT_EQ(copied.rewardAddress(), address);
  EXPECT_EQ(copied.anchor(), anchor);
  EXPECT_NE(copied, proposal);
  EXPECT_EQ(ProposalProcedure::fromCbor(copied.toCbor()), copied);
}
