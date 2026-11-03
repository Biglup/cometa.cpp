/**
 * @file BuilderConformanceTest.cpp
 *
 * @brief Checks C++ transaction builders against the shared native corpus.
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

#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Loads the shared builder vectors. @return The parsed document. */
JsonValue
loadBuilderVectors()
{
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/builders.json", std::ios::binary);

  if (!input)
  {
    throw std::runtime_error("Cannot open the native builder conformance corpus");
  }

  const std::string document((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  return JsonValue::parse(document);
}

/** @brief Encodes bytes as lowercase hexadecimal. @param bytes The bytes. @return The text. */
std::string
toHex(const std::vector<std::uint8_t>& bytes)
{
  constexpr char HEX[] = "0123456789abcdef";
  std::string    result;
  result.reserve(bytes.size() * 2);

  for (const std::uint8_t value: bytes)
  {
    result.push_back(HEX[value >> 4]);
    result.push_back(HEX[value & 15]);
  }

  return result;
}

/** @brief Configures the fixture's protocol parameters. @param config The fixture configuration. @return The parameters. */
ProtocolParameters
makeParameters(const JsonValue& config)
{
  const JsonValue    values = config.at("parameters");
  ProtocolParameters parameters;

  parameters.setMinFeeA(values.at("min_fee_a").asUInt());
  parameters.setMinFeeB(values.at("min_fee_b").asUInt());
  parameters.setMaxTxSize(values.at("max_tx_size").asUInt());
  parameters.setMaxValueSize(values.at("max_value_size").asUInt());
  parameters.setAdaPerUtxoByte(values.at("ada_per_utxo_byte").asUInt());
  parameters.setCollateralPercentage(values.at("collateral_percentage").asUInt());
  parameters.setMaxCollateralInputs(values.at("max_collateral_inputs").asUInt());
  parameters.setKeyDeposit(values.at("key_deposit").asUInt());
  parameters.setPoolDeposit(values.at("pool_deposit").asUInt());

  const JsonValue limits = config.at("max_tx_ex_units");
  parameters.setMaxTxExUnits(ExUnits(limits.at("memory").asUInt(), limits.at("steps").asUInt()));

  const JsonValue prices      = config.at("execution_prices");
  const JsonValue memoryPrice = prices.at("memory");
  const JsonValue stepsPrice  = prices.at("steps");
  parameters.setExecutionCosts(ExUnitPrices(
    UnitInterval(memoryPrice.at(0).asUInt(), memoryPrice.at(1).asUInt()),
    UnitInterval(stepsPrice.at(0).asUInt(), stepsPrice.at(1).asUInt())));

  const JsonValue           valuesV4 = config.at("plutus_v4_costs");
  std::vector<std::int64_t> costs;
  costs.reserve(valuesV4.size());

  for (std::size_t index = 0; index < valuesV4.size(); ++index)
  {
    costs.push_back(valuesV4.at(index).asInt());
  }

  CostModels models;
  models.insert(CostModel(PlutusLanguageVersion::V4, costs));
  parameters.setCostModels(models);

  return parameters;
}

/** @brief Builds the fixture's available outputs. @param entries The JSON entries. @return The outputs. */
UtxoList
makeUtxos(const JsonValue& entries)
{
  UtxoList utxos;

  for (std::size_t index = 0; index < entries.size(); ++index)
  {
    const JsonValue         item = entries.at(index);
    const TransactionInput  input(Blake2bHash::fromHex(item.at("tx_id").asString()), item.at("index").asUInt());
    const TransactionOutput output(Address::fromString(item.at("address").asString()), Value(item.at("coins").asUInt()));

    utxos.push_back(Utxo(input, output));
  }

  return utxos;
}

/** @brief Converts a key-hash guard from the corpus. @param step The guard operation. @return The credential. */
Credential
makeKeyGuard(const JsonValue& step)
{
  if (step.at("kind").asString() != "key")
  {
    throw std::runtime_error("Expected a key-hash guard");
  }

  return Credential(CredentialType::KeyHash, Blake2bHash::fromHex(step.at("hash").asString()));
}

/** @brief Builds and signs one shared guarded sub-transaction. @param item The corpus case. @param parameters The protocol parameters. @param slots The network slots. @return The signed sub-transaction. */
SubTransaction
buildGuardSub(const JsonValue& item, const ProtocolParameters& parameters, SlotConfig slots)
{
  const JsonValue       input = item.at("input");
  const JsonValue       steps = input.at("steps");
  SubTransactionBuilder builder(parameters, slots);

  for (std::size_t index = 0; index < steps.size(); ++index)
  {
    const JsonValue   step      = steps.at(index);
    const std::string operation = step.at("op").asString();

    if (operation == "send")
    {
      builder.sendLovelace(Address::fromString(step.at("address").asString()), step.at("amount").asUInt());
    }
    else if (operation == "guard")
    {
      builder.addGuard(makeKeyGuard(step));
    }
    else if (operation == "require_guard")
    {
      builder.requireTopLevelGuard(makeKeyGuard(step));
    }
    else
    {
      throw std::runtime_error("Unsupported shared sub-transaction operation: " + operation);
    }
  }

  SubTransaction          transaction = builder.build();
  const Ed25519PrivateKey key         = Ed25519PrivateKey::fromNormalHex(input.at("signing_seed").asString());
  VkeyWitnessSet          witnesses;
  witnesses.add(VkeyWitness(key.publicKey(), key.sign(transaction.id().toBytes())));
  transaction.applyVkeyWitnesses(witnesses);

  return transaction;
}

/** @brief Applies one supported shared builder operation. @param builder The builder. @param step The operation. */
void
applyStep(TransactionBuilder& builder, const JsonValue& step)
{
  const std::string operation = step.at("op").asString();

  if (operation == "send")
  {
    builder.sendLovelace(Address::fromString(step.at("address").asString()), step.at("amount").asUInt());
  }
  else if (operation == "deposit")
  {
    builder.addDirectDeposit(RewardAddress::fromBech32(step.at("address").asString()), step.at("amount").asUInt());
  }
  else if (operation == "withdraw")
  {
    builder.withdrawRewards(RewardAddress::fromBech32(step.at("address").asString()), step.at("amount").asInt());
  }
  else if (operation == "balance")
  {
    builder.addAccountBalanceInterval(
      RewardAddress::fromBech32(step.at("address").asString()),
      AccountBalanceInterval(step.at("lower").asUInt(), step.at("upper").asUInt()));
  }
  else if (operation == "starting_balance")
  {
    builder.addStartingAccountBalanceInterval(
      RewardAddress::fromBech32(step.at("address").asString()),
      AccountBalanceInterval::exact(step.at("exact").asUInt()));
  }
  else
  {
    throw std::runtime_error("Unsupported shared builder operation: " + operation);
  }
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(BuilderConformance, BuildsSharedPaymentAndAccountCases)
{
  // Arrange
  const JsonValue  document = loadBuilderVectors();
  const JsonValue  cases    = document.at("cases");
  const JsonValue  config   = document.at("config");
  const JsonValue  slots    = config.at("slot_config");
  const SlotConfig slotConfig {
    slots.at("zero_time").asUInt(), slots.at("zero_slot").asUInt(), slots.at("slot_length").asUInt()
  };
  std::size_t checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue   item = cases.at(index);
    const std::string id   = item.at("id").asString();

    if (id != "conway-payment" && id != "account-deposit" && id != "account-transfer" && id != "account-balances" && id != "insufficient-funds")
    {
      continue;
    }

    SCOPED_TRACE(id);

    const JsonValue    input      = item.at("input");
    ProtocolParameters parameters = makeParameters(config);

    if (input.contains("protocol_version"))
    {
      const JsonValue version = input.at("protocol_version");
      parameters.setProtocolVersion(ProtocolVersion(version.at(0).asUInt(), version.at(1).asUInt()));
    }
    else
    {
      const JsonValue version = config.at("protocol_version");
      parameters.setProtocolVersion(ProtocolVersion(version.at(0).asUInt(), version.at(1).asUInt()));
    }

    TransactionBuilder builder(parameters, slotConfig);
    builder.setUtxos(makeUtxos(input.at("utxos")))
      .setChangeAddress(Address::fromString(input.at("change_address").asString()));

    const JsonValue steps = input.at("steps");

    for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
    {
      applyStep(builder, steps.at(stepIndex));
    }

    if (id == "insufficient-funds")
    {
      try
      {
        builder.build();
        FAIL() << "Insufficient funds unexpectedly built a transaction";
      }
      catch (const Error& error)
      {
        EXPECT_EQ(error.code().value(), item.at("expected").at("native_error").asInt());
      }
    }
    else
    {
      const Transaction transaction = builder.build();
      const JsonValue   expected    = item.at("expected");

      EXPECT_EQ(toHex(transaction.toCbor()), expected.at("cbor").asString());
      EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), expected.at("inspection").toJson());
    }

    ++checked;
  }

  EXPECT_EQ(checked, 5U);
}

