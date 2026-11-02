/**
 * @file SubTransactionBodyTest.cpp
 *
 * @brief Unit tests for Dijkstra sub-transaction bodies.
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

TEST(SubTransactionBody, MinimalBodyMatchesNativeCborAndHash)
{
  // Arrange
  const TransactionInputSet   inputs;
  const TransactionOutputList outputs;
  const SubTransactionBody    body(inputs, outputs);
  CborReader                  reader("a200d90102800180");

  // Act
  const SubTransactionBody decoded = SubTransactionBody::fromCbor(body.toCbor());

  // Assert
  EXPECT_EQ(body.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(body.hash().toHex(), "da4603f4488d798af667794ab542f130a4bd1c20c7ed950c4648aaa95d0be4f4");
  EXPECT_EQ(decoded, body);
  EXPECT_EQ(body.inputs().size(), 0U);
  EXPECT_EQ(body.outputs().size(), 0U);
  EXPECT_FALSE(body.guards().has_value());
  EXPECT_FALSE(body.donation().has_value());
  EXPECT_FALSE(body.toJson().empty());
}

TEST(SubTransactionBody, DijkstraFieldsRoundTripAndClear)
{
  // Arrange
  const Credential    key     = Credential::fromCbor("8200581c00112233445566778899aabbccddeeff00112233445566778899aabb");
  const RewardAddress address = RewardAddress::fromBech32("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s");
  SubTransactionBody  body { TransactionInputSet(), TransactionOutputList() };
  GuardSet            guards;
  guards.add(key);
  RequiredGuardsMap required;
  required.insert(key);
  DirectDepositMap deposits;
  deposits.insert(address, 1000);
  AccountBalanceIntervalsMap balances;
  balances.insert(address, AccountBalanceInterval::exact(1000));

  // Act
  body.setGuards(guards);
  body.setRequiredTopLevelGuards(required);
  body.setDirectDeposits(deposits);
  body.setAccountBalanceIntervals(balances);
  body.setInvalidAfter(100);
  body.setInvalidBefore(10);
  body.setNetworkId(NetworkId::Testnet);
  body.setTreasuryValue(1000);
  body.setDonation(50);
  const SubTransactionBody copy(body);

  // Assert
  ASSERT_TRUE(body.guards().has_value());
  EXPECT_EQ(body.guards()->at(0), key);
  EXPECT_TRUE(body.requiredTopLevelGuards()->contains(key));
  EXPECT_EQ(body.directDeposits()->get(address), 1000U);
  EXPECT_EQ(body.accountBalanceIntervals()->get(address)->exactBalance(), 1000U);
  EXPECT_EQ(body.invalidAfter(), 100U);
  EXPECT_EQ(body.invalidBefore(), 10U);
  EXPECT_EQ(body.networkId(), NetworkId::Testnet);
  EXPECT_EQ(body.treasuryValue(), 1000U);
  EXPECT_EQ(body.donation(), 50U);
  EXPECT_EQ(SubTransactionBody::fromCbor(body.toCbor()), body);

  // Act
  body.setGuards(std::nullopt);
  body.setRequiredTopLevelGuards(std::nullopt);
  body.setDirectDeposits(std::nullopt);
  body.setAccountBalanceIntervals(std::nullopt);
  body.setDonation(std::nullopt);

  // Assert
  EXPECT_FALSE(body.guards().has_value());
  EXPECT_FALSE(body.requiredTopLevelGuards().has_value());
  EXPECT_FALSE(body.directDeposits().has_value());
  EXPECT_FALSE(body.accountBalanceIntervals().has_value());
  EXPECT_FALSE(body.donation().has_value());
  EXPECT_TRUE(copy.guards().has_value());
  EXPECT_NE(copy, body);
}

TEST(SubTransactionBody, MaximalNativeFixtureExposesEveryField)
{
  // Arrange
  const std::string_view cbor =
    "b300d90102818258200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d5000181825839009493"
    "315cd92eb5d8c4304e67b7e16ae36d61d34502694657811a2c8e32c728d3861e164cab28cb8f006448139c8f1740ffb8e7aa"
    "9e5232dc820aa3581c2a286ad895d091f2b3d168a6091ad2627d30a72761a5bc36eef00740a14014581c659f2917fb63f12b"
    "33667463ee575eeac1845bbc736b9c0bbc40ba82a14454534c411832581c7eae28af2208be856f7a119668ae52a49b73725e"
    "326dc16579dcc373a240182846504154415445181e031903e804d90102818304581c26b17b78de4f035dc0bfce60d1d3c3a8"
    "085c38dcce5fb8767e518bed1901f405a1581de1cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f0507"
    "58200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d508186409a3581c2a286ad895d091f2b3"
    "d168a6091ad2627d30a72761a5bc36eef00740a14014581c659f2917fb63f12b33667463ee575eeac1845bbc736b9c0bbc40"
    "ba82a14454534c413831581c7eae28af2208be856f7a119668ae52a49b73725e326dc16579dcc373a2401828465041544154"
    "45181e0b58200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d50ed90102828200581c001122"
    "33445566778899aabbccddeeff00112233445566778899aabb8201581caabbccddeeff00112233445566778899aabbccddee"
    "ff0011223344550f0112d90102818258200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d500"
    "13a18202581c00112233445566778899aabbccddeeff00112233445566778899aabba18258200f3abbc8fc19c2e61bab6059"
    "bf8a466e6e754833a08a62a6c56fe0e78f19d9d5038201f614d9010281841a000f4240581de1cb0ec2692497b458e46812c8"
    "a5bfa2931d1a2d965a99893828ec810f8106827668747470733a2f2f7777772e736f6d6575726c2e696f5820000000000000"
    "0000000000000000000000000000000000000000000000000000151907d0161903e81818a28200581c001122334455667788"
    "99aabbccddeeff00112233445566778899aabbf68201581caabbccddeeff00112233445566778899aabbccddeeff00112233"
    "4455d879801819a1581de1cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f1a000f4240181aa1581de0"
    "13cf55d175ea848b87deb3e914febd7e028e2bf6534475d52fb9c3d0821864191388";
  CborReader reader(cbor);

  // Act
  const SubTransactionBody body = SubTransactionBody::fromCbor(cbor);

  // Assert
  EXPECT_EQ(body.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(body.hash().toHex(), "29f569946d1ad32690effd3c74cfe3d437fae9bdf2797b6afa5d6d46e291da9e");
  EXPECT_EQ(body.inputs().size(), 1U);
  EXPECT_EQ(body.outputs().size(), 1U);
  EXPECT_TRUE(body.invalidAfter().has_value());
  EXPECT_TRUE(body.certificates().has_value());
  EXPECT_TRUE(body.withdrawals().has_value());
  EXPECT_TRUE(body.auxDataHash().has_value());
  EXPECT_TRUE(body.invalidBefore().has_value());
  EXPECT_TRUE(body.mint().has_value());
  EXPECT_TRUE(body.scriptDataHash().has_value());
  EXPECT_TRUE(body.guards().has_value());
  EXPECT_TRUE(body.networkId().has_value());
  EXPECT_TRUE(body.referenceInputs().has_value());
  EXPECT_TRUE(body.votingProcedures().has_value());
  EXPECT_TRUE(body.proposalProcedures().has_value());
  EXPECT_TRUE(body.treasuryValue().has_value());
  EXPECT_TRUE(body.donation().has_value());
  EXPECT_TRUE(body.requiredTopLevelGuards().has_value());
  EXPECT_TRUE(body.directDeposits().has_value());
  EXPECT_TRUE(body.accountBalanceIntervals().has_value());
}
