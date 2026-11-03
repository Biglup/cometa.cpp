/**
 * @file LifecycleConformanceTest.cpp
 *
 * @brief Exercises ownership and callback lifetimes from the shared binding corpus.
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

#include <cstdint>
#include <fstream>
#include <iterator>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Loads a shared corpus document. @param name The corpus file. @return Its JSON value. */
JsonValue
loadCorpus(const char* name)
{
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/" + name, std::ios::binary);

  if (!input)
  {
    throw std::runtime_error("Cannot open the binding conformance corpus");
  }

  const std::string document((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  return JsonValue::parse(document);
}

/** @brief Finds the iteration count for one named lifecycle case. @param name The case name. @return The count. */
std::size_t
iterations(const char* name)
{
  const JsonValue cases = loadCorpus("lifecycle.json").at("cases");

  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("id").asString() == name)
    {
      if (!item.at("expected").at("passed").asBool())
      {
        throw std::runtime_error("Unexpected lifecycle expectation");
      }

      return item.at("input").at("iterations").asUInt();
    }
  }

  throw std::runtime_error(std::string("Missing lifecycle case: ") + name);
}

/** @brief Makes the offline protocol parameters used by callback probes. @return The parameters. */
ProtocolParameters
parameters()
{
  const JsonValue           config = loadCorpus("builders.json").at("config");
  const JsonValue           values = config.at("plutus_v4_costs");
  std::vector<std::int64_t> costs;

  for (std::size_t index = 0; index < values.size(); ++index)
  {
    costs.push_back(values.at(index).asInt());
  }

  CostModels models;
  models.insert(CostModel(PlutusLanguageVersion::V4, costs));

  ProtocolParameters result;
  result.setMinFeeA(44);
  result.setMinFeeB(155381);
  result.setAdaPerUtxoByte(4310);
  result.setExecutionCosts(ExUnitPrices(
    UnitInterval::fromDouble(0.0577), UnitInterval::fromDouble(0.0000721)));
  result.setMaxTxExUnits(ExUnits(14000000, 10000000000));
  result.setProtocolVersion(ProtocolVersion(12, 0));
  result.setCostModels(models);
  return result;
}

/** @brief Makes a deferred V4 mint builder. @param params Protocol parameters. @return The builder. */
SubTransactionBuilder
mintBuilder(const ProtocolParameters& params)
{
  SubTransactionBuilder builder(params, SlotConfig::forNetwork(NetworkMagic::Preprod));
  builder.addScript(Script(PlutusV4Script::fromHex("450100002499")));
  return builder;
}

/** @brief Asset minted by the shared deferred-redeemer case. @return The asset. */
AssetId
mintedAsset()
{
  return AssetId::fromHex("1fd0190f1816f66a28208ce5836f64f54d99bdc2a088fee8e14547edaa");
}

