/**
 * @file BlockfrostProviderTest.cpp
 *
 * @brief Tests Blockfrost responses through an injected transport.
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
#include <Cometa/Providers/BlockfrostProvider.h>

#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief A queued response and its expected endpoint. */
struct ExpectedRequest
{
    /** @brief Expected method. */
    std::string method;

    /** @brief Expected API path. */
    std::string path;

    /** @brief Response status. */
    long status;

    /** @brief Response body. */
    std::string body;
};

/** @brief Records requests and serves fixed responses. */
class FakeHttpTransport final : public HttpTransport
{
  public:

    /** @brief Expected requests in order. */
    std::vector<ExpectedRequest> expected;

    /** @brief Recorded requests. */
    mutable std::vector<HttpRequest> requests;

  public:

    /** @copydoc HttpTransport::execute */
    HttpResponse
    execute(const HttpRequest& request) const override
    {
      if (requests.size() >= expected.size())
      {
        throw std::runtime_error("Unexpected Blockfrost request");
      }

      const ExpectedRequest& next = expected.at(requests.size());
      EXPECT_EQ(request.method, next.method);
      EXPECT_EQ(request.url, "https://cardano-preprod.blockfrost.io/api/v0/" + next.path);
      requests.push_back(request);

      return HttpResponse { next.status, std::vector<std::uint8_t>(next.body.begin(), next.body.end()) };
    }
};

/** @brief Returns a sample payment address. @return The address. */
Address
sampleAddress()
{
  return Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6");
}

/** @brief Returns a sample reward address. @return The address. */
RewardAddress
sampleRewardAddress()
{
  return RewardAddress::fromBech32("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s");
}

/** @brief Loads a shared transaction fixture. @return The transaction. */
Transaction
sampleTransaction()
{
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/builders.json", std::ios::binary);

  if (!input)
  {
    throw std::runtime_error("Cannot open the native builder conformance corpus");
  }

  const std::string document((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   fixture = JsonValue::parse(document).at("cases").at(0).at("expected");
  return Transaction::fromCbor(fixture.at("cbor").asString());
}

} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(BlockfrostProvider, ParsesProtocolParameters)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "GET", "epochs/latest/parameters", 200, R"({"min_fee_a":"44","min_fee_b":"155381","max_tx_size":"16384","max_val_size":"5000","coins_per_utxo_size":"4310","key_deposit":"2000000","pool_deposit":"500000000","protocol_major_ver":"11","protocol_minor_ver":"0","collateral_percent":"150","max_collateral_inputs":"3","max_tx_ex_mem":"14000000","max_tx_ex_steps":"10000000000","price_mem":0.1,"price_step":0.2,"cost_models":{"PlutusV4":[1,2,3]},"gov_action_deposit":"100000000","drep_deposit":"500000000","drep_activity":"20","min_fee_ref_script_cost_per_byte":"15"})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const ProtocolParameters parameters = provider.getParameters();

  // Assert
  EXPECT_EQ(parameters.minFeeA(), 44U);
  EXPECT_EQ(parameters.maxTxSize(), 16384U);
  EXPECT_EQ(parameters.protocolVersion().major(), 11U);
  EXPECT_TRUE(parameters.costModels().contains(PlutusLanguageVersion::V4));
  EXPECT_EQ(parameters.refScriptCostPerByte().numerator(), 15U);
  ASSERT_EQ(transport->requests.size(), 1U);
  ASSERT_FALSE(transport->requests.at(0).headers.empty());
  EXPECT_EQ(transport->requests.at(0).headers.at(0).first, "project_id");
  EXPECT_EQ(transport->requests.at(0).headers.at(0).second, "test-project-id");
}

TEST(BlockfrostProvider, PrefersOrderedRawCostModels)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "GET", "epochs/latest/parameters", 200, R"({"min_fee_a":44,"min_fee_b":155381,"max_tx_size":16384,"max_val_size":"5000","coins_per_utxo_size":"4310","key_deposit":"2000000","pool_deposit":"500000000","protocol_major_ver":11,"protocol_minor_ver":0,"cost_models":{"PlutusV3":{"addInteger-cpu-arguments-intercept":100788}},"cost_models_raw":{"PlutusV3":[100788,420]}})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const ProtocolParameters parameters = provider.getParameters();

  // Assert
  EXPECT_TRUE(parameters.costModels().contains(PlutusLanguageVersion::V3));
}

