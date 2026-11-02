/**
 * @file ProtocolParamUpdate.h
 *
 * @brief Optional protocol parameter changes.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Common/ExUnits.h>
#include <Cometa/Common/ProtocolVersion.h>
#include <Cometa/Common/UnitInterval.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/ProtocolParams/CostModels.h>
#include <Cometa/ProtocolParams/DRepVotingThresholds.h>
#include <Cometa/ProtocolParams/ExUnitPrices.h>
#include <Cometa/ProtocolParams/PoolVotingThresholds.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_protocol_param_update_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A set of proposed protocol parameter changes.
 *
 * Each absent parameter is omitted from the CBOR update. Passing std::nullopt
 * to a setter removes that parameter. Max pledge leverage and the Peras
 * bootstrap round also support an explicit ledger null state.
 */
class ProtocolParamUpdate
{
  public:

    /** @brief Creates an empty update. */
    ProtocolParamUpdate();

    /**
     * @brief Copies an update.
     * @param other The update to copy.
     */
    ProtocolParamUpdate(const ProtocolParamUpdate& other);

    /**
     * @brief Moves an update.
     * @param other The update to move.
     */
    ProtocolParamUpdate(ProtocolParamUpdate&& other) noexcept;

    /** @brief Destroys the update. */
    ~ProtocolParamUpdate();

  public:

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded update.
     */
    static ProtocolParamUpdate fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded update.
     */
    static ProtocolParamUpdate fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the proposed minFeeA value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> minFeeA() const;

    /**
     * @brief Proposes or removes the minFeeA value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMinFeeA(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed minFeeB value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> minFeeB() const;

    /**
     * @brief Proposes or removes the minFeeB value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMinFeeB(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxBlockBodySize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxBlockBodySize() const;

    /**
     * @brief Proposes or removes the maxBlockBodySize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxBlockBodySize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxTxSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxTxSize() const;

    /**
     * @brief Proposes or removes the maxTxSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxTxSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxBlockHeaderSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxBlockHeaderSize() const;

    /**
     * @brief Proposes or removes the maxBlockHeaderSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxBlockHeaderSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed keyDeposit value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> keyDeposit() const;

    /**
     * @brief Proposes or removes the keyDeposit value.
     * @param value The proposed value, or empty to remove it.
     */
    void setKeyDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed poolDeposit value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> poolDeposit() const;

