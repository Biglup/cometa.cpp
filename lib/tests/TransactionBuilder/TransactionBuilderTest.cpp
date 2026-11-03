/**
 * @file TransactionBuilderTest.cpp
 *
 * @brief Tests balanced transaction construction.
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
#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include <gtest/gtest.h>

#include <memory>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Creates the sender's address. @return The address. */
Address
sampleAddress()
{
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  return EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
}

/** @brief Creates protocol parameters for an ADA payment. @return The parameters. */
ProtocolParameters
sampleParameters()
{
  ProtocolParameters parameters;
  parameters.setMinFeeA(44);
  parameters.setMinFeeB(155381);
  parameters.setAdaPerUtxoByte(4310);
  parameters.setMaxTxSize(16384);
  return parameters;
}

/** @brief Selects the only test output and returns the exact remainder as change. */
class TestCoinSelector : public CoinSelector
{
  public:

    /** @brief Counts calls made by the builder. */
    mutable std::size_t calls = 0;

    /** @brief Gets the strategy name. @return Its name. */
    std::string
    name() const override
    {
      return "test-selector";
    }

    /** @brief Selects an output. @param request The target and available outputs. @return The selection. */
    CoinSelectionResult
    select(const CoinSelectionRequest& request) const override
    {
      ++calls;

      CoinSelectionResult result;
      result.selected = request.available;
      result.change.pushBack(
        TransactionOutput(request.changeAddress, Value(10000000 - request.target.coin())));
      return result;
    }
};
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(TransactionBuilder, BuildsBalancedAdaPayment)
{
  // Arrange
  const Address address = sampleAddress();
  const Utxo    available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  TransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setNetworkId(NetworkId::Testnet)
    .setChangeAddress(address)
    .setUtxos({ available })
    .sendLovelace(address, 2000000);

  // Act
  const Transaction transaction = builder.build();

  // Assert
  EXPECT_EQ(transaction.body().inputs().size(), 1U);
  EXPECT_EQ(transaction.body().outputs().size(), 2U);
  EXPECT_GT(transaction.body().fee(), 0U);
}

TEST(TransactionBuilder, UsesInjectedCoinSelector)
{
  // Arrange
  const Address address = sampleAddress();
  const Utxo    available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  const std::shared_ptr<TestCoinSelector> selector = std::make_shared<TestCoinSelector>();
  TransactionBuilder                      builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address)
    .setUtxos({ available })
    .setCoinSelector(selector)
    .sendLovelace(address, 2000000);

  // Act
  const Transaction transaction = builder.build();

  // Assert
  EXPECT_GT(selector->calls, 0U);
  EXPECT_EQ(transaction.body().inputs().size(), 1U);
  EXPECT_EQ(transaction.body().outputs().size(), 2U);
}

TEST(TransactionBuilder, ReportsBuildFailure)
{
  // Arrange
  TransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(sampleAddress()).sendLovelace(sampleAddress(), 2000000);

  // Act
  // Assert
  EXPECT_THROW(builder.build(), Error);
}

TEST(TransactionBuilder, RejectsMissingDeferredRedeemerCallback)
{
  // Arrange
  const Address address = sampleAddress();
  const Utxo    available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  const AssetId      asset(Blake2bHash(std::vector<std::uint8_t>(28, 3)), AssetName(std::vector<std::uint8_t> { 'T' }));
  TransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));

  // Act
  // Assert
  EXPECT_THROW(builder.addInputWithDeferredRedeemer(available, DeferredRedeemer()), std::invalid_argument);
  EXPECT_THROW(builder.mintTokenWithDeferredRedeemer(asset, 1, DeferredRedeemer()), std::invalid_argument);
  const RewardAddress reward(NetworkId::Testnet, Credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1))));
  const Certificate   certificate(StakeRegistrationCert(reward.credential()));
  EXPECT_THROW(builder.withdrawRewardsWithDeferredRedeemer(reward, 0, DeferredRedeemer()), std::invalid_argument);
  EXPECT_THROW(builder.addCertificateWithDeferredRedeemer(certificate, DeferredRedeemer()), std::invalid_argument);
}

TEST(TransactionBuilder, AddsStakeRegistrationCertificate)
{
  // Arrange
  ProtocolParameters parameters = sampleParameters();
  parameters.setKeyDeposit(2000000);
  const Address    address = sampleAddress();
  const Credential stakeCredential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 4)));
  const Utxo       available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  TransactionBuilder builder(parameters, SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address)
    .setUtxos({ available })
    .addCertificate(Certificate(StakeRegistrationCert(stakeCredential)));

  // Act
  const Transaction transaction = builder.build();

  // Assert
  ASSERT_TRUE(transaction.body().certificates().has_value());
  EXPECT_EQ(transaction.body().certificates()->size(), 1U);
}