TEST(BlockfrostProvider, ReadsUtxosAndRewards)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  const std::string                        address   = sampleAddress().toString();
  transport->expected.push_back({ "GET", "addresses/" + address + "/utxos?count=100&page=1", 200, "[{\"tx_hash\":\"0101010101010101010101010101010101010101010101010101010101010101\",\"output_index\":0,\"address\":\"" + address + "\",\"amount\":[{\"unit\":\"lovelace\",\"quantity\":\"3000000\"}]}]" });
  transport->expected.push_back({ "GET", "accounts/" + sampleRewardAddress().toString(), 200, R"({"withdrawable_amount":"12345"})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const UtxoList      outputs = provider.getUnspentOutputs(sampleAddress());
  const std::uint64_t rewards = provider.getRewardsAvailable(sampleRewardAddress());

  // Assert
  ASSERT_EQ(outputs.size(), 1U);
  EXPECT_EQ(outputs.at(0).output().amount().coin(), 3000000);
  EXPECT_EQ(rewards, 12345U);
  EXPECT_EQ(transport->requests.size(), 2U);
}

TEST(BlockfrostProvider, ReturnsEmptyOutputsAndZeroRewardsForMissingAddress)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "GET", "addresses/" + sampleAddress().toString() + "/utxos?count=100&page=1", 404, "{}" });
  transport->expected.push_back({ "GET", "accounts/" + sampleRewardAddress().toString(), 404, "{}" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const UtxoList      outputs = provider.getUnspentOutputs(sampleAddress());
  const std::uint64_t rewards = provider.getRewardsAvailable(sampleRewardAddress());

  // Assert
  EXPECT_TRUE(outputs.empty());
  EXPECT_EQ(rewards, 0U);
}

TEST(BlockfrostProvider, RejectsBadConfigurationAndHttpFailures)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "GET", "epochs/latest/parameters", 403, R"({"message":"forbidden"})" });

  // Act & Assert
  EXPECT_THROW(BlockfrostProvider(NetworkMagic::Preprod, "", transport), std::invalid_argument);
  EXPECT_THROW(BlockfrostProvider(NetworkMagic::Preprod, "id", nullptr), std::invalid_argument);
  EXPECT_THROW(BlockfrostProvider(static_cast<NetworkMagic>(0), "id", transport), std::invalid_argument);

  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);
  EXPECT_THROW(provider.getParameters(), std::runtime_error);
}

TEST(BlockfrostProvider, ResolvesOutputAndDatum)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  const std::string                        id(64, '1');
  const std::string                        address = sampleAddress().toString();
  transport->expected.push_back({ "GET", "txs/" + id + "/utxos", 200, "{\"outputs\":[{\"output_index\":0,\"address\":\"" + address + "\",\"amount\":[{\"unit\":\"lovelace\",\"quantity\":\"3000000\"}]}]}" });
  transport->expected.push_back({ "GET", "scripts/datum/" + std::string(64, '2') + "/cbor", 200, R"({"cbor":"00"})" });
  const BlockfrostProvider  provider(NetworkMagic::Preprod, "test-project-id", transport);
  const TransactionInputSet inputs { TransactionInput::fromHex(id, 0) };

  // Act
  const UtxoList   outputs = provider.resolveUnspentOutputs(inputs);
  const PlutusData datum   = provider.resolveDatum(Blake2bHash::fromHex(std::string(64, '2')));

  // Assert
  ASSERT_EQ(outputs.size(), 1U);
  EXPECT_EQ(outputs.at(0).output().amount().coin(), 3000000);
  EXPECT_EQ(datum.toJson(), R"({"tag":"integer","value":"0"})");
}