    /**
     * @brief Proposes or removes the poolDeposit value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPoolDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxEpoch value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxEpoch() const;

    /**
     * @brief Proposes or removes the maxEpoch value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxEpoch(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed nOpt value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> nOpt() const;

    /**
     * @brief Proposes or removes the nOpt value.
     * @param value The proposed value, or empty to remove it.
     */
    void setNOpt(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed poolPledgeInfluence value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> poolPledgeInfluence() const;

    /**
     * @brief Proposes or removes the poolPledgeInfluence value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPoolPledgeInfluence(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed expansionRate value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> expansionRate() const;

    /**
     * @brief Proposes or removes the expansionRate value.
     * @param value The proposed value, or empty to remove it.
     */
    void setExpansionRate(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed treasuryGrowthRate value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> treasuryGrowthRate() const;

    /**
     * @brief Proposes or removes the treasuryGrowthRate value.
     * @param value The proposed value, or empty to remove it.
     */
    void setTreasuryGrowthRate(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed d value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> d() const;

    /**
     * @brief Proposes or removes the d value.
     * @param value The proposed value, or empty to remove it.
     */
    void setD(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed extraEntropy value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::vector<std::uint8_t>> extraEntropy() const;

    /**
     * @brief Proposes or removes the extraEntropy value.
     * @param value The proposed value, or empty to remove it.
     */
    void setExtraEntropy(const std::optional<std::vector<std::uint8_t>>& value);

    /**
     * @brief Gets the proposed protocolVersion value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<ProtocolVersion> protocolVersion() const;

    /**
     * @brief Proposes or removes the protocolVersion value.
     * @param value The proposed value, or empty to remove it.
     */
    void setProtocolVersion(const std::optional<ProtocolVersion>& value);

    /**
     * @brief Gets the proposed minPoolCost value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> minPoolCost() const;

    /**
     * @brief Proposes or removes the minPoolCost value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMinPoolCost(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed adaPerUtxoByte value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> adaPerUtxoByte() const;

    /**
     * @brief Proposes or removes the adaPerUtxoByte value.
     * @param value The proposed value, or empty to remove it.
     */
    void setAdaPerUtxoByte(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed costModels value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<CostModels> costModels() const;

    /**
     * @brief Proposes or removes the costModels value.
     * @param value The proposed value, or empty to remove it.
     */
    void setCostModels(const std::optional<CostModels>& value);

    /**
     * @brief Gets the proposed executionCosts value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<ExUnitPrices> executionCosts() const;

    /**
     * @brief Proposes or removes the executionCosts value.
     * @param value The proposed value, or empty to remove it.
     */
    void setExecutionCosts(const std::optional<ExUnitPrices>& value);

    /**
     * @brief Gets the proposed maxTxExUnits value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<ExUnits> maxTxExUnits() const;

    /**
     * @brief Proposes or removes the maxTxExUnits value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxTxExUnits(const std::optional<ExUnits>& value);

    /**
     * @brief Gets the proposed maxBlockExUnits value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<ExUnits> maxBlockExUnits() const;

    /**
     * @brief Proposes or removes the maxBlockExUnits value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxBlockExUnits(const std::optional<ExUnits>& value);

    /**
     * @brief Gets the proposed maxValueSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxValueSize() const;

    /**
     * @brief Proposes or removes the maxValueSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxValueSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed collateralPercentage value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> collateralPercentage() const;

    /**
     * @brief Proposes or removes the collateralPercentage value.
     * @param value The proposed value, or empty to remove it.
     */
    void setCollateralPercentage(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxCollateralInputs value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxCollateralInputs() const;

    /**
     * @brief Proposes or removes the maxCollateralInputs value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxCollateralInputs(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed poolVotingThresholds value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<PoolVotingThresholds> poolVotingThresholds() const;

    /**
     * @brief Proposes or removes the poolVotingThresholds value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPoolVotingThresholds(const std::optional<PoolVotingThresholds>& value);

    /**
     * @brief Gets the proposed drepVotingThresholds value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<DRepVotingThresholds> drepVotingThresholds() const;

    /**
     * @brief Proposes or removes the drepVotingThresholds value.
     * @param value The proposed value, or empty to remove it.
     */
    void setDrepVotingThresholds(const std::optional<DRepVotingThresholds>& value);

    /**
     * @brief Gets the proposed minCommitteeSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> minCommitteeSize() const;

    /**
     * @brief Proposes or removes the minCommitteeSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMinCommitteeSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed committeeTermLimit value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> committeeTermLimit() const;

    /**
     * @brief Proposes or removes the committeeTermLimit value.
     * @param value The proposed value, or empty to remove it.
     */
    void setCommitteeTermLimit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed governanceActionValidityPeriod value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> governanceActionValidityPeriod() const;

    /**
     * @brief Proposes or removes the governanceActionValidityPeriod value.
     * @param value The proposed value, or empty to remove it.
     */
    void setGovernanceActionValidityPeriod(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed governanceActionDeposit value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> governanceActionDeposit() const;

    /**
     * @brief Proposes or removes the governanceActionDeposit value.
     * @param value The proposed value, or empty to remove it.
     */
    void setGovernanceActionDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed drepDeposit value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> drepDeposit() const;

    /**
     * @brief Proposes or removes the drepDeposit value.
     * @param value The proposed value, or empty to remove it.
     */
    void setDrepDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed drepInactivityPeriod value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> drepInactivityPeriod() const;

    /**
     * @brief Proposes or removes the drepInactivityPeriod value.
     * @param value The proposed value, or empty to remove it.
     */
    void setDrepInactivityPeriod(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed refScriptCostPerByte value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> refScriptCostPerByte() const;

    /**
     * @brief Proposes or removes the refScriptCostPerByte value.
     * @param value The proposed value, or empty to remove it.
     */
    void setRefScriptCostPerByte(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed maxRefScriptSizePerBlock value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxRefScriptSizePerBlock() const;

    /**
     * @brief Proposes or removes the maxRefScriptSizePerBlock value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxRefScriptSizePerBlock(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxRefScriptSizePerTx value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxRefScriptSizePerTx() const;

    /**
     * @brief Proposes or removes the maxRefScriptSizePerTx value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxRefScriptSizePerTx(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed refScriptCostStride value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> refScriptCostStride() const;

    /**
     * @brief Proposes or removes the refScriptCostStride value.
     * @param value The proposed value, or empty to remove it.
     */
    void setRefScriptCostStride(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed refScriptCostMultiplier value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> refScriptCostMultiplier() const;

    /**
     * @brief Proposes or removes the refScriptCostMultiplier value.
     * @param value The proposed value, or empty to remove it.
     */
    void setRefScriptCostMultiplier(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed maxPledgeLeverage value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> maxPledgeLeverage() const;

    /**
     * @brief Proposes or removes the maxPledgeLeverage value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxPledgeLeverage(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed minPoolMargin value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> minPoolMargin() const;

    /**
     * @brief Proposes or removes the minPoolMargin value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMinPoolMargin(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed leiosAnnouncementPeriodLength value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> leiosAnnouncementPeriodLength() const;

    /**
     * @brief Proposes or removes the leiosAnnouncementPeriodLength value.
     * @param value The proposed value, or empty to remove it.
     */
    void setLeiosAnnouncementPeriodLength(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed leiosVotePeriodLength value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> leiosVotePeriodLength() const;

    /**
     * @brief Proposes or removes the leiosVotePeriodLength value.
     * @param value The proposed value, or empty to remove it.
     */
    void setLeiosVotePeriodLength(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed leiosDiffusionPeriodLength value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> leiosDiffusionPeriodLength() const;

    /**
     * @brief Proposes or removes the leiosDiffusionPeriodLength value.
     * @param value The proposed value, or empty to remove it.
     */
    void setLeiosDiffusionPeriodLength(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed leiosCommitteeSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> leiosCommitteeSize() const;

    /**
     * @brief Proposes or removes the leiosCommitteeSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setLeiosCommitteeSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed leiosQuorumStakeThreshold value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> leiosQuorumStakeThreshold() const;

    /**
     * @brief Proposes or removes the leiosQuorumStakeThreshold value.
     * @param value The proposed value, or empty to remove it.
     */
    void setLeiosQuorumStakeThreshold(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed maxEndorserBlockReferencesSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxEndorserBlockReferencesSize() const;

    /**
     * @brief Proposes or removes the maxEndorserBlockReferencesSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxEndorserBlockReferencesSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxEndorserBlockTxsSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxEndorserBlockTxsSize() const;

    /**
     * @brief Proposes or removes the maxEndorserBlockTxsSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxEndorserBlockTxsSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed maxEndorserBlockExecutionUnits value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<ExUnits> maxEndorserBlockExecutionUnits() const;

    /**
     * @brief Proposes or removes the maxEndorserBlockExecutionUnits value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxEndorserBlockExecutionUnits(const std::optional<ExUnits>& value);

    /**
     * @brief Gets the proposed maxRefScriptSizePerEndorserBlock value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> maxRefScriptSizePerEndorserBlock() const;

    /**
     * @brief Proposes or removes the maxRefScriptSizePerEndorserBlock value.
     * @param value The proposed value, or empty to remove it.
     */
    void setMaxRefScriptSizePerEndorserBlock(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed perasMinCandidateBlockAge value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> perasMinCandidateBlockAge() const;

    /**
     * @brief Proposes or removes the perasMinCandidateBlockAge value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPerasMinCandidateBlockAge(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed perasHealingFactor value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> perasHealingFactor() const;

    /**
     * @brief Proposes or removes the perasHealingFactor value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPerasHealingFactor(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed perasCertBoost value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> perasCertBoost() const;

    /**
     * @brief Proposes or removes the perasCertBoost value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPerasCertBoost(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed perasTargetCommitteeSize value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> perasTargetCommitteeSize() const;

    /**
     * @brief Proposes or removes the perasTargetCommitteeSize value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPerasTargetCommitteeSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed perasBootstrapRound value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> perasBootstrapRound() const;

    /**
     * @brief Proposes or removes the perasBootstrapRound value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPerasBootstrapRound(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed perasQuorumThresholdSafetyMargin value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<UnitInterval> perasQuorumThresholdSafetyMargin() const;

    /**
     * @brief Proposes or removes the perasQuorumThresholdSafetyMargin value.
     * @param value The proposed value, or empty to remove it.
     */
    void setPerasQuorumThresholdSafetyMargin(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the proposed refInputsCostPerMultiAssetPolicy value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> refInputsCostPerMultiAssetPolicy() const;

    /**
     * @brief Proposes or removes the refInputsCostPerMultiAssetPolicy value.
     * @param value The proposed value, or empty to remove it.
     */
    void setRefInputsCostPerMultiAssetPolicy(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the proposed refInputsCostPerDatumByte value.
     * @return The value, or empty when the parameter is not proposed.
     */
    std::optional<std::uint64_t> refInputsCostPerDatumByte() const;

    /**
     * @brief Proposes or removes the refInputsCostPerDatumByte value.
     * @param value The proposed value, or empty to remove it.
     */
    void setRefInputsCostPerDatumByte(const std::optional<std::uint64_t>& value);

    /**
     * @brief Checks whether max pledge leverage is proposed.
     * @return True when present, including ledger null.
     */
    bool hasMaxPledgeLeverage() const;

    /**
     * @brief Checks whether max pledge leverage is unbounded.
     * @return True for explicit ledger null.
     */
    bool isMaxPledgeLeverageUnbounded() const;

    /** @brief Proposes unbounded pledge leverage. */
    void setMaxPledgeLeverageUnbounded();

    /** @brief Removes the pledge leverage proposal. */
    void clearMaxPledgeLeverage();

    /**
     * @brief Checks whether the Peras bootstrap round is proposed.
     * @return True when present, including ledger null.
     */
    bool hasPerasBootstrapRound() const;

    /**
     * @brief Checks whether the Peras bootstrap round is null.
     * @return True for explicit ledger null.
     */
    bool isPerasBootstrapRoundNull() const;

    /** @brief Proposes a null Peras bootstrap round. */
    void setPerasBootstrapRoundNull();

    /** @brief Removes the Peras bootstrap round proposal. */
    void clearPerasBootstrapRound();

    /**
     * @brief Encodes the update as CBOR.
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the update as Plutus data.
     * @return Independent Plutus data.
     */
    PlutusData toPlutusData() const;

    /**
     * @brief Formats the update as CIP-116 JSON.
     * @return Compact JSON.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges updates.
     * @param other The update to exchange.
     */
    void swap(ProtocolParamUpdate& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source.
     * @return This update.
     */
    ProtocolParamUpdate& operator=(const ProtocolParamUpdate& other);

    /**
     * @brief Assigns a moved update.
     * @param other The source.
     * @return This update.
     */
    ProtocolParamUpdate& operator=(ProtocolParamUpdate&& other) noexcept;

    /**
     * @brief Compares updates.
     * @param other The other update.
     * @return True when equal.
     */
    bool operator==(const ProtocolParamUpdate& other) const;

    /**
     * @brief Compares updates.
     * @param other The other update.
     * @return True when different.
     */
    bool operator!=(const ProtocolParamUpdate& other) const;

    /** @brief Allows the proposed-update map to access native values. */
    friend class ProposedParamUpdates;

  private:

    /**
     * @brief Adopts an owned update.
     * @param value The owned update.
     */
    explicit ProtocolParamUpdate(cardano_protocol_param_update_t* value) noexcept;

    /**
     * @brief Gets the stored update.
     * @return The borrowed update.
     */
    cardano_protocol_param_update_t* checked() const;

  private:

    /** @brief The owned update. */
    cardano_protocol_param_update_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges updates.
 * @param lhs The first update.
 * @param rhs The second update.
 */
void swap(ProtocolParamUpdate& lhs, ProtocolParamUpdate& rhs) noexcept;
} // namespace Cometa
