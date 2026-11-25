/**
 * @file HydraProviderTest.cpp
 *
 * @brief Tests Hydra responses through an injected transport.
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

#include "ProviderFixtures.h"

/* USINGS ********************************************************************/

using namespace Cometa;
using namespace ProviderFixtures;

/* IMPLEMENTATION ************************************************************/

TEST(HydraProvider, CompleteProtocolSnapshot)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(fixture("hydra-parameters")));
  const HydraProvider provider(NetworkMagic::Preprod, "https://hydra.test", transport);

  // Act
  const ProtocolParameters parameters = provider.getParameters();

  // Assert
  EXPECT_EQ(parameters.adaPerUtxoByte(), 4310U);
  EXPECT_EQ(parameters.protocolVersion().major(), 11U);
  EXPECT_EQ(parameters.refScriptCostStride(), 25600U);
  EXPECT_EQ(parameters.minFeeA(), 0U);
  EXPECT_EQ(provider.name(), "Hydra");
  EXPECT_EQ(provider.networkMagic(), NetworkMagic::Preprod);
}

TEST(HydraProvider, HttpFailuresAreNotEmptyResults)
{
  // Arrange
  for (const long status: { 401L, 403L, 429L, 500L })
  {
    const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
    transport->responses.push_back(reply("credential-secret", status));
    const HydraProvider provider(NetworkMagic::Preprod, "https://hydra.test", transport);

    // Act & Assert
    EXPECT_THROW(provider.getParameters(), std::runtime_error);
    EXPECT_EQ(transport->requests.size(), 1U);
  }
}

TEST(HydraProvider, ZeroTimeoutMakesNoRequest)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  const HydraProvider              provider(NetworkMagic::Preprod, "https://hydra.test", transport);

  // Act & Assert
  EXPECT_FALSE(provider.confirmTransaction(transaction().id(), std::chrono::milliseconds(0)));
  EXPECT_TRUE(transport->requests.empty());
}

TEST(HydraProvider, UpstreamOutputsRetainNativeAndPlutusReferences)
{
  // Arrange
  const std::string source = fixture("hydra-utxos");
  const JsonValue   rows   = JsonValue::parse(source);
  for (std::size_t i = 0; i < rows.size(); ++i)
  {
    const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
    transport->responses.push_back(reply(source));
    const HydraProvider provider(NetworkMagic::Preprod, "https://hydra.test", transport);

    // Act
    const UtxoList outputs = provider.getUnspentOutputs(Address::fromString(rows.valueAt(i).at("address").asString()));

    // Assert
    ASSERT_EQ(outputs.size(), 1U);
    EXPECT_TRUE(outputs.at(0).output().scriptRef().has_value());
  }
}

TEST(HydraProvider, SubmissionUsesL2AndDoesNotClaimConfirmation)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(R"({"tag":"SubmitTxSubmitted"})", 202));
  const HydraProvider provider(NetworkMagic::Preprod, "https://hydra.test", transport);
  const Transaction   tx = transaction();

  // Act & Assert
  EXPECT_EQ(provider.submitTransaction(tx), tx.id());
  ASSERT_EQ(transport->requests.size(), 1U);
  EXPECT_EQ(transport->requests[0].url, "https://hydra.test/transaction");
  const std::vector<std::uint8_t>& body = transport->requests[0].body;
  EXPECT_EQ(JsonValue::parse(std::string(body.begin(), body.end())).at("cborHex").asString(), "84a3008001800200a0f5f6");
}

TEST(HydraProvider, UnsupportedOperationsMakeNoRequests)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  const HydraProvider              provider(NetworkMagic::Preprod, "https://hydra.test", transport);

  // Act & Assert
  EXPECT_THROW(provider.getRewardsAvailable(reward()), std::logic_error);
  EXPECT_THROW(provider.evaluateTransaction(transaction(), UtxoList()), std::logic_error);
  EXPECT_TRUE(transport->requests.empty());
}

TEST(HydraProvider, SnapshotTransactionRecordsProveConfirmation)
{
  // Arrange
  const Transaction                tx        = transaction();
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply("{\"tag\":\"ConfirmedSnapshot\",\"snapshot\":{\"number\":1,\"confirmed\":[{\"cborHex\":\"84a3008001800200a0f5f6\",\"txId\":\"" + tx.id().toHex() + "\"}]}}"));
  const HydraProvider provider(NetworkMagic::Preprod, "https://hydra.test", transport);

  // Act & Assert
  EXPECT_TRUE(provider.confirmTransaction(tx.id(), std::chrono::milliseconds(1000)));
}

TEST(HydraProvider, RejectsInlineHashWithoutDatumBytes)
{
  // Arrange
  const JsonValue                  rows      = JsonValue::parse(fixture("hydra-utxos"));
  const std::string                address   = rows.valueAt(0).at("address").asString();
  const std::string                source    = "{\"" + rows.keyAt(0) + "\":{\"address\":\"" + address + "\",\"value\":{\"lovelace\":1},\"inlineDatumhash\":\"" + std::string(64, '1') + "\"}}";
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(source));
  const HydraProvider provider(NetworkMagic::Preprod, "https://hydra.test", transport);

  // Act & Assert
  EXPECT_THROW(provider.getUnspentOutputs(Address::fromString(address)), std::runtime_error);
}
