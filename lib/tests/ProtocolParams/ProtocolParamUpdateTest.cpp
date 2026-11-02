/**
 * @file ProtocolParamUpdateTest.cpp
 *
 * @brief Unit tests for optional protocol parameter changes.
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

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ProtocolParamUpdate, MatchesNativeScalarCborFixture)
{
  // Arrange
  ProtocolParamUpdate update;
  CborReader          reader("a10018c8");

  // Act
  update.setMinFeeA(std::uint64_t(200));
  const ProtocolParamUpdate decoded = ProtocolParamUpdate::fromCbor("a10018c8");

  // Assert
  EXPECT_EQ(update.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded.minFeeA(), std::optional<std::uint64_t>(200));
  EXPECT_EQ(decoded, update);
  EXPECT_FALSE(update.toJson().empty());
  EXPECT_FALSE(update.toPlutusData().toCbor().empty());

  // Act
  update.setMinFeeA(std::nullopt);

  // Assert
  EXPECT_FALSE(update.minFeeA().has_value());
  EXPECT_EQ(update, ProtocolParamUpdate::fromCbor("a0"));
}

TEST(ProtocolParamUpdate, PreservesExplicitLedgerNull)
{
  // Arrange
  ProtocolParamUpdate leverage;
  ProtocolParamUpdate bootstrap;
  CborReader          leverageReader("a11826f6");
  CborReader          bootstrapReader("a11835f6");

  // Act
  leverage.setMaxPledgeLeverageUnbounded();
  bootstrap.setPerasBootstrapRoundNull();

  // Assert
  EXPECT_EQ(leverage.toCbor(), leverageReader.readEncodedValue());
  EXPECT_TRUE(leverage.hasMaxPledgeLeverage());
  EXPECT_TRUE(leverage.isMaxPledgeLeverageUnbounded());
  EXPECT_EQ(bootstrap.toCbor(), bootstrapReader.readEncodedValue());
  EXPECT_TRUE(bootstrap.hasPerasBootstrapRound());
  EXPECT_TRUE(bootstrap.isPerasBootstrapRoundNull());
  EXPECT_EQ(ProtocolParamUpdate::fromCbor("a11826f6"), leverage);
  EXPECT_EQ(ProtocolParamUpdate::fromCbor("a11835f6"), bootstrap);

  // Act
  leverage.clearMaxPledgeLeverage();
  bootstrap.clearPerasBootstrapRound();

  // Assert
  EXPECT_FALSE(leverage.hasMaxPledgeLeverage());
  EXPECT_FALSE(bootstrap.hasPerasBootstrapRound());
}

TEST(ProtocolParamUpdate, OptionalFieldsRoundTripIndependently)
{
  // Arrange
  ProtocolParamUpdate update;
  CostModels          models;
  models.insert(CostModel(PlutusLanguageVersion::V1, { 1, 2, 3 }));
  const DRepVotingThresholds drep = DRepVotingThresholds::fromCbor("8ad81e820000d81e820101d81e820202d81e820303d81e820404d81e820505d81e820606d81e820707d81e820808d81e820909");
  const PoolVotingThresholds pool = PoolVotingThresholds::fromCbor("85d81e820000d81e820101d81e820202d81e820303d81e820404");

  // Act
  update.setMinFeeA(std::uint64_t(1));
  update.setMinFeeB(std::uint64_t(1));
  update.setMaxBlockBodySize(std::uint64_t(1));
  update.setMaxTxSize(std::uint64_t(1));
  update.setMaxBlockHeaderSize(std::uint64_t(1));
  update.setKeyDeposit(std::uint64_t(1));
  update.setPoolDeposit(std::uint64_t(1));
  update.setMaxEpoch(std::uint64_t(1));
  update.setNOpt(std::uint64_t(1));
  update.setPoolPledgeInfluence(UnitInterval(1, 2));
  update.setExpansionRate(UnitInterval(1, 2));
  update.setTreasuryGrowthRate(UnitInterval(1, 2));
  update.setD(UnitInterval(1, 2));
  update.setExtraEntropy(std::vector<std::uint8_t>(32, 0));
  update.setProtocolVersion(ProtocolVersion(12, 0));
  update.setMinPoolCost(std::uint64_t(1));
  update.setAdaPerUtxoByte(std::uint64_t(1));
  update.setCostModels(models);
  update.setExecutionCosts(ExUnitPrices(UnitInterval(1, 2), UnitInterval(1, 3)));
  update.setMaxTxExUnits(ExUnits(1, 2));
  update.setMaxBlockExUnits(ExUnits(1, 2));
  update.setMaxValueSize(std::uint64_t(1));
  update.setCollateralPercentage(std::uint64_t(1));
  update.setMaxCollateralInputs(std::uint64_t(1));
  update.setPoolVotingThresholds(pool);
  update.setDrepVotingThresholds(drep);
  update.setMinCommitteeSize(std::uint64_t(1));
  update.setCommitteeTermLimit(std::uint64_t(1));
  update.setGovernanceActionValidityPeriod(std::uint64_t(1));
  update.setGovernanceActionDeposit(std::uint64_t(1));
  update.setDrepDeposit(std::uint64_t(1));
  update.setDrepInactivityPeriod(std::uint64_t(1));
  update.setRefScriptCostPerByte(UnitInterval(1, 2));
  update.setMaxRefScriptSizePerBlock(std::uint64_t(1));
  update.setMaxRefScriptSizePerTx(std::uint64_t(1));
  update.setRefScriptCostStride(std::uint64_t(1));
  update.setRefScriptCostMultiplier(UnitInterval(1, 2));
  update.setMaxPledgeLeverage(UnitInterval(1, 2));
  update.setMinPoolMargin(UnitInterval(1, 2));
  update.setLeiosAnnouncementPeriodLength(std::uint64_t(1));
  update.setLeiosVotePeriodLength(std::uint64_t(1));
  update.setLeiosDiffusionPeriodLength(std::uint64_t(1));
  update.setLeiosCommitteeSize(std::uint64_t(1));
  update.setLeiosQuorumStakeThreshold(UnitInterval(1, 2));
  update.setMaxEndorserBlockReferencesSize(std::uint64_t(1));
  update.setMaxEndorserBlockTxsSize(std::uint64_t(1));
  update.setMaxEndorserBlockExecutionUnits(ExUnits(1, 2));
  update.setMaxRefScriptSizePerEndorserBlock(std::uint64_t(1));
  update.setPerasMinCandidateBlockAge(std::uint64_t(1));
  update.setPerasHealingFactor(UnitInterval(1, 2));
  update.setPerasCertBoost(std::uint64_t(1));
  update.setPerasTargetCommitteeSize(std::uint64_t(1));
  update.setPerasBootstrapRound(std::uint64_t(1));
  update.setPerasQuorumThresholdSafetyMargin(UnitInterval(1, 2));
  update.setRefInputsCostPerMultiAssetPolicy(std::uint64_t(1));
  update.setRefInputsCostPerDatumByte(std::uint64_t(1));

  const ProtocolParamUpdate copied(update);
  ProtocolParamUpdate       assigned;
  assigned = update;

  // Assert
  ASSERT_TRUE(update.minFeeA().has_value());
  EXPECT_EQ(*update.minFeeA(), std::uint64_t(1));
  ASSERT_TRUE(update.minFeeB().has_value());
  EXPECT_EQ(*update.minFeeB(), std::uint64_t(1));
  ASSERT_TRUE(update.maxBlockBodySize().has_value());
  EXPECT_EQ(*update.maxBlockBodySize(), std::uint64_t(1));
  ASSERT_TRUE(update.maxTxSize().has_value());
  EXPECT_EQ(*update.maxTxSize(), std::uint64_t(1));
  ASSERT_TRUE(update.maxBlockHeaderSize().has_value());
  EXPECT_EQ(*update.maxBlockHeaderSize(), std::uint64_t(1));
  ASSERT_TRUE(update.keyDeposit().has_value());
  EXPECT_EQ(*update.keyDeposit(), std::uint64_t(1));
  ASSERT_TRUE(update.poolDeposit().has_value());
  EXPECT_EQ(*update.poolDeposit(), std::uint64_t(1));
  ASSERT_TRUE(update.maxEpoch().has_value());
  EXPECT_EQ(*update.maxEpoch(), std::uint64_t(1));
  ASSERT_TRUE(update.nOpt().has_value());
  EXPECT_EQ(*update.nOpt(), std::uint64_t(1));
  ASSERT_TRUE(update.poolPledgeInfluence().has_value());
  EXPECT_EQ(*update.poolPledgeInfluence(), UnitInterval(1, 2));
  ASSERT_TRUE(update.expansionRate().has_value());
  EXPECT_EQ(*update.expansionRate(), UnitInterval(1, 2));
  ASSERT_TRUE(update.treasuryGrowthRate().has_value());
  EXPECT_EQ(*update.treasuryGrowthRate(), UnitInterval(1, 2));
  ASSERT_TRUE(update.d().has_value());
  EXPECT_EQ(*update.d(), UnitInterval(1, 2));
  ASSERT_TRUE(update.extraEntropy().has_value());
  EXPECT_EQ(*update.extraEntropy(), std::vector<std::uint8_t>(32, 0));
  ASSERT_TRUE(update.protocolVersion().has_value());
  EXPECT_EQ(*update.protocolVersion(), ProtocolVersion(12, 0));
  ASSERT_TRUE(update.minPoolCost().has_value());
  EXPECT_EQ(*update.minPoolCost(), std::uint64_t(1));
  ASSERT_TRUE(update.adaPerUtxoByte().has_value());
  EXPECT_EQ(*update.adaPerUtxoByte(), std::uint64_t(1));
  ASSERT_TRUE(update.costModels().has_value());
  EXPECT_EQ(*update.costModels(), models);
  ASSERT_TRUE(update.executionCosts().has_value());
  EXPECT_EQ(*update.executionCosts(), ExUnitPrices(UnitInterval(1, 2), UnitInterval(1, 3)));
  ASSERT_TRUE(update.maxTxExUnits().has_value());
  EXPECT_EQ(*update.maxTxExUnits(), ExUnits(1, 2));
  ASSERT_TRUE(update.maxBlockExUnits().has_value());
  EXPECT_EQ(*update.maxBlockExUnits(), ExUnits(1, 2));
  ASSERT_TRUE(update.maxValueSize().has_value());
  EXPECT_EQ(*update.maxValueSize(), std::uint64_t(1));
  ASSERT_TRUE(update.collateralPercentage().has_value());
  EXPECT_EQ(*update.collateralPercentage(), std::uint64_t(1));
  ASSERT_TRUE(update.maxCollateralInputs().has_value());
  EXPECT_EQ(*update.maxCollateralInputs(), std::uint64_t(1));
  ASSERT_TRUE(update.poolVotingThresholds().has_value());
  EXPECT_EQ(*update.poolVotingThresholds(), pool);
  ASSERT_TRUE(update.drepVotingThresholds().has_value());
  EXPECT_EQ(*update.drepVotingThresholds(), drep);
  ASSERT_TRUE(update.minCommitteeSize().has_value());
  EXPECT_EQ(*update.minCommitteeSize(), std::uint64_t(1));
  ASSERT_TRUE(update.committeeTermLimit().has_value());
  EXPECT_EQ(*update.committeeTermLimit(), std::uint64_t(1));
  ASSERT_TRUE(update.governanceActionValidityPeriod().has_value());
  EXPECT_EQ(*update.governanceActionValidityPeriod(), std::uint64_t(1));
  ASSERT_TRUE(update.governanceActionDeposit().has_value());
  EXPECT_EQ(*update.governanceActionDeposit(), std::uint64_t(1));
  ASSERT_TRUE(update.drepDeposit().has_value());
  EXPECT_EQ(*update.drepDeposit(), std::uint64_t(1));
  ASSERT_TRUE(update.drepInactivityPeriod().has_value());
  EXPECT_EQ(*update.drepInactivityPeriod(), std::uint64_t(1));
  ASSERT_TRUE(update.refScriptCostPerByte().has_value());
  EXPECT_EQ(*update.refScriptCostPerByte(), UnitInterval(1, 2));
  ASSERT_TRUE(update.maxRefScriptSizePerBlock().has_value());
  EXPECT_EQ(*update.maxRefScriptSizePerBlock(), std::uint64_t(1));
  ASSERT_TRUE(update.maxRefScriptSizePerTx().has_value());
  EXPECT_EQ(*update.maxRefScriptSizePerTx(), std::uint64_t(1));
  ASSERT_TRUE(update.refScriptCostStride().has_value());
  EXPECT_EQ(*update.refScriptCostStride(), std::uint64_t(1));
  ASSERT_TRUE(update.refScriptCostMultiplier().has_value());
  EXPECT_EQ(*update.refScriptCostMultiplier(), UnitInterval(1, 2));
  ASSERT_TRUE(update.maxPledgeLeverage().has_value());
  EXPECT_EQ(*update.maxPledgeLeverage(), UnitInterval(1, 2));
  ASSERT_TRUE(update.minPoolMargin().has_value());
  EXPECT_EQ(*update.minPoolMargin(), UnitInterval(1, 2));
  ASSERT_TRUE(update.leiosAnnouncementPeriodLength().has_value());
  EXPECT_EQ(*update.leiosAnnouncementPeriodLength(), std::uint64_t(1));
  ASSERT_TRUE(update.leiosVotePeriodLength().has_value());
  EXPECT_EQ(*update.leiosVotePeriodLength(), std::uint64_t(1));
  ASSERT_TRUE(update.leiosDiffusionPeriodLength().has_value());
  EXPECT_EQ(*update.leiosDiffusionPeriodLength(), std::uint64_t(1));
  ASSERT_TRUE(update.leiosCommitteeSize().has_value());
  EXPECT_EQ(*update.leiosCommitteeSize(), std::uint64_t(1));
  ASSERT_TRUE(update.leiosQuorumStakeThreshold().has_value());
  EXPECT_EQ(*update.leiosQuorumStakeThreshold(), UnitInterval(1, 2));
  ASSERT_TRUE(update.maxEndorserBlockReferencesSize().has_value());
  EXPECT_EQ(*update.maxEndorserBlockReferencesSize(), std::uint64_t(1));
  ASSERT_TRUE(update.maxEndorserBlockTxsSize().has_value());
  EXPECT_EQ(*update.maxEndorserBlockTxsSize(), std::uint64_t(1));
  ASSERT_TRUE(update.maxEndorserBlockExecutionUnits().has_value());
  EXPECT_EQ(*update.maxEndorserBlockExecutionUnits(), ExUnits(1, 2));
  ASSERT_TRUE(update.maxRefScriptSizePerEndorserBlock().has_value());
  EXPECT_EQ(*update.maxRefScriptSizePerEndorserBlock(), std::uint64_t(1));
  ASSERT_TRUE(update.perasMinCandidateBlockAge().has_value());
  EXPECT_EQ(*update.perasMinCandidateBlockAge(), std::uint64_t(1));
  ASSERT_TRUE(update.perasHealingFactor().has_value());
  EXPECT_EQ(*update.perasHealingFactor(), UnitInterval(1, 2));
  ASSERT_TRUE(update.perasCertBoost().has_value());
  EXPECT_EQ(*update.perasCertBoost(), std::uint64_t(1));
  ASSERT_TRUE(update.perasTargetCommitteeSize().has_value());
  EXPECT_EQ(*update.perasTargetCommitteeSize(), std::uint64_t(1));
  ASSERT_TRUE(update.perasBootstrapRound().has_value());
  EXPECT_EQ(*update.perasBootstrapRound(), std::uint64_t(1));
  ASSERT_TRUE(update.perasQuorumThresholdSafetyMargin().has_value());
  EXPECT_EQ(*update.perasQuorumThresholdSafetyMargin(), UnitInterval(1, 2));
  ASSERT_TRUE(update.refInputsCostPerMultiAssetPolicy().has_value());
  EXPECT_EQ(*update.refInputsCostPerMultiAssetPolicy(), std::uint64_t(1));
  ASSERT_TRUE(update.refInputsCostPerDatumByte().has_value());
  EXPECT_EQ(*update.refInputsCostPerDatumByte(), std::uint64_t(1));
  EXPECT_EQ(copied, update);
  EXPECT_EQ(assigned, update);
  EXPECT_EQ(ProtocolParamUpdate::fromCbor(update.toCbor()), update);

  // Act
  update.setMinFeeA(std::uint64_t(200));

  // Assert
  EXPECT_EQ(copied.minFeeA(), std::optional<std::uint64_t>(1));
  EXPECT_EQ(assigned.minFeeA(), std::optional<std::uint64_t>(1));

  // Act
  update.setPoolPledgeInfluence(std::nullopt);
  update.setExtraEntropy(std::nullopt);

  // Assert
  EXPECT_FALSE(update.poolPledgeInfluence().has_value());
  EXPECT_FALSE(update.extraEntropy().has_value());
  EXPECT_TRUE(copied.poolPledgeInfluence().has_value());
}
