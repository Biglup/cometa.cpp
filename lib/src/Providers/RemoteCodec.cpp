/**
 * @file RemoteCodec.cpp
 *
 * @brief Private remote provider implementation.
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

#include "RemoteCodec.h"

#include <algorithm>
#include <cctype>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Rejects invalid server data without echoing it. */
[[noreturn]] void
invalid()
{
  throw std::runtime_error("Invalid provider response");
}

/** @brief Requires a JSON array. @param value JSON value. */
void
requireArray(const JsonValue& value)
{
  if (value.type() != JsonType::Array)
  {
    invalid();
  }
}

/** @brief Reads a bounded decimal natural. @param text Decimal digits. @return Value. */
std::uint64_t
decimal(const std::string& text)
{
  if (text.empty())
  {
    invalid();
  }
  std::uint64_t result = 0;
  for (const char digit: text)
  {
    if (digit < '0' || digit > '9' || result > (std::numeric_limits<std::uint64_t>::max() - (digit - '0')) / 10)
    {
      invalid();
    }
    result = result * 10 + (digit - '0');
  }
  return result;
}

/** @brief Converts an exact rational or decimal token. @param value Fraction. @return Reduced fraction. */
UnitInterval
fraction(const JsonValue& value)
{
  std::string       text     = value.type() == JsonType::String ? value.asString() : value.toJson();
  int               exponent = 0;
  const std::size_t marker   = text.find_first_of("eE");
  if (marker != std::string::npos)
  {
    const std::string suffix = text.substr(marker + 1);
    if (suffix.empty() || suffix.size() > 4)
    {
      invalid();
    }
    std::size_t consumed = 0;
    try
    {
      exponent = std::stoi(suffix, &consumed);
    }
    catch (const std::exception&)
    {
      invalid();
    }
    if (consumed != suffix.size() || exponent < -100 || exponent > 100)
    {
      invalid();
    }
    text.resize(marker);
  }
  const std::size_t slash       = text.find('/');
  std::uint64_t     numerator   = 0;
  std::uint64_t     denominator = 1;
  if (slash != std::string::npos)
  {
    numerator   = decimal(text.substr(0, slash));
    denominator = decimal(text.substr(slash + 1));
  }
  else
  {
    const std::size_t point = text.find('.');
    if (point == std::string::npos)
    {
      numerator = decimal(text);
    }
    else
    {
      numerator = decimal(text.substr(0, point) + text.substr(point + 1));
      for (std::size_t i = point + 1; i < text.size(); ++i)
      {
        if (denominator > std::numeric_limits<std::uint64_t>::max() / 10)
        {
          invalid();
        }
        denominator *= 10;
      }
    }
  }
  if (denominator == 0)
  {
    invalid();
  }
  const std::uint64_t divisor = std::gcd(numerator, denominator);
  numerator                   /= divisor;
  denominator                 /= divisor;
  while (exponent != 0)
  {
    std::uint64_t&      multiply = exponent > 0 ? numerator : denominator;
    std::uint64_t&      divide   = exponent > 0 ? denominator : numerator;
    const std::uint64_t common   = std::gcd(divide, std::uint64_t(10));
    divide                       /= common;
    if (multiply > std::numeric_limits<std::uint64_t>::max() / (10 / common))
    {
      invalid();
    }
    multiply *= 10 / common;
    exponent += exponent > 0 ? -1 : 1;
  }
  return UnitInterval(numerator, denominator);
}

/** @brief Validates an exact fraction in [0, 1]. @param value JSON fraction. @return Validated interval. */
UnitInterval
interval(const JsonValue& value)
{
  const UnitInterval result = fraction(value);
  if (result.numerator() > result.denominator())
  {
    invalid();
  }
  return result;
}

/** @brief Reads a required field. @param object Source. @param name Field. @return Value. */
JsonValue
field(const JsonValue& object, const std::string& name)
{
  return object.at(name);
}

/** @brief Reads nested lovelace. @param value Amount. @return Lovelace. */
std::uint64_t
lovelace(const JsonValue& value)
{
  return value.at("ada").at("lovelace").asUInt();
}

/** @brief Parses execution resources. @param value Resources. @return Memory/CPU budget. */
ExUnits
units(const JsonValue& value)
{
  return ExUnits(value.at("memory").asUInt(), value.at("cpu").asUInt());
}

