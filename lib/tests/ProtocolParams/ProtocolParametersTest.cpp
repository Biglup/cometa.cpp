/**
 * @file ProtocolParametersTest.cpp
 *
 * @brief Unit tests for current protocol parameters.
 *
 * @author angel.castillo
 * @date   Nov 02, 2026
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

#include <gtest/gtest.h>

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ProtocolParameters, CoversEveryField)
{
  // Arrange
  ProtocolParameters       parameters;
  const ProtocolParameters defaults(parameters);
  const UnitInterval       half(1, 2);

  // Act
  parameters.setMinFeeA(std::uint64_t(42));
  parameters.setMinFeeB(std::uint64_t(42));
  parameters.setMaxBlockBodySize(std::uint64_t(42));
  parameters.setMaxTxSize(std::uint64_t(42));
  parameters.setMaxBlockHeaderSize(std::uint64_t(42));
  parameters.setKeyDeposit(std::uint64_t(42));
  parameters.setPoolDeposit(std::uint64_t(42));
  parameters.setMaxEpoch(std::uint64_t(42));
  parameters.setNOpt(std::uint64_t(42));
  parameters.setPoolPledgeInfluence(half);
  parameters.setExpansionRate(half);
  parameters.setTreasuryGrowthRate(half);
  parameters.setD(half);
  parameters.setExtraEntropy(std::vector<std::uint8_t> { 1, 2, 3 });
  parameters.setProtocolVersion(ProtocolVersion(12, 0));
  parameters.setMinPoolCost(std::uint64_t(42));
  parameters.setAdaPerUtxoByte(std::uint64_t(42));
  parameters.setCostModels(CostModels());
  parameters.setExecutionCosts(ExUnitPrices(half, half));
  parameters.setMaxTxExUnits(ExUnits(10, 20));
  parameters.setMaxBlockExUnits(ExUnits(10, 20));
  parameters.setMaxValueSize(std::uint64_t(42));
  parameters.setCollateralPercentage(std::uint64_t(42));
  parameters.setMaxCollateralInputs(std::uint64_t(42));
  parameters.setPoolVotingThresholds(defaults.poolVotingThresholds());
  parameters.setDrepVotingThresholds(defaults.drepVotingThresholds());
  parameters.setMinCommitteeSize(std::uint64_t(42));
  parameters.setCommitteeTermLimit(std::uint64_t(42));
  parameters.setGovernanceActionValidityPeriod(std::uint64_t(42));
  parameters.setGovernanceActionDeposit(std::uint64_t(42));
  parameters.setDrepDeposit(std::uint64_t(42));
  parameters.setDrepInactivityPeriod(std::uint64_t(42));
  parameters.setRefScriptCostPerByte(half);
  parameters.setMaxRefScriptSizePerBlock(std::uint64_t(42));
  parameters.setMaxRefScriptSizePerTx(std::uint64_t(42));
  parameters.setRefScriptCostStride(std::uint64_t(42));
  parameters.setRefScriptCostMultiplier(half);
  parameters.setMaxPledgeLeverage(half);
  parameters.setMinPoolMargin(half);
  parameters.setLeiosAnnouncementPeriodLength(std::uint64_t(42));
  parameters.setLeiosVotePeriodLength(std::uint64_t(42));
  parameters.setLeiosDiffusionPeriodLength(std::uint64_t(42));
  parameters.setLeiosCommitteeSize(std::uint64_t(42));
  parameters.setLeiosQuorumStakeThreshold(half);
  parameters.setMaxEndorserBlockReferencesSize(std::uint64_t(42));
  parameters.setMaxEndorserBlockTxsSize(std::uint64_t(42));
  parameters.setMaxEndorserBlockExecutionUnits(ExUnits(10, 20));
  parameters.setMaxRefScriptSizePerEndorserBlock(std::uint64_t(42));
  parameters.setPerasMinCandidateBlockAge(std::uint64_t(42));
  parameters.setPerasHealingFactor(half);
  parameters.setPerasCertBoost(std::uint64_t(42));
  parameters.setPerasTargetCommitteeSize(std::uint64_t(42));
  parameters.setPerasBootstrapRound(std::uint64_t(42));
  parameters.setPerasQuorumThresholdSafetyMargin(half);
  parameters.setRefInputsCostPerMultiAssetPolicy(std::uint64_t(42));
  parameters.setRefInputsCostPerDatumByte(std::uint64_t(42));

  // Assert
  EXPECT_EQ(parameters.minFeeA(), std::uint64_t(42));
  EXPECT_EQ(parameters.minFeeB(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxBlockBodySize(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxTxSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxBlockHeaderSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.keyDeposit(), std::uint64_t(42));
  EXPECT_EQ(parameters.poolDeposit(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxEpoch(), std::uint64_t(42));
  EXPECT_EQ(parameters.nOpt(), std::uint64_t(42));
  EXPECT_EQ(parameters.poolPledgeInfluence(), half);
  EXPECT_EQ(parameters.expansionRate(), half);
  EXPECT_EQ(parameters.treasuryGrowthRate(), half);
  EXPECT_EQ(parameters.d(), half);
  EXPECT_EQ(parameters.extraEntropy(), (std::vector<std::uint8_t> { 1, 2, 3 }));
  EXPECT_EQ(parameters.protocolVersion(), ProtocolVersion(12, 0));
  EXPECT_EQ(parameters.minPoolCost(), std::uint64_t(42));
  EXPECT_EQ(parameters.adaPerUtxoByte(), std::uint64_t(42));
  EXPECT_EQ(parameters.costModels(), CostModels());
  EXPECT_EQ(parameters.executionCosts(), ExUnitPrices(half, half));
  EXPECT_EQ(parameters.maxTxExUnits(), ExUnits(10, 20));
  EXPECT_EQ(parameters.maxBlockExUnits(), ExUnits(10, 20));
  EXPECT_EQ(parameters.maxValueSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.collateralPercentage(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxCollateralInputs(), std::uint64_t(42));
  EXPECT_EQ(parameters.poolVotingThresholds(), defaults.poolVotingThresholds());
  EXPECT_EQ(parameters.drepVotingThresholds(), defaults.drepVotingThresholds());
  EXPECT_EQ(parameters.minCommitteeSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.committeeTermLimit(), std::uint64_t(42));
  EXPECT_EQ(parameters.governanceActionValidityPeriod(), std::uint64_t(42));
  EXPECT_EQ(parameters.governanceActionDeposit(), std::uint64_t(42));
  EXPECT_EQ(parameters.drepDeposit(), std::uint64_t(42));
  EXPECT_EQ(parameters.drepInactivityPeriod(), std::uint64_t(42));
  EXPECT_EQ(parameters.refScriptCostPerByte(), half);
  EXPECT_EQ(parameters.maxRefScriptSizePerBlock(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxRefScriptSizePerTx(), std::uint64_t(42));
  EXPECT_EQ(parameters.refScriptCostStride(), std::uint64_t(42));
  EXPECT_EQ(parameters.refScriptCostMultiplier(), half);
  EXPECT_EQ(parameters.maxPledgeLeverage(), half);
  EXPECT_EQ(parameters.minPoolMargin(), half);
  EXPECT_EQ(parameters.leiosAnnouncementPeriodLength(), std::uint64_t(42));
  EXPECT_EQ(parameters.leiosVotePeriodLength(), std::uint64_t(42));
  EXPECT_EQ(parameters.leiosDiffusionPeriodLength(), std::uint64_t(42));
  EXPECT_EQ(parameters.leiosCommitteeSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.leiosQuorumStakeThreshold(), half);
  EXPECT_EQ(parameters.maxEndorserBlockReferencesSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxEndorserBlockTxsSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.maxEndorserBlockExecutionUnits(), ExUnits(10, 20));
  EXPECT_EQ(parameters.maxRefScriptSizePerEndorserBlock(), std::uint64_t(42));
  EXPECT_EQ(parameters.perasMinCandidateBlockAge(), std::uint64_t(42));
  EXPECT_EQ(parameters.perasHealingFactor(), half);
  EXPECT_EQ(parameters.perasCertBoost(), std::uint64_t(42));
  EXPECT_EQ(parameters.perasTargetCommitteeSize(), std::uint64_t(42));
  EXPECT_EQ(parameters.perasBootstrapRound(), std::optional<std::uint64_t>(42));
  EXPECT_EQ(parameters.perasQuorumThresholdSafetyMargin(), half);
  EXPECT_EQ(parameters.refInputsCostPerMultiAssetPolicy(), std::uint64_t(42));
  EXPECT_EQ(parameters.refInputsCostPerDatumByte(), std::uint64_t(42));
  EXPECT_EQ(ProtocolParameters(parameters), parameters);
  EXPECT_NE(parameters, defaults);
}

TEST(ProtocolParameters, CopiesAndNullableFieldsAreIndependent)
{
  // Arrange
  ProtocolParameters parameters;
  parameters.setMinFeeA(44);
  parameters.setPoolPledgeInfluence(UnitInterval(1, 3));
  parameters.setMaxPledgeLeverage(UnitInterval(1, 2));
  parameters.setRefScriptCostMultiplier(UnitInterval(1, 3));
  parameters.setPerasHealingFactor(UnitInterval(1, 4));
  parameters.setPerasBootstrapRound(std::uint64_t(9));
  const ProtocolParameters copied(parameters);
  ProtocolParameters       assigned;

  // Act
  assigned              = parameters;
  UnitInterval returned = parameters.poolPledgeInfluence();
  returned.setDenominator(4);
  parameters.setMinFeeA(99);
  parameters.setMaxPledgeLeverage(std::nullopt);
  parameters.setRefScriptCostMultiplier(std::nullopt);
  parameters.setPerasHealingFactor(std::nullopt);
  parameters.setPerasBootstrapRound(std::nullopt);

  // Assert
  EXPECT_EQ(copied, assigned);
  EXPECT_NE(parameters.poolPledgeInfluence(), returned);
  EXPECT_EQ(copied.minFeeA(), 44);
  EXPECT_TRUE(copied.maxPledgeLeverage().has_value());
  EXPECT_TRUE(copied.refScriptCostMultiplier().has_value());
  EXPECT_TRUE(copied.perasHealingFactor().has_value());
  EXPECT_EQ(copied.perasBootstrapRound(), std::optional<std::uint64_t>(9));
  EXPECT_FALSE(parameters.maxPledgeLeverage().has_value());
  EXPECT_FALSE(parameters.refScriptCostMultiplier().has_value());
  EXPECT_FALSE(parameters.perasHealingFactor().has_value());
  EXPECT_FALSE(parameters.perasBootstrapRound().has_value());
  EXPECT_THROW(parameters.setMaxValueSize(std::numeric_limits<std::uint64_t>::max()), std::out_of_range);
}
