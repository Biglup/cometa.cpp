/**
 * @file KoiosProviderTest.cpp
 *
 * @brief Tests Koios responses through an injected transport.
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

TEST(KoiosProvider, CompleteProtocolSnapshot)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(rpc(JsonValue::parse(fixture("ogmios-parameters")).at("result").toJson())));
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act
  const ProtocolParameters parameters = provider.getParameters();

  // Assert
  EXPECT_EQ(parameters.adaPerUtxoByte(), 4310U);
  EXPECT_EQ(parameters.protocolVersion().major(), 11U);
  EXPECT_EQ(parameters.refScriptCostStride(), 25600U);
  EXPECT_EQ(parameters.minFeeA(), 44U);
  EXPECT_EQ(provider.name(), "Koios");
  EXPECT_EQ(provider.networkMagic(), NetworkMagic::Preprod);
}

TEST(KoiosProvider, HttpFailuresAreNotEmptyResults)
{
  // Arrange
  for (const long status: { 401L, 403L, 429L, 500L })
  {
    const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
    transport->responses.push_back(reply("credential-secret", status));
    const KoiosProvider provider(NetworkMagic::Preprod, transport);

    // Act & Assert
    EXPECT_THROW(provider.getParameters(), std::runtime_error);
    EXPECT_EQ(transport->requests.size(), 1U);
  }
}

TEST(KoiosProvider, ZeroTimeoutMakesNoRequest)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  const KoiosProvider              provider(NetworkMagic::Preprod, transport);

  // Act & Assert
  EXPECT_FALSE(provider.confirmTransaction(transaction().id(), std::chrono::milliseconds(0)));
  EXPECT_TRUE(transport->requests.empty());
}

TEST(KoiosProvider, SubmissionRejectsMismatchedIdWithoutRetry)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(rpc("{\"transaction\":{\"id\":\"" + std::string(64, '1') + "\"}}")));
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act & Assert
  EXPECT_THROW(provider.submitTransaction(transaction()), std::runtime_error);
  EXPECT_EQ(transport->requests.size(), 1U);
}

TEST(KoiosProvider, UnexpectedEvaluationBudgetFails)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(rpc(R"([{"validator":{"purpose":"spend","index":0},"budget":{"memory":1,"cpu":2}}])")));
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act & Assert
  EXPECT_THROW(provider.evaluateTransaction(transaction(), UtxoList()), std::runtime_error);
}

TEST(KoiosProvider, RpcCorrelationIsRequired)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(R"({"jsonrpc":"2.0","id":"wrong","result":{}})"));
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act & Assert
  EXPECT_THROW(provider.getParameters(), std::runtime_error);
}

TEST(KoiosProvider, LiveOutputsRetainAllReferenceScripts)
{
  // Arrange
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  const std::string                source    = fixture("koios-utxos");
  transport->responses.push_back(reply(source));
  const JsonValue     rows = JsonValue::parse(source);
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act
  const UtxoList outputs = provider.getUnspentOutputs(Address::fromString(rows.at(0).at("address").asString()));

  // Assert
  ASSERT_EQ(outputs.size(), rows.size());
  std::size_t scripts = 0;
  for (std::size_t i = 0; i < outputs.size(); ++i)
  {
    const std::optional<Script> script = outputs.at(i).output().scriptRef();
    if (script)
    {
      EXPECT_EQ(script->hash().toHex(), rows.at(i).at("reference_script").at("hash").asString());
      ++scripts;
    }
  }
  EXPECT_EQ(scripts, 3U);
}

TEST(KoiosProvider, EvaluationRequiresCompleteUniqueBudgetSet)
{
  // Arrange
  const Transaction                tx        = Transaction::fromCbor("84a3008001800200a1058184000000820000f5f6");
  const std::string                budget    = R"({"validator":{"purpose":"spend","index":0},"budget":{"memory":17,"cpu":29}})";
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply(rpc("[" + budget + "]")));
  transport->responses.push_back(reply(rpc("[]")));
  transport->responses.push_back(reply(rpc("[" + budget + "," + budget + "]")));
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act
  const RedeemerList result = provider.evaluateTransaction(tx, UtxoList());

  // Assert
  ASSERT_EQ(result.size(), 1U);
  EXPECT_EQ(result.at(0).exUnits().memory(), 17U);
  EXPECT_EQ(result.at(0).exUnits().cpuSteps(), 29U);
  EXPECT_EQ(result.at(0).data().toCbor(), tx.witnessSet().redeemers()->at(0).data().toCbor());
  EXPECT_THROW(provider.evaluateTransaction(tx, UtxoList()), std::runtime_error);
  EXPECT_THROW(provider.evaluateTransaction(tx, UtxoList()), std::runtime_error);
}

TEST(KoiosProvider, RejectsDuplicateAssetsWithDifferentHexCase)
{
  // Arrange
  const JsonValue   original = JsonValue::parse(fixture("koios-utxos")).at(0);
  const std::string address  = original.at("address").asString();
  std::string       row      = original.toJson();
  const std::string key      = "\"asset_list\":[]";
  const std::size_t position = row.find(key);
  ASSERT_NE(position, std::string::npos);
  const std::string lower = "{\"policy_id\":\"" + std::string(56, 'a') + "\",\"asset_name\":\"\",\"quantity\":\"1\"}";
  const std::string upper = "{\"policy_id\":\"" + std::string(56, 'A') + "\",\"asset_name\":\"\",\"quantity\":\"1\"}";
  row.replace(position, key.size(), "\"asset_list\":[" + lower + "," + upper + "]");
  const std::shared_ptr<Transport> transport = std::make_shared<Transport>();
  transport->responses.push_back(reply("[" + row + "]"));
  const KoiosProvider provider(NetworkMagic::Preprod, transport);

  // Act & Assert
  EXPECT_THROW(provider.getUnspentOutputs(Address::fromString(address)), std::runtime_error);
}