TEST(TransactionBuilder, AddsGovernanceVote)
{
  // Arrange
  ProtocolParameters parameters = sampleParameters();
  parameters.setProtocolVersion(ProtocolVersion(11, 0));
  const Address address = sampleAddress();
  const Utxo    available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  const Credential         credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 4)));
  const Voter              voter(VoterType::DRepKeyHash, credential);
  const GovernanceActionId action(Blake2bHash(std::vector<std::uint8_t>(32, 5)), 0);
  const VotingProcedure    choice(Vote::Yes);
  TransactionBuilder       builder(parameters, SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address).setUtxos({ available }).vote(voter, action, choice);

  // Act
  const Transaction transaction = builder.build();

  // Assert
  ASSERT_TRUE(transaction.body().votingProcedures().has_value());
  EXPECT_EQ(transaction.body().votingProcedures()->get(voter, action), choice);
}

TEST(TransactionBuilder, MintsNativeToken)
{
  // Arrange
  const Address address = sampleAddress();
  const Utxo    available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  const Blake2bHash  keyHash(std::vector<std::uint8_t>(28, 1));
  const NativeScript nativeScript { ScriptPubkey(keyHash) };
  const Script       script(nativeScript);
  const AssetId      asset(nativeScript.hash(), AssetName(std::vector<std::uint8_t> { 'T' }));
  Value              outputValue(2000000);
  outputValue.addAsset(asset, 1);

  TransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address)
    .setUtxos({ available })
    .addScript(script)
    .addSigner(keyHash)
    .mintToken(asset, 1)
    .sendValue(address, outputValue);

  // Act
  const Transaction transaction = builder.build();

  // Assert
  EXPECT_TRUE(transaction.body().mint().has_value());
  ASSERT_TRUE(transaction.witnessSet().nativeScripts().has_value());
  EXPECT_EQ(transaction.witnessSet().nativeScripts()->size(), 1U);
}

TEST(TransactionBuilder, LocksScriptOutputWithDatumAndMetadata)
{
  // Arrange
  const Address    address = sampleAddress();
  const Credential scriptCredential(
    CredentialType::ScriptHash, Blake2bHash(std::vector<std::uint8_t>(28, 3)));
  const Address scriptAddress = EnterpriseAddress(NetworkId::Testnet, scriptCredential).toAddress();
  const Utxo    available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  TransactionBuilder builder(sampleParameters(), SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address)
    .setUtxos({ available })
    .lockLovelace(scriptAddress, 2000000, Datum(PlutusData(42)))
    .setMetadata(674, Metadatum("test"));

  // Act
  const Transaction transaction = builder.build();

  // Assert
  ASSERT_EQ(transaction.body().outputs().size(), 2U);
  EXPECT_EQ(transaction.body().outputs().at(0).address(), scriptAddress);
  EXPECT_TRUE(transaction.auxiliaryData().has_value());
  ASSERT_TRUE(transaction.auxiliaryData()->metadata().has_value());
}

TEST(TransactionBuilder, BuildsDirectDepositAndBalanceBounds)
{
  // Arrange
  ProtocolParameters parameters = sampleParameters();
  parameters.setProtocolVersion(ProtocolVersion(12, 0));
  const Address       address = sampleAddress();
  const Credential    credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const RewardAddress reward(NetworkId::Testnet, credential);
  const Utxo          available(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  TransactionBuilder builder(parameters, SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address)
    .setUtxos({ available })
    .addDirectDeposit(reward, 2000000)
    .addAccountBalanceInterval(reward, AccountBalanceInterval(0, 10000000))
    .addStartingAccountBalanceInterval(reward, AccountBalanceInterval(0, 10000000));

  // Act
  const Transaction transaction = builder.build();

  // Assert
  EXPECT_TRUE(transaction.body().directDeposits().has_value());
  EXPECT_TRUE(transaction.body().accountBalanceIntervals().has_value());
  EXPECT_TRUE(transaction.body().startingAccountBalanceIntervals().has_value());
}

TEST(TransactionBuilder, IncludesSubTransaction)
{
  // Arrange
  ProtocolParameters parameters = sampleParameters();
  parameters.setProtocolVersion(ProtocolVersion(12, 0));
  const Address address = sampleAddress();
  const Utxo    funding(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 2)), 0),
    TransactionOutput(address, Value(10000000)));
  const Utxo party(
    TransactionInput(Blake2bHash(std::vector<std::uint8_t>(32, 3)), 0),
    TransactionOutput(address, Value(5000000)));
  const SubTransactionBody subBody(
    TransactionInputSet({ party.input() }),
    TransactionOutputList({ TransactionOutput(address, Value(5000000)) }));
  const SubTransaction subTransaction(subBody, WitnessSet());
  TransactionBuilder   builder(parameters, SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.setChangeAddress(address).setUtxos({ funding }).addSubTransaction(subTransaction, { party });

  // Act
  const Transaction transaction = builder.build();

  // Assert
  ASSERT_TRUE(transaction.body().subTransactions().has_value());
  EXPECT_EQ(transaction.body().subTransactions()->size(), 1U);
}
