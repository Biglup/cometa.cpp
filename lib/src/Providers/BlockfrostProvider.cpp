/**
 * @file BlockfrostProvider.cpp
 *
 * @brief Blockfrost network provider.
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

#include <Cometa/Providers/BlockfrostProvider.h>

#include <Cometa/Assets/AssetIdMap.h>
#include <Cometa/Common/Datum.h>
#include <Cometa/Common/DatumType.h>
#include <Cometa/Common/ExUnits.h>
#include <Cometa/Common/ProtocolVersion.h>
#include <Cometa/Common/UnitInterval.h>
#include <Cometa/Json/JsonWriter.h>
#include <Cometa/ProtocolParams/CostModel.h>
#include <Cometa/ProtocolParams/CostModels.h>
#include <Cometa/ProtocolParams/ExUnitPrices.h>
#include <Cometa/Scripts/PlutusScripts/PlutusLanguageVersion.h>
#include <Cometa/TransactionBody/TransactionInput.h>
#include <Cometa/TransactionBody/TransactionOutput.h>
#include <Cometa/WitnessSet/RedeemerTag.h>

#include <algorithm>
#include <chrono>
#include <limits>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Converts an HTTP response body to UTF-8 text. @param response The response. @return The text. */
std::string
bodyText(const HttpResponse& response)
{
  return std::string(response.body.begin(), response.body.end());
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

/** @brief Parses the amount array of a Blockfrost output. @param amount The amount entries. @return The value. */
Value
parseAmount(const JsonValue& amount)
{
  AssetIdMap assets;

  for (std::size_t index = 0; index < amount.size(); ++index)
  {
    const JsonValue     entry    = amount.at(index);
    const std::string   unit     = entry.at("unit").asString();
    const std::uint64_t quantity = entry.at("quantity").asUInt();

    if (quantity > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
    {
      throw std::out_of_range("Blockfrost asset quantity exceeds signed 64-bit range");
    }

    assets.set(unit == "lovelace" ? AssetId::lovelace() : AssetId::fromHex(unit), static_cast<std::int64_t>(quantity));
  }

  return Value::fromAssetMap(assets);
}

/** @brief Converts an optional decimal price to an exact unit interval. @param value The JSON value. @return The interval. */
UnitInterval
parsePrice(const JsonValue& value)
{
  return UnitInterval::fromDouble(value.asDouble());
}

/** @brief Writes an additional UTxO for Blockfrost evaluation. @param writer The JSON writer. @param utxo The output. */
void
writeAdditionalUtxo(JsonWriter& writer, const Utxo& utxo)
{
  const TransactionInput  input  = utxo.input();
  const TransactionOutput output = utxo.output();
  const Value             value  = output.amount();

  writer.writeStartArray();
  writer.writeStartObject();
  writer.writePropertyName("txId");
  writer.writeString(input.id().toHex());
  writer.writePropertyName("index");
  writer.writeUInt(input.index());
  writer.writeEndObject();

  writer.writeStartObject();
  writer.writePropertyName("address");
  writer.writeString(output.address().toString());
  writer.writePropertyName("value");
  writer.writeStartObject();
  writer.writePropertyName("coins");
  writer.writeInt(value.coin());

  const AssetIdMap assets = value.toAssetMap();

  if (assets.size() > 1)
  {
    writer.writePropertyName("assets");
    writer.writeStartObject();

    for (const std::pair<AssetId, std::int64_t>& entry: assets.entries())
    {
      if (!entry.first.toHex().empty())
      {
        writer.writePropertyName(entry.first.toHex());
        writer.writeInt(entry.second);
      }
    }

    writer.writeEndObject();
  }

  writer.writeEndObject();

  const std::optional<Datum> datum = output.datum();

  if (datum.has_value())
  {
    if (datum->type() == DatumType::InlineData)
    {
      throw std::invalid_argument("Inline data in additional evaluation UTxOs is not supported");
    }

    writer.writePropertyName("datum_hash");
    writer.writeString(datum->hash().toHex());
  }

  writer.writeEndObject();
  writer.writeEndArray();
}

/** @brief Maps an Ogmios script-purpose name to a redeemer tag. @param name The purpose. @return The tag. */
RedeemerTag
parseRedeemerTag(const std::string& name)
{
  if (name == "spend")
  {
    return RedeemerTag::Spend;
  }

  if (name == "mint")
  {
    return RedeemerTag::Mint;
  }

  if (name == "certificate")
  {
    return RedeemerTag::Certifying;
  }

  if (name == "withdrawal")
  {
    return RedeemerTag::Reward;
  }

  if (name == "vote")
  {
    return RedeemerTag::Voting;
  }

  if (name == "propose")
  {
    return RedeemerTag::Proposing;
  }

  if (name == "guard")
  {
    return RedeemerTag::Guarding;
  }

  throw std::runtime_error("Unknown Blockfrost redeemer purpose");
}
} // namespace

/* IMPLEMENTATION ************************************************************/

BlockfrostProvider::BlockfrostProvider(
  NetworkMagic                   network,
  std::string                    projectId,
  std::shared_ptr<HttpTransport> transport)
: m_network(network)
, m_projectId(std::move(projectId))
, m_transport(std::move(transport))
{
  if (m_projectId.empty() || !m_transport)
  {
    throw std::invalid_argument("Blockfrost requires a project ID and HTTP transport");
  }

  if (m_projectId.find_first_of("\r\n") != std::string::npos)
  {
    throw std::invalid_argument("Blockfrost project ID contains a newline");
  }

  url("");
}

std::string
BlockfrostProvider::name() const
{
  return "blockfrost";
}

NetworkMagic
BlockfrostProvider::networkMagic() const
{
  return m_network;
}

std::string
BlockfrostProvider::url(const std::string& path) const
{
  switch (m_network)
  {
    case NetworkMagic::Mainnet:
      return "https://cardano-mainnet.blockfrost.io/api/v0/" + path;
    case NetworkMagic::Preprod:
      return "https://cardano-preprod.blockfrost.io/api/v0/" + path;
    case NetworkMagic::Preview:
      return "https://cardano-preview.blockfrost.io/api/v0/" + path;
    case NetworkMagic::SanchoNet:
      return "https://cardano-sanchonet.blockfrost.io/api/v0/" + path;
    default:
      throw std::invalid_argument("Unsupported Blockfrost network");
  }
}

HttpResponse
BlockfrostProvider::request(
  const std::string&               method,
  const std::string&               path,
  const std::vector<std::uint8_t>& body,
  const std::string&               contentType) const
{
  HttpRequest query { method, url(path), { { "project_id", m_projectId } }, body };

  if (!contentType.empty())
  {
    query.headers.emplace_back("Content-Type", contentType);
  }

  return m_transport->execute(query);
}

JsonValue
BlockfrostProvider::parseJson(const HttpResponse& response) const
{
  if (response.status != 200)
  {
    std::string message = "Blockfrost HTTP " + std::to_string(response.status);

    try
    {
      const JsonValue error = JsonValue::parse(bodyText(response));

      if (error.contains("message") && error.at("message").type() == JsonType::String)
      {
        message += ": " + error.at("message").asString();
      }
    }
    catch (const std::exception&)
    {
      // Keep the HTTP status when the response is not a Blockfrost JSON error.
    }

    throw std::runtime_error(message);
  }

  return JsonValue::parse(bodyText(response));
}

ProtocolParameters
BlockfrostProvider::getParameters() const
{
  const JsonValue    data = parseJson(request("GET", "epochs/latest/parameters"));
  ProtocolParameters parameters;

  parameters.setMinFeeA(data.at("min_fee_a").asUInt());
  parameters.setMinFeeB(data.at("min_fee_b").asUInt());
  parameters.setMaxTxSize(data.at("max_tx_size").asUInt());
  parameters.setMaxValueSize(data.at("max_val_size").asUInt());
  parameters.setAdaPerUtxoByte(data.at("coins_per_utxo_size").asUInt());
  parameters.setKeyDeposit(data.at("key_deposit").asUInt());
  parameters.setPoolDeposit(data.at("pool_deposit").asUInt());
  parameters.setProtocolVersion(
    ProtocolVersion(data.at("protocol_major_ver").asUInt(), data.at("protocol_minor_ver").asUInt()));

  if (data.contains("collateral_percent"))
  {
    parameters.setCollateralPercentage(data.at("collateral_percent").asUInt());
  }

  if (data.contains("max_collateral_inputs"))
  {
    parameters.setMaxCollateralInputs(data.at("max_collateral_inputs").asUInt());
  }

  if (data.contains("max_tx_ex_mem") && data.contains("max_tx_ex_steps"))
  {
    parameters.setMaxTxExUnits(ExUnits(data.at("max_tx_ex_mem").asUInt(), data.at("max_tx_ex_steps").asUInt()));
  }

  if (data.contains("price_mem") && data.contains("price_step"))
  {
    parameters.setExecutionCosts(ExUnitPrices(parsePrice(data.at("price_mem")), parsePrice(data.at("price_step"))));
  }

  if (data.contains("cost_models_raw") || data.contains("cost_models"))
  {
    const JsonValue                                     costData = data.contains("cost_models_raw") ? data.at("cost_models_raw") : data.at("cost_models");
    CostModels                                          models;
    const std::pair<const char*, PlutusLanguageVersion> languages[] = {
      { "PlutusV1", PlutusLanguageVersion::V1 },
      { "PlutusV2", PlutusLanguageVersion::V2 },
      { "PlutusV3", PlutusLanguageVersion::V3 },
      { "PlutusV4", PlutusLanguageVersion::V4 }
    };

    for (const std::pair<const char*, PlutusLanguageVersion>& language: languages)
    {
      if (!costData.contains(language.first))
      {
        continue;
      }

      const JsonValue values = costData.at(language.first);

      if (values.type() != JsonType::Array)
      {
        throw std::runtime_error("Blockfrost cost models require cost_models_raw arrays");
      }
      std::vector<std::int64_t> costs;
      costs.reserve(values.size());

      for (std::size_t index = 0; index < values.size(); ++index)
      {
        costs.push_back(values.at(index).asInt());
      }

      models.insert(CostModel(language.second, costs));
    }

    parameters.setCostModels(models);
  }

  if (data.contains("gov_action_deposit"))
  {
    parameters.setGovernanceActionDeposit(data.at("gov_action_deposit").asUInt());
  }

  if (data.contains("drep_deposit"))
  {
    parameters.setDrepDeposit(data.at("drep_deposit").asUInt());
  }

  if (data.contains("drep_activity"))
  {
    parameters.setDrepInactivityPeriod(data.at("drep_activity").asUInt());
  }

  if (data.contains("min_fee_ref_script_cost_per_byte"))
  {
    parameters.setRefScriptCostPerByte(UnitInterval(data.at("min_fee_ref_script_cost_per_byte").asUInt(), 1));
  }

  return parameters;
}

UtxoList
BlockfrostProvider::loadUtxos(const std::string& path) const
{
  UtxoList result;

  for (std::size_t page = 1;; ++page)
  {
    const HttpResponse response = request("GET", path + "?count=100&page=" + std::to_string(page));

    if (response.status == 404)
    {
      return result;
    }

    const JsonValue entries = parseJson(response);

    for (std::size_t index = 0; index < entries.size(); ++index)
    {
      const JsonValue entry = entries.at(index);
      result.push_back(parseUtxo(
        entry, Blake2bHash::fromHex(entry.at("tx_hash").asString()), entry.at("output_index").asUInt()));
    }

    if (entries.size() < 100)
    {
      return result;
    }
  }
}

Script
BlockfrostProvider::loadScript(const std::string& hash) const
{
  const std::string type = parseJson(request("GET", "scripts/" + hash)).at("type").asString();

  if (type == "timelock")
  {
    const JsonValue content = parseJson(request("GET", "scripts/" + hash + "/json")).at("json");
    return Script(NativeScript::fromJson(content.toJson()));
  }

  const std::string cbor = parseJson(request("GET", "scripts/" + hash + "/cbor")).at("cbor").asString();

  if (type == "plutusV1")
  {
    return Script(PlutusV1Script::fromHex(cbor));
  }

  if (type == "plutusV2")
  {
    return Script(PlutusV2Script::fromHex(cbor));
  }

  if (type == "plutusV3")
  {
    return Script(PlutusV3Script::fromHex(cbor));
  }

  if (type == "plutusV4")
  {
    return Script(PlutusV4Script::fromHex(cbor));
  }

  throw std::runtime_error("Unsupported Blockfrost script type: " + type);
}

Utxo
BlockfrostProvider::parseUtxo(const JsonValue& object, const Blake2bHash& id, std::uint64_t index) const
{
  TransactionOutput output(
    Address::fromString(object.at("address").asString()), parseAmount(object.at("amount")));

  if (object.contains("inline_datum") && object.at("inline_datum").type() == JsonType::String)
  {
    output.setDatum(Datum(PlutusData::fromCbor(object.at("inline_datum").asString())));
  }
  else if (object.contains("data_hash") && object.at("data_hash").type() == JsonType::String)
  {
    output.setDatum(Datum(Blake2bHash::fromHex(object.at("data_hash").asString())));
  }

  if (object.contains("reference_script_hash") && object.at("reference_script_hash").type() == JsonType::String)
  {
    const std::string hash   = object.at("reference_script_hash").asString();
    const Script      script = loadScript(hash);

    if (script.hash().toHex() != hash)
    {
      throw std::runtime_error("Blockfrost reference script hash mismatch");
    }

    output.setScriptRef(script);
  }

  return Utxo(TransactionInput(id, index), output);
}

UtxoList
BlockfrostProvider::getUnspentOutputs(const Address& address) const
{
  return loadUtxos("addresses/" + address.toString() + "/utxos");
}

std::uint64_t
BlockfrostProvider::getRewardsAvailable(const RewardAddress& address) const
{
  const HttpResponse response = request("GET", "accounts/" + address.toString());

  if (response.status == 404)
  {
    return 0;
  }

  return parseJson(response).at("withdrawable_amount").asUInt();
}

UtxoList
BlockfrostProvider::getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const
{
  if (asset.toHex().empty())
  {
    throw std::invalid_argument("An asset ID is required");
  }

  return loadUtxos("addresses/" + address.toString() + "/utxos/" + asset.toHex());
}

Utxo
BlockfrostProvider::getUnspentOutputByNft(const AssetId& asset) const
{
  if (asset.toHex().empty())
  {
    throw std::invalid_argument("An NFT asset ID is required");
  }

  const JsonValue holders = parseJson(request("GET", "assets/" + asset.toHex() + "/addresses"));

  if (holders.size() != 1 || holders.at(0).at("quantity").asUInt() != 1)
  {
    throw std::runtime_error("Asset is not held as one NFT");
  }

  const Address  address = Address::fromString(holders.at(0).at("address").asString());
  const UtxoList utxos   = getUnspentOutputsWithAsset(address, asset);

  for (const Utxo& utxo: utxos)
  {
    const std::optional<std::int64_t> quantity = utxo.output().amount().toAssetMap().get(asset);

    if (quantity.has_value() && *quantity == 1)
    {
      return utxo;
    }
  }

  throw std::runtime_error("NFT output was not found at its holder address");
}

UtxoList
BlockfrostProvider::resolveUnspentOutputs(const TransactionInputSet& inputs) const
{
  UtxoList resolved;

  for (std::size_t index = 0; index < inputs.size(); ++index)
  {
    const TransactionInput input   = inputs.at(index);
    const JsonValue        data    = parseJson(request("GET", "txs/" + input.id().toHex() + "/utxos"));
    const JsonValue        outputs = data.at("outputs");
    bool                   found   = false;

    for (std::size_t outputIndex = 0; outputIndex < outputs.size(); ++outputIndex)
    {
      const JsonValue entry = outputs.at(outputIndex);

      if (entry.at("output_index").asUInt() == input.index())
      {
        resolved.push_back(parseUtxo(entry, input.id(), input.index()));
        found = true;
        break;
      }
    }

    if (!found)
    {
      throw std::runtime_error("Blockfrost transaction output was not found");
    }
  }

  return resolved;
}

PlutusData
BlockfrostProvider::resolveDatum(const Blake2bHash& hash) const
{
  const JsonValue data = parseJson(request("GET", "scripts/datum/" + hash.toHex() + "/cbor"));
  return PlutusData::fromCbor(data.at("cbor").asString());
}

bool
BlockfrostProvider::confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const
{
  if (timeout < std::chrono::milliseconds::zero())
  {
    throw std::invalid_argument("Confirmation timeout cannot be negative");
  }

  const std::chrono::steady_clock::time_point deadline = std::chrono::steady_clock::now() + timeout;

  while (true)
  {
    const HttpResponse response = request("GET", "txs/" + id.toHex());

    if (response.status == 200)
    {
      return true;
    }

    if (response.status != 404)
    {
      parseJson(response);
    }

    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    if (now >= deadline)
    {
      return false;
    }

    std::this_thread::sleep_for(std::min(std::chrono::milliseconds(2000), std::chrono::duration_cast<std::chrono::milliseconds>(deadline - now)));
  }
}

Blake2bHash
BlockfrostProvider::submitTransaction(const Transaction& transaction) const
{
  const JsonValue data = parseJson(request("POST", "tx/submit", transaction.toCbor(), "application/cbor"));
  return Blake2bHash::fromHex(data.asString());
}

RedeemerList
BlockfrostProvider::evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const
{
  const std::optional<RedeemerList> original  = transaction.witnessSet().redeemers();
  RedeemerList                      redeemers = original.has_value() ? *original : RedeemerList();
  HttpResponse                      response;

  if (additionalUtxos.empty())
  {
    response = request("POST", "utils/txs/evaluate", transaction.toCbor(), "application/cbor");
  }
  else
  {
    JsonWriter writer;
    writer.writeStartObject();
    writer.writePropertyName("cbor");
    writer.writeString(toHex(transaction.toCbor()));
    writer.writePropertyName("additionalUtxoSet");
    writer.writeStartArray();

    for (const Utxo& utxo: additionalUtxos)
    {
      writeAdditionalUtxo(writer, utxo);
    }

    writer.writeEndArray();
    writer.writeEndObject();

    const std::string body = writer.toString();
    response               = request("POST", "utils/txs/evaluate/utxos", std::vector<std::uint8_t>(body.begin(), body.end()), "application/json");
  }

  const JsonValue data   = parseJson(response);
  const JsonValue result = data.at("result");

  if (result.contains("EvaluationFailure"))
  {
    throw std::runtime_error("Blockfrost script evaluation failed");
  }

  const JsonValue budgets = result.at("EvaluationResult");

  for (std::size_t index = 0; index < budgets.size(); ++index)
  {
    const std::string key       = budgets.keyAt(index);
    const std::size_t delimiter = key.find(':');

    if (delimiter == std::string::npos)
    {
      throw std::runtime_error("Invalid Blockfrost redeemer purpose");
    }

    const RedeemerTag   tag           = parseRedeemerTag(key.substr(0, delimiter));
    const std::string   indexText     = key.substr(delimiter + 1);
    std::size_t         consumed      = 0;
    const std::uint64_t redeemerIndex = std::stoull(indexText, &consumed);

    if (consumed != indexText.size())
    {
      throw std::runtime_error("Invalid Blockfrost redeemer index");
    }

    const JsonValue units = budgets.valueAt(index);
    redeemers.setExUnits(tag, redeemerIndex, ExUnits(units.at("memory").asUInt(), units.at("steps").asUInt()));
  }

  return redeemers;
}