/** @brief Reads a protocol cost model map. @param value Models. @return Models. */
CostModels
costModels(const JsonValue& value)
{
  CostModels                                         result;
  const std::map<std::string, PlutusLanguageVersion> languages = std::map<std::string, PlutusLanguageVersion> { { "plutus:v1", PlutusLanguageVersion::V1 }, { "plutus:v2", PlutusLanguageVersion::V2 }, { "plutus:v3", PlutusLanguageVersion::V3 }, { "plutus:v4", PlutusLanguageVersion::V4 } };
  for (std::size_t i = 0; i < value.size(); ++i)
  {
    const std::map<std::string, PlutusLanguageVersion>::const_iterator language = languages.find(value.keyAt(i));
    if (language == languages.end())
    {
      invalid();
    }
    const JsonValue entries = value.valueAt(i);
    requireArray(entries);
    std::vector<std::int64_t> costs;
    for (std::size_t j = 0; j < entries.size(); ++j)
    {
      costs.push_back(entries.at(j).asInt());
    }
    result.insert(CostModel(language->second, costs));
  }
  return result;
}

/** @brief Maps only documented redeemer purposes. @param name API name. @return Ledger tag. */
RedeemerTag
redeemerTag(const std::string& name)
{
  if (name == "spend")
  {
    return RedeemerTag::Spend;
  }
  if (name == "mint")
  {
    return RedeemerTag::Mint;
  }
  if (name == "publish" || name == "cert")
  {
    return RedeemerTag::Certifying;
  }
  if (name == "withdraw" || name == "reward")
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
  invalid();
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::string
remoteString(const std::string& value)
{
  JsonWriter writer;
  writer.writeString(value);
  return writer.toString();
}

std::string
remoteObject(std::initializer_list<std::pair<std::string, std::string>> fields)
{
  JsonWriter writer;
  writer.writeStartObject();
  for (const std::pair<std::string, std::string>& entry: fields)
  {
    writer.writePropertyName(entry.first);
    writer.writeRaw(entry.second);
  }
  writer.writeEndObject();
  return writer.toString();
}

std::string
remoteHex(const std::vector<std::uint8_t>& bytes)
{
  constexpr char HEX[] = "0123456789abcdef";
  std::string    result;
  result.reserve(bytes.size() * 2);
  for (const std::uint8_t byte: bytes)
  {
    result += HEX[byte >> 4];
    result += HEX[byte & 15];
  }
  return result;
}

std::vector<std::uint8_t>
remoteBytes(const std::string& value)
{
  if (value.empty() || value.size() % 2 != 0)
  {
    invalid();
  }
  std::vector<std::uint8_t> result;
  for (std::size_t i = 0; i < value.size(); i += 2)
  {
    unsigned byte = 0;
    for (std::size_t j = i; j < i + 2; ++j)
    {
      const char digit = value[j];
      byte             *= 16;
      if (digit >= '0' && digit <= '9')
      {
        byte += digit - '0';
      }
      else if (digit >= 'a' && digit <= 'f')
      {
        byte += digit - 'a' + 10;
      }
      else if (digit >= 'A' && digit <= 'F')
      {
        byte += digit - 'A' + 10;
      }
      else
      {
        invalid();
      }
    }
    result.push_back(static_cast<std::uint8_t>(byte));
  }
  return result;
}

JsonValue
remoteJson(const HttpResponse& response)
{
  if (response.status < 200 || response.status >= 300)
  {
    throw std::runtime_error("Provider HTTP status " + std::to_string(response.status));
  }
  if (response.body.size() > 8 * 1024 * 1024)
  {
    throw std::runtime_error("Provider response exceeds 8 MiB");
  }
  return JsonValue::parse(std::string(response.body.begin(), response.body.end()));
}

bool
remotePresent(const JsonValue& value, const std::string& key)
{
  return value.contains(key) && value.at(key).type() != JsonType::Null;
}

std::int64_t
remoteQuantity(const JsonValue& value)
{
  const std::uint64_t amount = value.asUInt();
  if (amount > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
  {
    invalid();
  }
  return static_cast<std::int64_t>(amount);
}

std::string
remoteInputKey(const TransactionInput& input)
{
  return input.id().toHex() + "#" + std::to_string(input.index());
}

ProtocolParameters
remoteParameters(const JsonValue& value)
{
  ProtocolParameters result;
  result.setMinFeeA(field(value, "minFeeCoefficient").asUInt());
  result.setMaxEpoch(field(value, "stakePoolRetirementEpochBound").asUInt());
  result.setNOpt(field(value, "desiredNumberOfStakePools").asUInt());
  result.setAdaPerUtxoByte(field(value, "minUtxoDepositCoefficient").asUInt());
  result.setCollateralPercentage(field(value, "collateralPercentage").asUInt());
  result.setMaxCollateralInputs(field(value, "maxCollateralInputs").asUInt());
  result.setMinCommitteeSize(field(value, "constitutionalCommitteeMinSize").asUInt());
  result.setCommitteeTermLimit(field(value, "constitutionalCommitteeMaxTermLength").asUInt());
  result.setGovernanceActionValidityPeriod(field(value, "governanceActionLifetime").asUInt());
  result.setDrepInactivityPeriod(field(value, "delegateRepresentativeMaxIdleTime").asUInt());
  result.setMinFeeB(lovelace(field(value, "minFeeConstant")));
  result.setKeyDeposit(lovelace(field(value, "stakeCredentialDeposit")));
  result.setPoolDeposit(lovelace(field(value, "stakePoolDeposit")));
  result.setMinPoolCost(lovelace(field(value, "minStakePoolCost")));
  result.setGovernanceActionDeposit(lovelace(field(value, "governanceActionDeposit")));
  result.setDrepDeposit(lovelace(field(value, "delegateRepresentativeDeposit")));
  result.setMaxBlockBodySize(field(value, "maxBlockBodySize").at("bytes").asUInt());
  result.setMaxBlockHeaderSize(field(value, "maxBlockHeaderSize").at("bytes").asUInt());
  result.setMaxTxSize(field(value, "maxTransactionSize").at("bytes").asUInt());
  result.setMaxValueSize(field(value, "maxValueSize").at("bytes").asUInt());
  result.setPoolPledgeInfluence(fraction(field(value, "stakePoolPledgeInfluence")));
  result.setExpansionRate(interval(field(value, "monetaryExpansion")));
  result.setTreasuryGrowthRate(interval(field(value, "treasuryExpansion")));
  const JsonValue version = field(value, "version");
  result.setProtocolVersion(ProtocolVersion(version.at("major").asUInt(), version.at("minor").asUInt()));
  result.setCostModels(costModels(field(value, "plutusCostModels")));
  result.setMaxTxExUnits(units(field(value, "maxExecutionUnitsPerTransaction")));
  result.setMaxBlockExUnits(units(field(value, "maxExecutionUnitsPerBlock")));
  const JsonValue prices = field(value, "scriptExecutionPrices");
  result.setExecutionCosts(ExUnitPrices(fraction(prices.at("memory")), fraction(prices.at("cpu"))));
  const JsonValue pool = field(value, "stakePoolVotingThresholds");
  const JsonValue pc   = field(pool, "constitutionalCommittee");
  result.setPoolVotingThresholds(PoolVotingThresholds(
    interval(field(pool, "noConfidence")), interval(pc.at("default")), interval(field(pc, "stateOfNoConfidence")), interval(field(pool, "hardForkInitiation")), interval(field(pool, "protocolParametersUpdate").at("security"))));
  const JsonValue drep = field(value, "delegateRepresentativeVotingThresholds");
  const JsonValue dc   = field(drep, "constitutionalCommittee");
  const JsonValue dp   = field(drep, "protocolParametersUpdate");
  result.setDrepVotingThresholds(DRepVotingThresholds(
    interval(field(drep, "noConfidence")), interval(dc.at("default")), interval(field(dc, "stateOfNoConfidence")), interval(drep.at("constitution")), interval(field(drep, "hardForkInitiation")), interval(dp.at("network")), interval(dp.at("economic")), interval(dp.at("technical")), interval(dp.at("governance")), interval(field(drep, "treasuryWithdrawals"))));
  const JsonValue fees = field(value, "minFeeReferenceScripts");
  result.setRefScriptCostPerByte(fraction(fees.at("base")));
  result.setRefScriptCostStride(fees.at("range").asUInt());
  result.setRefScriptCostMultiplier(fraction(fees.at("multiplier")));
  if (remotePresent(value, "maxReferenceScriptsSizePerTransaction"))
  {
    result.setMaxRefScriptSizePerTx(field(value, "maxReferenceScriptsSizePerTransaction").at("bytes").asUInt());
  }
  if (remotePresent(value, "maxReferenceScriptsSizePerBlock"))
  {
    result.setMaxRefScriptSizePerBlock(field(value, "maxReferenceScriptsSizePerBlock").at("bytes").asUInt());
  }
  return result;
}

Script
remoteScript(const std::string& kind, const std::string& bytes, const std::string& hash)
{
  const std::vector<std::uint8_t> raw = remoteBytes(bytes);
  std::optional<Script>           result;
  if (kind == "native" || kind == "timelock")
  {
    result = Script(NativeScript::fromCbor(raw));
  }
  else if (kind == "plutusV1" || kind == "plutus:v1")
  {
    result = Script(PlutusV1Script(raw));
  }
  else if (kind == "plutusV2" || kind == "plutus:v2")
  {
    result = Script(PlutusV2Script(raw));
  }
  else if (kind == "plutusV3" || kind == "plutus:v3")
  {
    result = Script(PlutusV3Script(raw));
  }
  else if (kind == "plutusV4" || kind == "plutus:v4")
  {
    result = Script(PlutusV4Script(raw));
  }
  else
  {
    throw std::runtime_error("Unsupported reference-script language");
  }
  if (result->hash().toHex() != hash)
  {
    invalid();
  }
  return *result;
}

PlutusData
remoteDatum(const std::string& bytes, const Blake2bHash& hash)
{
  const std::vector<std::uint8_t> raw = remoteBytes(bytes);
  if (hash.toBytes().size() != 32 || Blake2bHash::compute(raw) != hash)
  {
    invalid();
  }
  return PlutusData::fromCbor(raw);
}

RedeemerList
remoteBudgets(const Transaction& transaction, const JsonValue& result)
{
  requireArray(result);
  RedeemerList                                    original = transaction.witnessSet().redeemers().value_or(RedeemerList());
  std::set<std::pair<RedeemerTag, std::uint64_t>> pending;
  for (const Redeemer& redeemer: original)
  {
    pending.insert({ redeemer.tag(), redeemer.index() });
  }
  for (std::size_t i = 0; i < result.size(); ++i)
  {
    const JsonValue     row       = result.at(i);
    const JsonValue     validator = row.at("validator");
    const JsonValue     budget    = row.at("budget");
    const RedeemerTag   tag       = redeemerTag(validator.at("purpose").asString());
    const std::uint64_t index     = validator.at("index").asUInt();
    if (pending.erase({ tag, index }) != 1)
    {
      invalid();
    }
    original.setExUnits(tag, index, ExUnits(budget.at("memory").asUInt(), budget.at("cpu").asUInt()));
  }
  if (!pending.empty())
  {
    invalid();
  }
  return original;
}

std::string
remoteOgmiosOutput(const Utxo& value)
{
  const TransactionOutput output = value.output();
  const Value             amount = output.amount();
  if (amount.coin() < 0)
  {
    throw std::invalid_argument("Negative additional output quantity");
  }
  std::map<std::string, std::map<std::string, std::int64_t>> policies;
  for (const std::pair<AssetId, std::int64_t>& entry: amount.toAssetMap().entries())
  {
    if (entry.first.isLovelace())
    {
      continue;
    }
    if (entry.second <= 0)
    {
      throw std::invalid_argument("Nonpositive additional asset quantity");
    }
    const std::string unit                        = entry.first.toHex();
    policies[unit.substr(0, 56)][unit.substr(56)] = entry.second;
  }
  JsonWriter writer;
  writer.writeStartObject();
  writer.writePropertyName("transaction");
  writer.writeRaw(remoteObject({ { "id", remoteString(value.input().id().toHex()) } }));
  writer.writePropertyName("index");
  writer.writeUInt(value.input().index());
  writer.writePropertyName("address");
  writer.writeString(output.address().toString());
  writer.writePropertyName("value");
  writer.writeStartObject();
  writer.writePropertyName("ada");
  writer.writeRaw(remoteObject({ { "lovelace", std::to_string(amount.coin()) } }));
  for (const std::pair<const std::string, std::map<std::string, std::int64_t>>& policy: policies)
  {
    writer.writePropertyName(policy.first);
    writer.writeStartObject();
    for (const std::pair<const std::string, std::int64_t>& asset: policy.second)
    {
      writer.writePropertyName(asset.first);
      writer.writeInt(asset.second);
    }
    writer.writeEndObject();
  }
  writer.writeEndObject();
  const std::optional<Datum> datum = output.datum();
  if (datum)
  {
    const std::optional<PlutusData> inlineData = datum->inlineData();
    if (inlineData)
    {
      writer.writePropertyName("datum");
      writer.writeString(remoteHex(inlineData->toCbor()));
    }
    else
    {
      writer.writePropertyName("datumHash");
      writer.writeString(datum->hash().toHex());
    }
  }
  const std::optional<Script> script = output.scriptRef();
  if (script)
  {
    std::string               language;
    std::vector<std::uint8_t> bytes;
    switch (script->language())
    {
      case ScriptLanguage::Native:
        language = "native";
        bytes    = script->asNative().toCbor();
        break;
      case ScriptLanguage::PlutusV1:
        language = "plutus:v1";
        bytes    = script->asPlutusV1().toBytes();
        break;
      case ScriptLanguage::PlutusV2:
        language = "plutus:v2";
        bytes    = script->asPlutusV2().toBytes();
        break;
      case ScriptLanguage::PlutusV3:
        language = "plutus:v3";
        bytes    = script->asPlutusV3().toBytes();
        break;
      case ScriptLanguage::PlutusV4:
        language = "plutus:v4";
        bytes    = script->asPlutusV4().toBytes();
        break;
    }
    writer.writePropertyName("script");
    writer.writeRaw(remoteObject({ { "language", remoteString(language) }, { "cbor", remoteString(remoteHex(bytes)) } }));
  }
  writer.writeEndObject();
  return writer.toString();
}

} // namespace Cometa