TEST(BlockfrostProvider, ResolvesNativeReferenceScript)
{
  // Arrange
  const NativeScript                       nativeScript = NativeScript::fromJson(R"({"type":"after","slot":5})");
  const Script                             script(nativeScript);
  const std::string                        hash      = script.hash().toHex();
  const std::string                        address   = sampleAddress().toString();
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "GET", "addresses/" + address + "/utxos?count=100&page=1", 200, "[{\"tx_hash\":\"" + std::string(64, '1') + "\",\"output_index\":0,\"address\":\"" + address + "\",\"amount\":[{\"unit\":\"lovelace\",\"quantity\":\"3000000\"}],\"reference_script_hash\":\"" + hash + "\"}]" });
  transport->expected.push_back({ "GET", "scripts/" + hash, 200, R"({"type":"timelock"})" });
  transport->expected.push_back({ "GET", "scripts/" + hash + "/json", 200, R"({"json":{"type":"after","slot":5}})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const UtxoList outputs = provider.getUnspentOutputs(sampleAddress());

  // Assert
  ASSERT_EQ(outputs.size(), 1U);
  ASSERT_TRUE(outputs.at(0).output().scriptRef().has_value());
  EXPECT_EQ(outputs.at(0).output().scriptRef()->hash(), script.hash());
  EXPECT_EQ(transport->requests.size(), 3U);
}

TEST(BlockfrostProvider, SubmitsAndEvaluatesSerializedTransaction)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport   = std::make_shared<FakeHttpTransport>();
  const Transaction                        transaction = sampleTransaction();
  const std::string                        id          = transaction.id().toHex();
  transport->expected.push_back({ "POST", "tx/submit", 200, "\"" + id + "\"" });
  transport->expected.push_back({ "POST", "utils/txs/evaluate", 200, R"({"result":{"EvaluationResult":{}}})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const Blake2bHash  submitted = provider.submitTransaction(transaction);
  const RedeemerList evaluated = provider.evaluateTransaction(transaction, {});

  // Assert
  EXPECT_EQ(submitted.toHex(), id);
  EXPECT_EQ(evaluated.size(), 0U);
  ASSERT_EQ(transport->requests.size(), 2U);
  EXPECT_EQ(transport->requests.at(0).body, transaction.toCbor());
  EXPECT_EQ(transport->requests.at(1).body, transaction.toCbor());
}

TEST(BlockfrostProvider, ChecksConfirmationWithoutWaiting)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  const Blake2bHash                        id        = Blake2bHash::fromHex(std::string(64, '3'));
  transport->expected.push_back({ "GET", "txs/" + id.toHex(), 404, "{}" });
  transport->expected.push_back({ "GET", "txs/" + id.toHex(), 200, "{}" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const bool pending   = provider.confirmTransaction(id, std::chrono::milliseconds(0));
  const bool confirmed = provider.confirmTransaction(id, std::chrono::milliseconds(0));

  // Assert
  EXPECT_FALSE(pending);
  EXPECT_TRUE(confirmed);
}

TEST(BlockfrostProvider, FindsNftOutput)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  const AssetId                            asset     = AssetId::fromHex(std::string(56, '4') + "aa");
  const std::string                        address   = sampleAddress().toString();
  transport->expected.push_back({ "GET", "assets/" + asset.toHex() + "/addresses", 200, "[{\"address\":\"" + address + "\",\"quantity\":\"1\"}]" });
  transport->expected.push_back({ "GET", "addresses/" + address + "/utxos/" + asset.toHex() + "?count=100&page=1", 200, "[{\"tx_hash\":\"" + std::string(64, '5') + "\",\"output_index\":1,\"address\":\"" + address + "\",\"amount\":[{\"unit\":\"lovelace\",\"quantity\":\"2000000\"},{\"unit\":\"" + asset.toHex() + "\",\"quantity\":\"1\"}]}]" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);

  // Act
  const Utxo output = provider.getUnspentOutputByNft(asset);

  // Assert
  EXPECT_EQ(output.input().index(), 1U);
  EXPECT_EQ(output.output().amount().toAssetMap().at(asset), 1);
}

TEST(BlockfrostProvider, SendsAdditionalUtxosForEvaluation)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "POST", "utils/txs/evaluate/utxos", 200, R"({"result":{"EvaluationResult":{}}})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);
  const Transaction        transaction = sampleTransaction();
  const Utxo               output(
    TransactionInput::fromHex(std::string(64, '6'), 2),
    TransactionOutput(sampleAddress(), Value(3000000)));

  // Act
  const RedeemerList evaluated = provider.evaluateTransaction(transaction, { output });

  // Assert
  EXPECT_EQ(evaluated.size(), 0U);
  ASSERT_EQ(transport->requests.size(), 1U);
  const HttpRequest& request = transport->requests.at(0);
  const JsonValue    body    = JsonValue::parse(std::string(request.body.begin(), request.body.end()));
  EXPECT_EQ(body.at("additionalUtxoSet").size(), 1U);
  EXPECT_EQ(body.at("additionalUtxoSet").at(0).at(0).at("index").asUInt(), 2U);
  EXPECT_EQ(body.at("additionalUtxoSet").at(0).at(1).at("value").at("coins").asUInt(), 3000000U);
}

TEST(BlockfrostProvider, AppliesEvaluatedExecutionBudget)
{
  // Arrange
  const std::shared_ptr<FakeHttpTransport> transport = std::make_shared<FakeHttpTransport>();
  transport->expected.push_back({ "POST", "utils/txs/evaluate", 200, R"({"result":{"EvaluationResult":{"spend:0":{"memory":111,"steps":222}}}})" });
  const BlockfrostProvider provider(NetworkMagic::Preprod, "test-project-id", transport);
  Transaction              transaction = sampleTransaction();
  WitnessSet               witnesses   = transaction.witnessSet();
  RedeemerList             redeemers;
  redeemers.add(Redeemer(RedeemerTag::Spend, 0, PlutusData(0), ExUnits(0, 0)));
  witnesses.setRedeemers(redeemers);
  transaction.setWitnessSet(witnesses);

  // Act
  const RedeemerList evaluated = provider.evaluateTransaction(transaction, {});

  // Assert
  ASSERT_EQ(evaluated.size(), 1U);
  EXPECT_EQ(evaluated.at(0).exUnits().memory(), 111U);
  EXPECT_EQ(evaluated.at(0).exUnits().cpuSteps(), 222U);
}