TEST(BuilderConformance, BuildsSharedGuardedSubTransactions)
{
  // Arrange
  const JsonValue  document = loadBuilderVectors();
  const JsonValue  cases    = document.at("cases");
  const JsonValue  config   = document.at("config");
  const JsonValue  slots    = config.at("slot_config");
  const SlotConfig slotConfig {
    slots.at("zero_time").asUInt(), slots.at("zero_slot").asUInt(), slots.at("slot_length").asUInt()
  };
  ProtocolParameters parameters = makeParameters(config);
  parameters.setProtocolVersion(ProtocolVersion(12, 0));
  std::size_t checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue   item = cases.at(index);
    const std::string id   = item.at("id").asString();

    if (id != "guard-sub" && id != "guard-sub-second")
    {
      continue;
    }

    SCOPED_TRACE(id);

    const SubTransaction transaction = buildGuardSub(item, parameters, slotConfig);
    const JsonValue      expected    = item.at("expected");

    EXPECT_EQ(toHex(transaction.toCbor()), expected.at("cbor").asString());
    EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), expected.at("inspection").toJson());
    EXPECT_FALSE(expected.at("callback_invoked").asBool());
    ++checked;
  }

  EXPECT_EQ(checked, 2U);
}

TEST(BuilderConformance, BuildsSharedSignedBatch)
{
  // Arrange
  const JsonValue  document = loadBuilderVectors();
  const JsonValue  cases    = document.at("cases");
  const JsonValue  config   = document.at("config");
  const JsonValue  slots    = config.at("slot_config");
  const SlotConfig slotConfig {
    slots.at("zero_time").asUInt(), slots.at("zero_slot").asUInt(), slots.at("slot_length").asUInt()
  };
  ProtocolParameters parameters = makeParameters(config);
  parameters.setProtocolVersion(ProtocolVersion(12, 0));

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("id").asString() != "signed-batch")
    {
      continue;
    }

    const JsonValue    input = item.at("input");
    TransactionBuilder builder(parameters, slotConfig);
    builder.setUtxos(makeUtxos(input.at("utxos")))
      .setChangeAddress(Address::fromString(input.at("change_address").asString()));

    const JsonValue steps = input.at("steps");

    for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
    {
      const JsonValue   step      = steps.at(stepIndex);
      const std::string operation = step.at("op").asString();

      if (operation == "guard")
      {
        builder.addGuard(makeKeyGuard(step));
      }
      else if (operation == "sub_transaction")
      {
        const std::string recipe = step.at("recipe").asString();
        bool              found  = false;

        for (std::size_t recipeIndex = 0; recipeIndex < cases.size(); ++recipeIndex)
        {
          const JsonValue candidate = cases.at(recipeIndex);

          if (candidate.at("id").asString() == recipe)
          {
            builder.addSubTransaction(buildGuardSub(candidate, parameters, slotConfig), UtxoList {});
            found = true;
            break;
          }
        }

        ASSERT_TRUE(found);
      }
      else
      {
        FAIL() << "Unsupported batch operation: " << operation;
      }
    }

    const Transaction transaction = builder.build();
    const JsonValue   expected    = item.at("expected");

    EXPECT_EQ(toHex(transaction.toCbor()), expected.at("cbor").asString());
    EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), expected.at("inspection").toJson());
    EXPECT_FALSE(expected.at("callback_invoked").asBool());
    return;
  }

  FAIL() << "Missing signed-batch vector";
}