/** @brief Creates a minimal transaction for ownership probes. @return The transaction. */
Transaction
minimalTransaction()
{
  return Transaction(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(LifecycleConformance, OwnedGetterSurvivesItsParent)
{
  const std::size_t count = iterations("owned-getter");

  for (std::size_t index = 0; index < count; ++index)
  {
    std::optional<TransactionBody> retained;

    {
      const Transaction parent = minimalTransaction();
      retained.emplace(parent.body());
    }

    ASSERT_TRUE(retained.has_value());
    EXPECT_EQ(TransactionBody::fromCbor(retained->toCbor()), *retained);
  }
}

TEST(LifecycleConformance, ReleasedAliasLeavesIndependentCopy)
{
  const std::size_t count = iterations("released-alias");

  for (std::size_t index = 0; index < count; ++index)
  {
    std::optional<Credential> retained;

    {
      const Credential source(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
      retained.emplace(source);
    }

    EXPECT_EQ(Credential::fromCbor(retained->toCbor()), *retained);
  }
}

TEST(LifecycleConformance, RepeatedBuildSignAndInspect)
{
  const std::size_t       count = iterations("repeated-operations");
  const Ed25519PrivateKey key   = Ed25519PrivateKey::fromNormalHex(
    "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");
  const Credential credential(CredentialType::KeyHash, key.publicKey().hash());
  const Address    address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  const Utxo       utxo(
    TransactionInput::fromHex(std::string(64, '1'), 0), TransactionOutput(address, Value(100000000)));
  ProtocolParameters params;
  params.setMinFeeA(44);
  params.setMinFeeB(155381);
  params.setAdaPerUtxoByte(4310);

  for (std::size_t index = 0; index < count; ++index)
  {
    TransactionBuilder builder(params, SlotConfig::forNetwork(NetworkMagic::Preprod));
    builder.setUtxos(UtxoList { utxo }).setChangeAddress(address).sendLovelace(address, 2000000);

    Transaction    transaction = builder.build();
    VkeyWitnessSet witnesses;
    witnesses.add(VkeyWitness(key.publicKey(), key.sign(transaction.id().toBytes())));
    transaction.applyVkeyWitnesses(witnesses);

    EXPECT_EQ(Transaction::fromCbor(transaction.toCbor()), transaction);
    EXPECT_FALSE(transaction.toJson().empty());
  }
}

TEST(LifecycleConformance, PartialConstructionRejectsBadInput)
{
  const std::size_t count = iterations("partial-construction");

  for (std::size_t index = 0; index < count; ++index)
  {
    const Credential owner(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
    EXPECT_THROW(Transaction::fromCbor("ff"), std::exception);
    EXPECT_THROW(Blake2bHash::fromHex("invalid"), std::exception);
    EXPECT_FALSE(owner.toCbor().empty());
  }
}

TEST(LifecycleConformance, DeferredCallbackErrorReleasesPartialBuild)
{
  const std::size_t        count  = iterations("callback-errors");
  const ProtocolParameters params = parameters();

  for (std::size_t index = 0; index < count; ++index)
  {
    bool                  called  = false;
    SubTransactionBuilder builder = mintBuilder(params);
    builder.mintTokenWithDeferredRedeemer(
      mintedAsset(), 1, [&called](const Transaction&, const UtxoList&) -> PlutusData
      {
        called = true;
        throw std::runtime_error("Lifecycle callback failed"); });

    EXPECT_THROW(builder.build(), std::runtime_error);
    EXPECT_TRUE(called);
  }
}

TEST(LifecycleConformance, BuilderRetainsCallbackUntilDestruction)
{
  const std::size_t        count  = iterations("callback-retention");
  const ProtocolParameters params = parameters();

  for (std::size_t index = 0; index < count; ++index)
  {
    std::shared_ptr<std::size_t>     state = std::make_shared<std::size_t>(0);
    const std::weak_ptr<std::size_t> weak  = state;

    {
      SubTransactionBuilder builder = mintBuilder(params);
      builder.mintTokenWithDeferredRedeemer(
        mintedAsset(), 1, [state](const Transaction&, const UtxoList&)
        {
          ++*state;
          return PlutusData(0); });

      state.reset();

      const SubTransaction               transaction = builder.build();
      const std::shared_ptr<std::size_t> observed    = weak.lock();
      ASSERT_NE(observed, nullptr);
      EXPECT_FALSE(transaction.toCbor().empty());
      EXPECT_GT(*observed, 0U);
    }

    EXPECT_TRUE(weak.expired());
  }
}

TEST(LifecycleConformance, SynchronousCallbackFinishesBeforeBuildReturns)
{
  const std::size_t        count  = iterations("cancellation");
  const ProtocolParameters params = parameters();

  for (std::size_t index = 0; index < count; ++index)
  {
    bool                  invoked = false;
    SubTransactionBuilder builder = mintBuilder(params);
    builder.mintTokenWithDeferredRedeemer(
      mintedAsset(), 1, [&invoked](const Transaction&, const UtxoList&)
      {
        invoked = true;
        return PlutusData(0); });

    const SubTransaction transaction = builder.build();
    EXPECT_TRUE(invoked);
    EXPECT_FALSE(transaction.toCbor().empty());
  }
}

TEST(LifecycleConformance, RepeatedBalancingProducesStableResults)
{
  const std::size_t                        count  = iterations("repeated-balancing");
  const ProtocolParameters                 params = parameters();
  std::optional<std::vector<std::uint8_t>> expected;

  for (std::size_t index = 0; index < count; ++index)
  {
    std::size_t           calls   = 0;
    SubTransactionBuilder builder = mintBuilder(params);
    builder.mintTokenWithDeferredRedeemer(
      mintedAsset(), 1, [&calls](const Transaction&, const UtxoList&)
      {
        ++calls;
        return PlutusData(0); });

    const SubTransaction transaction = builder.build();
    ASSERT_GT(calls, 0U);

    if (!expected.has_value())
    {
      expected = transaction.toCbor();
    }
    else
    {
      EXPECT_EQ(transaction.toCbor(), *expected);
    }
  }
}
