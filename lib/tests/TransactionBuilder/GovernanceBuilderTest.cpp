/**
 * @file GovernanceBuilderTest.cpp
 *
 * @brief Tests standalone transaction operations and governance construction.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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
#include <Cometa/TransactionBuilder/SubTransactionBuilder.h>
#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Builds the seven governance proposals. @tparam Builder The builder type. @param builder The builder. @param predecessor Whether to include a predecessor. @return The independently constructed expected proposals. */
template<class Builder>
ProposalProcedureSet
addAllProposals(Builder& builder, bool predecessor)
{
  const Credential                        credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 4)));
  const RewardAddress                     reward(NetworkId::Testnet, credential);
  const Anchor                            anchor("https://example.com/action", Blake2bHash(std::vector<std::uint8_t>(32, 5)));
  const GovernanceActionId                id(Blake2bHash(std::vector<std::uint8_t>(32, 6)), 3);
  const std::optional<GovernanceActionId> previous = predecessor ? std::optional<GovernanceActionId>(id) : std::nullopt;
  ProtocolParamUpdate                     update;
  update.setMinFeeA(47);

  WithdrawalMap withdrawals;
  withdrawals.set(reward, 2'000'000);
  const CredentialSet removed { credential };
  CommitteeMembersMap added;
  added.insert(Credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 8))), 500);
  const UnitInterval    quorum(2, 3);
  const Constitution    constitution(anchor);
  const ProtocolVersion version(12, 1);

  const Script      policy(PlutusV4Script::fromHex("450100002499"));
  const Blake2bHash policyHash = policy.hash();

  builder.addScript(policy);

  builder.proposeParameterChange(reward, anchor, update, policyHash, previous)
    .proposeHardFork(reward, anchor, version, previous)
    .proposeTreasuryWithdrawals(reward, anchor, withdrawals, policyHash)
    .proposeNoConfidence(reward, anchor, previous)
    .proposeUpdateCommittee(reward, anchor, removed, added, quorum, previous)
    .proposeNewConstitution(reward, anchor, constitution, previous)
    .proposeInfo(reward, anchor);

  return ProposalProcedureSet {
    ProposalProcedure(1'000'000, reward, anchor, ParameterChangeAction(update, previous, policyHash)),
    ProposalProcedure(1'000'000, reward, anchor, HardForkInitiationAction(version, previous)),
    ProposalProcedure(1'000'000, reward, anchor, TreasuryWithdrawalsAction(withdrawals, policyHash)),
    ProposalProcedure(1'000'000, reward, anchor, NoConfidenceAction(previous)),
    ProposalProcedure(1'000'000, reward, anchor, UpdateCommitteeAction(removed, added, quorum, previous)),
    ProposalProcedure(1'000'000, reward, anchor, NewConstitutionAction(constitution, previous)),
    ProposalProcedure(1'000'000, reward, anchor, InfoAction())
  };
}

/** @brief Supplies the presence or absence of predecessor IDs. */
class GovernanceBuilder : public testing::TestWithParam<bool>
{
};

/** @brief Creates protocol parameters for deterministic proposal tests. @return The parameters. */
ProtocolParameters
parameters()
{
  ProtocolParameters result;
  result.setProtocolVersion(ProtocolVersion(12, 0));
  result.setMinFeeA(44);
  result.setMinFeeB(155381);
  result.setAdaPerUtxoByte(4310);
  result.setMaxTxSize(65536);
  result.setGovernanceActionDeposit(1'000'000);

  CostModels models;
  models.insert(CostModel(PlutusLanguageVersion::V4, std::vector<std::int64_t>(179, 0)));
  result.setCostModels(models);

  return result;
}

/** @brief Creates a funded payment output. @return The UTxO. */
Utxo
funding()
{
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address    address = EnterpriseAddress(NetworkId::Testnet, credential);

  return Utxo(TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0), TransactionOutput(address, Value(20'000'000)));
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST_P(GovernanceBuilder, BuildsEveryTopLevelProposal)
{
  // Arrange
  const Utxo         input = funding();
  TransactionBuilder builder(parameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));

  const Utxo collateral(TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 9)), 0), input.output());

  builder.setUtxos({ input })
    .setChangeAddress(input.output().address())
    .setCollateralUtxos({ collateral })
    .setCollateralChangeAddress(input.output().address());
  const ProposalProcedureSet expected = addAllProposals(builder, GetParam());

  // Act
  const Transaction transaction = builder.build();

  // Assert
  ASSERT_TRUE(transaction.body().proposalProcedures());
  EXPECT_EQ(transaction.body().proposalProcedures()->toCbor(), expected.toCbor());
  EXPECT_EQ(computeImplicitCoin(transaction, parameters()).deposits, 7'000'000U);
  EXPECT_TRUE(isTransactionBalanced(transaction, { input }, parameters()));
}

TEST_P(GovernanceBuilder, BuildsEverySubTransactionProposalAndDonation)
{
  // Arrange
  const Utxo            input = funding();
  SubTransactionBuilder builder(parameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));

  builder.addInput(input).sendLovelace(input.output().address(), 12'500'000).setDonation(500'000);
  const ProposalProcedureSet expected = addAllProposals(builder, GetParam());

  // Act
  const SubTransaction transaction = builder.build();

  // Assert
  ASSERT_TRUE(transaction.body().proposalProcedures());
  EXPECT_EQ(transaction.body().proposalProcedures()->toCbor(), expected.toCbor());
  EXPECT_EQ(transaction.body().donation(), 500'000U);
  EXPECT_EQ(computeImplicitCoin(transaction, parameters()).deposits, 7'000'000U);
  EXPECT_TRUE(computeSubTransactionImbalance(transaction, { input }, parameters()).isZero());
}

INSTANTIATE_TEST_SUITE_P(Predecessors, GovernanceBuilder, testing::Bool());