TEST(BuilderConformance, BuildsSharedScriptGuard)
{
  // Arrange
  const JsonValue  document = loadBuilderVectors();
  const JsonValue  cases    = document.at("cases");
  const JsonValue  config   = document.at("config");
  const JsonValue  slots    = config.at("slot_config");
  const SlotConfig slotConfig {
    slots.at("zero_time").asUInt(), slots.at("zero_slot").asUInt(), slots.at("slot_length").asUInt()
  };
  ProtocolParameters parameters = makeParameters(config);
  parameters.setProtocolVersion(ProtocolVersion(12, 0));

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("id").asString() != "script-guard")
    {
      continue;
    }

    SubTransactionBuilder builder(parameters, slotConfig);
    const JsonValue       steps = item.at("input").at("steps");

    for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
    {
      const JsonValue   step      = steps.at(stepIndex);
      const std::string operation = step.at("op").asString();

      if (operation == "script")
      {
        ASSERT_EQ(step.at("language").asString(), "plutus_v4");
        builder.addScript(Script(PlutusV4Script::fromHex(step.at("bytes").asString())));
      }
      else if (operation == "guard")
      {
        ASSERT_EQ(step.at("kind").asString(), "script");
        builder.addGuard(
          Credential(CredentialType::ScriptHash, Blake2bHash::fromHex(step.at("hash").asString())),
          PlutusData(step.at("redeemer").asInt()));
      }
      else
      {
        FAIL() << "Unsupported script-guard operation: " << operation;
      }
    }

    const SubTransaction transaction = builder.build();
    const JsonValue      expected    = item.at("expected");

    EXPECT_EQ(toHex(transaction.toCbor()), expected.at("cbor").asString());
    EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), expected.at("inspection").toJson());
    EXPECT_FALSE(expected.at("callback_invoked").asBool());
    return;
  }

  FAIL() << "Missing script-guard vector";
}

