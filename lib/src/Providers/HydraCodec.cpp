/**
 * @file HydraCodec.cpp
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
#include <set>
#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
ProtocolParameters
remoteHydraParameters(const JsonValue& value)
{
  const std::uint64_t major = value.at("protocolVersion").at("major").asUInt();
  if (major < 9 || major > 11)
  {
    throw std::runtime_error("Unsupported Hydra ledger version");
  }
  JsonWriter models;
  models.writeStartObject();
  const JsonValue source = value.at("costModels");
  for (std::size_t i = 0; i < source.size(); ++i)
  {
    const std::string key = source.keyAt(i);
    if (key != "PlutusV1" && key != "PlutusV2" && key != "PlutusV3")
    {
      throw std::runtime_error("Unsupported Hydra script language");
    }
    models.writePropertyName("plutus:v" + key.substr(7));
    models.writeValue(source.valueAt(i));
  }
  models.writeEndObject();
  // Conway's ledger tier constants are absent from cardano-cli parameter JSON.
  const std::string normalized = remoteObject({ { "minFeeCoefficient", value.at("txFeePerByte").toJson() },
                                                { "minFeeConstant", remoteObject({ { "ada", remoteObject({ { "lovelace", value.at("txFeeFixed").toJson() } }) } }) },
                                                { "maxBlockBodySize", remoteObject({ { "bytes", value.at("maxBlockBodySize").toJson() } }) },
                                                { "maxTransactionSize", remoteObject({ { "bytes", value.at("maxTxSize").toJson() } }) },
                                                { "maxBlockHeaderSize", remoteObject({ { "bytes", value.at("maxBlockHeaderSize").toJson() } }) },
                                                { "stakeCredentialDeposit", remoteObject({ { "ada", remoteObject({ { "lovelace", value.at("stakeAddressDeposit").toJson() } }) } }) },
                                                { "stakePoolDeposit", remoteObject({ { "ada", remoteObject({ { "lovelace", value.at("stakePoolDeposit").toJson() } }) } }) },
                                                { "stakePoolRetirementEpochBound", value.at("poolRetireMaxEpoch").toJson() },
                                                { "desiredNumberOfStakePools", value.at("stakePoolTargetNum").toJson() },
                                                { "stakePoolPledgeInfluence", value.at("poolPledgeInfluence").toJson() },
                                                { "monetaryExpansion", value.at("monetaryExpansion").toJson() },
                                                { "treasuryExpansion", value.at("treasuryCut").toJson() },
                                                { "minStakePoolCost", remoteObject({ { "ada", remoteObject({ { "lovelace", value.at("minPoolCost").toJson() } }) } }) },
                                                { "minUtxoDepositCoefficient", value.at("utxoCostPerByte").toJson() },
                                                { "scriptExecutionPrices", remoteObject({ { "memory", value.at("executionUnitPrices").at("priceMemory").toJson() }, { "cpu", value.at("executionUnitPrices").at("priceSteps").toJson() } }) },
                                                { "maxExecutionUnitsPerTransaction", remoteObject({ { "memory", value.at("maxTxExecutionUnits").at("memory").toJson() }, { "cpu", value.at("maxTxExecutionUnits").at("steps").toJson() } }) },
                                                { "maxExecutionUnitsPerBlock", remoteObject({ { "memory", value.at("maxBlockExecutionUnits").at("memory").toJson() }, { "cpu", value.at("maxBlockExecutionUnits").at("steps").toJson() } }) },
                                                { "maxValueSize", remoteObject({ { "bytes", value.at("maxValueSize").toJson() } }) },
                                                { "collateralPercentage", value.at("collateralPercentage").toJson() },
                                                { "maxCollateralInputs", value.at("maxCollateralInputs").toJson() },
                                                { "version", remoteObject({ { "major", value.at("protocolVersion").at("major").toJson() }, { "minor", value.at("protocolVersion").at("minor").toJson() } }) },
                                                { "constitutionalCommitteeMinSize", value.at("committeeMinSize").toJson() },
                                                { "constitutionalCommitteeMaxTermLength", value.at("committeeMaxTermLength").toJson() },
                                                { "governanceActionLifetime", value.at("govActionLifetime").toJson() },
                                                { "governanceActionDeposit", remoteObject({ { "ada", remoteObject({ { "lovelace", value.at("govActionDeposit").toJson() } }) } }) },
                                                { "delegateRepresentativeDeposit", remoteObject({ { "ada", remoteObject({ { "lovelace", value.at("dRepDeposit").toJson() } }) } }) },
                                                { "delegateRepresentativeMaxIdleTime", value.at("dRepActivity").toJson() },
                                                { "minFeeReferenceScripts", remoteObject({ { "base", value.at("minFeeRefScriptCostPerByte").toJson() }, { "range", "25600" }, { "multiplier", remoteString("6/5") } }) },
                                                { "stakePoolVotingThresholds", remoteObject({ { "noConfidence", value.at("poolVotingThresholds").at("motionNoConfidence").toJson() }, { "constitutionalCommittee", remoteObject({ { "default", value.at("poolVotingThresholds").at("committeeNormal").toJson() }, { "stateOfNoConfidence", value.at("poolVotingThresholds").at("committeeNoConfidence").toJson() } }) }, { "hardForkInitiation", value.at("poolVotingThresholds").at("hardForkInitiation").toJson() }, { "protocolParametersUpdate", remoteObject({ { "security", value.at("poolVotingThresholds").at("ppSecurityGroup").toJson() } }) } }) },
                                                { "delegateRepresentativeVotingThresholds", remoteObject({ { "noConfidence", value.at("dRepVotingThresholds").at("motionNoConfidence").toJson() }, { "constitutionalCommittee", remoteObject({ { "default", value.at("dRepVotingThresholds").at("committeeNormal").toJson() }, { "stateOfNoConfidence", value.at("dRepVotingThresholds").at("committeeNoConfidence").toJson() } }) }, { "hardForkInitiation", value.at("dRepVotingThresholds").at("hardForkInitiation").toJson() }, { "constitution", value.at("dRepVotingThresholds").at("updateToConstitution").toJson() }, { "treasuryWithdrawals", value.at("dRepVotingThresholds").at("treasuryWithdrawal").toJson() }, { "protocolParametersUpdate", remoteObject({ { "network", value.at("dRepVotingThresholds").at("ppNetworkGroup").toJson() }, { "economic", value.at("dRepVotingThresholds").at("ppEconomicGroup").toJson() }, { "technical", value.at("dRepVotingThresholds").at("ppTechnicalGroup").toJson() }, { "governance", value.at("dRepVotingThresholds").at("ppGovGroup").toJson() } }) } }) },
                                                { "plutusCostModels", models.toString() } });
  return remoteParameters(JsonValue::parse(normalized));
}

UtxoList
remoteHydraOutputs(const JsonValue& value)
{
  if (value.type() != JsonType::Object)
  {
    throw std::runtime_error("Invalid Hydra UTxO map");
  }
  UtxoList              result;
  std::set<std::string> seen;
  for (std::size_t i = 0; i < value.size(); ++i)
  {
    const std::string key = value.keyAt(i);
    if (key.size() < 66 || key[64] != '#')
    {
      throw std::runtime_error("Invalid Hydra output reference");
    }
    const std::string indexText = key.substr(65);
    if (indexText.empty() || indexText.find_first_not_of("0123456789") != std::string::npos)
    {
      throw std::runtime_error("Invalid Hydra output index");
    }
    const std::uint64_t index = JsonValue::parse(remoteString(indexText)).asUInt();
    if (index > 65535)
    {
      throw std::runtime_error("Invalid Hydra output index");
    }
    const TransactionInput input(Blake2bHash::fromHex(key.substr(0, 64)), index);
    if (!seen.insert(remoteInputKey(input)).second)
    {
      throw std::runtime_error("Duplicate Hydra output reference");
    }
    const JsonValue       row     = value.valueAt(i);
    const JsonValue       amounts = row.at("value");
    Value                 amount(amounts.contains("lovelace") ? remoteQuantity(amounts.at("lovelace")) : 0);
    std::set<std::string> assets;
    for (std::size_t p = 0; p < amounts.size(); ++p)
    {
      const std::string policy = amounts.keyAt(p);
      if (policy == "lovelace")
      {
        continue;
      }
      if (policy.size() != 56)
      {
        throw std::runtime_error("Invalid Hydra policy");
      }
      const JsonValue tokens = amounts.valueAt(p);
      if (tokens.type() != JsonType::Object)
      {
        throw std::runtime_error("Invalid Hydra asset map");
      }
      for (std::size_t n = 0; n < tokens.size(); ++n)
      {
        const std::int64_t quantity = remoteQuantity(tokens.valueAt(n));
        if (quantity <= 0)
        {
          throw std::runtime_error("Invalid Hydra asset quantity");
        }
        const AssetId asset = AssetId::fromHex(policy + tokens.keyAt(n));
        if (!assets.insert(asset.toHex()).second)
        {
          throw std::runtime_error("Duplicate Hydra asset");
        }
        amount.addAsset(asset, quantity);
      }
    }
    TransactionOutput output(Address::fromString(row.at("address").asString()), amount);
    if (remotePresent(row, "inlineDatumRaw"))
    {
      if (remotePresent(row, "datumhash"))
      {
        throw std::runtime_error("Conflicting Hydra datum");
      }
      const PlutusData datum = PlutusData::fromCbor(row.at("inlineDatumRaw").asString());
      if (remotePresent(row, "inlineDatumhash") && Blake2bHash::compute(datum.toCbor()).toHex() != row.at("inlineDatumhash").asString())
      {
        throw std::runtime_error("Hydra datum hash mismatch");
      }
      output.setDatum(Datum(datum));
    }
    else
    {
      if (remotePresent(row, "inlineDatum") || remotePresent(row, "inlineDatumhash"))
      {
        throw std::runtime_error("Missing Hydra inline datum bytes");
      }
      if (remotePresent(row, "datumhash"))
      {
        const Blake2bHash hash = Blake2bHash::fromHex(row.at("datumhash").asString());
        if (hash.toBytes().size() != 32)
        {
          throw std::runtime_error("Invalid Hydra datum hash");
        }
        output.setDatum(Datum(hash));
      }
    }
    if (remotePresent(row, "referenceScript"))
    {
      const JsonValue   reference = row.at("referenceScript");
      const JsonValue   envelope  = reference.at("script");
      const std::string kind      = envelope.at("type").asString();
      const std::string language  = kind == "SimpleScript" ? "SimpleScriptLanguage" : "PlutusScriptLanguage " + kind;
      if (reference.at("scriptLanguage").asString() != language)
      {
        throw std::runtime_error("Hydra script language mismatch");
      }
      const std::string prefix = kind == "SimpleScript" ? "8200" : kind == "PlutusScriptV1" ? "8201"
        : kind == "PlutusScriptV2"                                                          ? "8202"
        : kind == "PlutusScriptV3"                                                          ? "8203"
                                                                                            : "";
      if (prefix.empty())
      {
        throw std::runtime_error("Unsupported Hydra script language");
      }
      output.setScriptRef(Script::fromCbor(prefix + envelope.at("cborHex").asString()));
    }
    result.push_back(Utxo(input, output));
  }
  return result;
}
} // namespace Cometa
