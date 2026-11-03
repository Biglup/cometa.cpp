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
      EXPECT_THROW(builder.build(), Error);
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