TEST(BuilderConformance, BuildsSharedDeferredRedeemers)
{
  // Arrange
  const JsonValue  document = loadBuilderVectors();
  const JsonValue  cases    = document.at("cases");
  const JsonValue  config   = document.at("config");
  const JsonValue  slots    = config.at("slot_config");
  const SlotConfig slotConfig {
    slots.at("zero_time").asUInt(), slots.at("zero_slot").asUInt(), slots.at("slot_length").asUInt()
  };
  ProtocolParameters parameters = makeParameters(config);
  parameters.setProtocolVersion(ProtocolVersion(12, 0));
  std::size_t checked = 0;

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue   item = cases.at(index);
    const std::string id   = item.at("id").asString();

    if (id != "deferred-spend" && id != "deferred-mint" && id != "deferred-error" && id != "deferred-null")
    {
      continue;
    }

    SCOPED_TRACE(id);

    SubTransactionBuilder builder(parameters, slotConfig);
    const JsonValue       steps           = item.at("input").at("steps");
    bool                  callbackInvoked = false;

    for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
    {
      const JsonValue   step      = steps.at(stepIndex);
      const std::string operation = step.at("op").asString();

      if (operation == "script")
      {
        builder.addScript(Script(PlutusV4Script::fromHex(step.at("bytes").asString())));
      }
      else if (operation == "send")
      {
        builder.sendLovelace(Address::fromString(step.at("address").asString()), step.at("amount").asUInt());
      }
      else if (operation == "input" || operation == "mint")
      {
        const JsonValue   callback = step.at("callback");
        const std::string kind     = callback.at("kind").asString();
        DeferredRedeemer  producer = [&callbackInvoked, kind, callback](const Transaction&, const UtxoList&)
        {
          callbackInvoked = true;

          if (kind == "error" || kind == "null")
          {
            throw std::runtime_error("Deferred redeemer fixture failure");
          }

          return PlutusData(callback.at("value").asInt());
        };

        if (operation == "input")
        {
          const JsonValue        utxo = step.at("utxo");
          const TransactionInput input(
            Blake2bHash::fromHex(utxo.at("tx_id").asString()), utxo.at("index").asUInt());
          const TransactionOutput output(
            Address::fromString(utxo.at("address").asString()), Value(utxo.at("coins").asUInt()));
          builder.addInputWithDeferredRedeemer(Utxo(input, output), producer);
        }
        else
        {
          builder.mintTokenWithDeferredRedeemer(
            AssetId::fromHex(step.at("asset_id").asString()), step.at("amount").asInt(), producer);
        }
      }
      else
      {
        FAIL() << "Unsupported deferred operation: " << operation;
      }
    }

    const JsonValue expected = item.at("expected");

    if (id == "deferred-error" || id == "deferred-null")
    {
      EXPECT_THROW(builder.build(), std::runtime_error);
    }
    else
    {
      const SubTransaction transaction = builder.build();
      EXPECT_EQ(toHex(transaction.toCbor()), expected.at("cbor").asString());
      EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), expected.at("inspection").toJson());
    }

    EXPECT_EQ(callbackInvoked, expected.at("callback_invoked").asBool());
    ++checked;
  }

  EXPECT_EQ(checked, 4U);
}

