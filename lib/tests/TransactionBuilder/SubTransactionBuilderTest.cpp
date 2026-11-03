/**
 * @file SubTransactionBuilderTest.cpp
 *
 * @brief Tests Dijkstra sub-transaction construction.
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
/** @brief Creates a test payment address. @return The address. */
Address
sampleAddress()
{
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  return EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
}

/** @brief Creates Dijkstra test parameters. @return The parameters. */
ProtocolParameters
sampleParameters()
{
  ProtocolParameters parameters;
  parameters.setMinFeeA(44);
  parameters.setMinFeeB(155381);
  parameters.setAdaPerUtxoByte(4310);
  parameters.setMaxTxSize(16384);
  parameters.setProtocolVersion(ProtocolVersion(12, 0));
  return parameters;
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(SubTransactionBuilder, BuildsAndBatchesGuardedPayment)
{
  // Arrange
  const Address    address = sampleAddress();
  const Credential guard(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Utxo       party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(5000000)));
  const Utxo funding(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 3)), 0),
    TransactionOutput(address, Value(10000000)));
  const ProtocolParameters parameters = sampleParameters();
  const SlotConfig         slots      = SlotConfig::forNetwork(NetworkMagic::Preprod);
  SubTransactionBuilder    subBuilder(parameters, slots);
  subBuilder.addInput(party)
    .sendLovelace(address, 5000000)
    .requireTopLevelGuard(guard);

  // Act
  const SubTransaction subTransaction = subBuilder.build();
  TransactionBuilder   builder(parameters, slots);
  builder.setChangeAddress(address)
    .setUtxos({ funding })
    .addGuard(guard)
    .addSubTransaction(subTransaction, { party });
  const Transaction transaction = builder.build();

  // Assert
  EXPECT_EQ(subTransaction.body().inputs().size(), 1U);
  EXPECT_EQ(subTransaction.body().outputs().size(), 1U);
  EXPECT_TRUE(subTransaction.body().requiredTopLevelGuards().has_value());
  ASSERT_TRUE(transaction.body().subTransactions().has_value());
  EXPECT_EQ(transaction.body().subTransactions()->size(), 1U);
}

TEST(SubTransactionBuilder, RejectsSecondBuild)
{
  // Arrange
  const Address address = sampleAddress();
  const Utxo    party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(5000000)));
  SubTransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.addInput(party).sendLovelace(address, 5000000);

  // Act
  const SubTransaction transaction = builder.build();

  // Assert
  EXPECT_EQ(transaction.body().inputs().size(), 1U);
  EXPECT_THROW(builder.build(), Error);
}

TEST(SubTransactionBuilder, RejectsMissingDeferredRedeemerCallback)
{
  // Arrange
  const Address address = sampleAddress();
  const Utxo    party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(5000000)));
  SubTransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));

  // Act
  // Assert
  EXPECT_THROW(builder.addInputWithDeferredRedeemer(party, DeferredRedeemer()), std::invalid_argument);
  const RewardAddress      reward(NetworkId::Testnet, Credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1))));
  const Certificate        certificate(StakeRegistrationCert(reward.credential()));
  const Voter              voter(VoterType::DRepKeyHash, reward.credential());
  const GovernanceActionId action(Blake2bHash(std::vector<std::uint8_t>(32, 5)), 0);
  const VotingProcedure    choice(Vote::Yes);
  EXPECT_THROW(builder.withdrawRewardsWithDeferredRedeemer(reward, 0, DeferredRedeemer()), std::invalid_argument);
  EXPECT_THROW(builder.addCertificateWithDeferredRedeemer(certificate, DeferredRedeemer()), std::invalid_argument);
  EXPECT_THROW(builder.voteWithDeferredRedeemer(voter, action, choice, DeferredRedeemer()), std::invalid_argument);
}

TEST(SubTransactionBuilder, BuildsMintWithReferenceAndDirectDeposit)
{
  // Arrange
  const Address       address = sampleAddress();
  const Credential    credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const RewardAddress reward(NetworkId::Testnet, credential);
  const Utxo          party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(5000000)));
  const Utxo reference(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 3)), 0),
    TransactionOutput(address, Value(2000000)));
  const Blake2bHash  keyHash(std::vector<std::uint8_t>(28, 1));
  const NativeScript nativeScript { ScriptPubkey(keyHash) };
  const AssetId      asset(nativeScript.hash(), AssetName(std::vector<std::uint8_t> { 'T' }));
  Value              outputValue(2000000);
  outputValue.addAsset(asset, 1);
  SubTransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.addInput(party)
    .addReferenceInput(reference)
    .addScript(Script(nativeScript))
    .addSigner(keyHash)
    .mintToken(asset, 1)
    .sendValue(address, outputValue)
    .addDirectDeposit(reward, 1000000)
    .setMetadata(674, Metadatum("test"));

  // Act
  const SubTransaction transaction = builder.build();

  // Assert
  EXPECT_TRUE(transaction.body().mint().has_value());
  EXPECT_TRUE(transaction.body().referenceInputs().has_value());
  EXPECT_TRUE(transaction.body().directDeposits().has_value());
}

TEST(SubTransactionBuilder, LocksOutputWithDatum)
{
  // Arrange
  const Address    address = sampleAddress();
  const Credential scriptCredential(
    CredentialType::ScriptHash, Blake2bHash(std::vector<std::uint8_t>(28, 3)));
  const Address scriptAddress = EnterpriseAddress(NetworkId::Testnet, scriptCredential).toAddress();
  const Utxo    party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(5000000)));
  SubTransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.addInput(party).lockLovelace(scriptAddress, 2000000, Datum(PlutusData(42)));

  // Act
  const SubTransaction transaction = builder.build();

  // Assert
  ASSERT_EQ(transaction.body().outputs().size(), 1U);
  EXPECT_EQ(transaction.body().outputs().at(0).address(), scriptAddress);
}

TEST(SubTransactionBuilder, AddsCertificateAndVote)
{
  // Arrange
  ProtocolParameters parameters = sampleParameters();
  parameters.setKeyDeposit(2000000);
  const Address       address = sampleAddress();
  const Credential    credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 4)));
  const RewardAddress reward(NetworkId::Testnet, credential);
  const Utxo          party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(5000000)));
  const Voter              voter(VoterType::DRepKeyHash, credential);
  const GovernanceActionId action(Blake2bHash(std::vector<std::uint8_t>(32, 5)), 0);
  const VotingProcedure    choice(Vote::Yes);
  SubTransactionBuilder    builder(parameters, SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.addInput(party)
    .addCertificate(Certificate(StakeRegistrationCert(credential)))
    .vote(voter, action, choice)
    .addAccountBalanceInterval(reward, AccountBalanceInterval(0, 10000000));

  // Act
  const SubTransaction transaction = builder.build();

  // Assert
  ASSERT_TRUE(transaction.body().certificates().has_value());
  EXPECT_EQ(transaction.body().certificates()->size(), 1U);
  ASSERT_TRUE(transaction.body().votingProcedures().has_value());
  EXPECT_EQ(transaction.body().votingProcedures()->get(voter, action), choice);
}
