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