TEST(BuilderConformance, BuildsSharedNativeGuardMint)
{
  // Arrange
  const JsonValue  document = loadBuilderVectors();
  const JsonValue  cases    = document.at("cases");
  const JsonValue  config   = document.at("config");
  const JsonValue  slots    = config.at("slot_config");
  const SlotConfig slotConfig {
    slots.at("zero_time").asUInt(), slots.at("zero_slot").asUInt(), slots.at("slot_length").asUInt()
  };
  ProtocolParameters parameters = makeParameters(config);
  parameters.setProtocolVersion(ProtocolVersion(12, 0));

  // Act & Assert
  for (std::size_t index = 0; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);

    if (item.at("id").asString() != "native-guard-mint")
    {
      continue;
    }

    const JsonValue    input = item.at("input");
    TransactionBuilder builder(parameters, slotConfig);
    builder.setUtxos(makeUtxos(input.at("utxos")))
      .setChangeAddress(Address::fromString(input.at("change_address").asString()));

    const JsonValue             steps = input.at("steps");
    std::optional<NativeScript> nativeScript;

    for (std::size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
    {
      const JsonValue   step      = steps.at(stepIndex);
      const std::string operation = step.at("op").asString();

      if (operation == "native_script")
      {
        nativeScript = NativeScript::fromCbor(step.at("cbor").asString());
        builder.addScript(Script(*nativeScript));
      }
      else if (operation == "guard")
      {
        builder.addGuard(makeKeyGuard(step));
      }
      else if (operation == "native_mint")
      {
        ASSERT_TRUE(nativeScript.has_value());
        builder.mintToken(
          AssetId(nativeScript->hash(), AssetName::fromHex(step.at("asset_name").asString())),
          step.at("amount").asInt());
      }
      else
      {
        FAIL() << "Unsupported native mint operation: " << operation;
      }
    }

    const Transaction transaction = builder.build();
    const JsonValue   expected    = item.at("expected");

    EXPECT_EQ(toHex(transaction.toCbor()), expected.at("cbor").asString());
    EXPECT_EQ(JsonValue::parse(transaction.toJson()).toJson(), expected.at("inspection").toJson());
    EXPECT_FALSE(expected.at("callback_invoked").asBool());
    return;
  }

  FAIL() << "Missing native-guard-mint vector";
}
