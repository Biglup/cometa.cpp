/**
 * @file ProtocolParameters.h
 *
 * @brief Current protocol parameters.
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
#include <Cometa/ProtocolParams/CostModels.h>
#include <Cometa/ProtocolParams/DRepVotingThresholds.h>
#include <Cometa/ProtocolParams/ExUnitPrices.h>
#include <Cometa/ProtocolParams/PoolVotingThresholds.h>

#include <cstdint>
#include <optional>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_protocol_parameters_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
class ProviderAdapter;

/** @brief Current network protocol parameters. */
class ProtocolParameters
{
  public:

    /** @brief Creates parameters with default values. */
    ProtocolParameters();

    /**
     * @brief Copies parameters.
     * @param other The parameters to copy.
     */
    ProtocolParameters(const ProtocolParameters& other);

    /**
     * @brief Moves parameters.
     * @param other The parameters to move.
     */
    ProtocolParameters(ProtocolParameters&& other) noexcept;

    /** @brief Destroys the parameters. */
    ~ProtocolParameters();

  public:

    /**
     * @brief Gets min fee a.
     * @return The current value.
     */
    std::uint64_t minFeeA() const;

    /**
     * @brief Sets min fee a.
     * @param value The new value.
     */
    void setMinFeeA(std::uint64_t value);

    /**
     * @brief Gets min fee b.
     * @return The current value.
     */
    std::uint64_t minFeeB() const;

    /**
     * @brief Sets min fee b.
     * @param value The new value.
     */
    void setMinFeeB(std::uint64_t value);

    /**
     * @brief Gets max block body size.
     * @return The current value.
     */
    std::uint64_t maxBlockBodySize() const;

    /**
     * @brief Sets max block body size.
     * @param value The new value.
     */
    void setMaxBlockBodySize(std::uint64_t value);

    /**
     * @brief Gets max tx size.
     * @return The current value.
     */
    std::uint64_t maxTxSize() const;

    /**
     * @brief Sets max tx size.
     * @param value The new value.
     */
    void setMaxTxSize(std::uint64_t value);

    /**
     * @brief Gets max block header size.
     * @return The current value.
     */
    std::uint64_t maxBlockHeaderSize() const;

    /**
     * @brief Sets max block header size.
     * @param value The new value.
     */
    void setMaxBlockHeaderSize(std::uint64_t value);

    /**
     * @brief Gets key deposit.
     * @return The current value.
     */
    std::uint64_t keyDeposit() const;

    /**
     * @brief Sets key deposit.
     * @param value The new value.
     */
    void setKeyDeposit(std::uint64_t value);

    /**
     * @brief Gets pool deposit.
     * @return The current value.
     */
    std::uint64_t poolDeposit() const;

    /**
     * @brief Sets pool deposit.
     * @param value The new value.
     */
    void setPoolDeposit(std::uint64_t value);

    /**
     * @brief Gets max epoch.
     * @return The current value.
     */
    std::uint64_t maxEpoch() const;

    /**
     * @brief Sets max epoch.
     * @param value The new value.
     */
    void setMaxEpoch(std::uint64_t value);

    /**
     * @brief Gets n opt.
     * @return The current value.
     */
    std::uint64_t nOpt() const;

    /**
     * @brief Sets n opt.
     * @param value The new value.
     */
    void setNOpt(std::uint64_t value);

    /**
     * @brief Gets pool pledge influence.
     * @return The current value.
     */
    UnitInterval poolPledgeInfluence() const;

    /**
     * @brief Sets pool pledge influence.
     * @param value The new value.
     */
    void setPoolPledgeInfluence(const UnitInterval& value);

    /**
     * @brief Gets expansion rate.
     * @return The current value.
     */
    UnitInterval expansionRate() const;

    /**
     * @brief Sets expansion rate.
     * @param value The new value.
     */
    void setExpansionRate(const UnitInterval& value);

    /**
     * @brief Gets treasury growth rate.
     * @return The current value.
     */
    UnitInterval treasuryGrowthRate() const;

    /**
     * @brief Sets treasury growth rate.
     * @param value The new value.
     */
    void setTreasuryGrowthRate(const UnitInterval& value);

    /**
     * @brief Gets d.
     * @return The current value.
     */
    UnitInterval d() const;

    /**
     * @brief Sets d.
     * @param value The new value.
     */
    void setD(const UnitInterval& value);

    /**
     * @brief Gets extra entropy.
     * @return The current value.
     */
    std::vector<std::uint8_t> extraEntropy() const;

    /**
     * @brief Sets extra entropy.
     * @param value The new value.
     */
    void setExtraEntropy(const std::vector<std::uint8_t>& value);

    /**
     * @brief Gets protocol version.
     * @return The current value.
     */
    ProtocolVersion protocolVersion() const;

    /**
     * @brief Sets protocol version.
     * @param value The new value.
     */
    void setProtocolVersion(const ProtocolVersion& value);

    /**
     * @brief Gets min pool cost.
     * @return The current value.
     */
    std::uint64_t minPoolCost() const;

    /**
     * @brief Sets min pool cost.
     * @param value The new value.
     */
    void setMinPoolCost(std::uint64_t value);

    /**
     * @brief Gets ada per utxo byte.
     * @return The current value.
     */
    std::uint64_t adaPerUtxoByte() const;

    /**
     * @brief Sets ada per utxo byte.
     * @param value The new value.
     */
    void setAdaPerUtxoByte(std::uint64_t value);

    /**
     * @brief Gets cost models.
     * @return The current value.
     */
    CostModels costModels() const;

    /**
     * @brief Sets cost models.
     * @param value The new value.
     */
    void setCostModels(const CostModels& value);

    /**
     * @brief Gets execution costs.
     * @return The current value.
     */
    ExUnitPrices executionCosts() const;

    /**
     * @brief Sets execution costs.
     * @param value The new value.
     */
    void setExecutionCosts(const ExUnitPrices& value);

    /**
     * @brief Gets max tx ex units.
     * @return The current value.
     */
    ExUnits maxTxExUnits() const;

    /**
     * @brief Sets max tx ex units.
     * @param value The new value.
     */
    void setMaxTxExUnits(const ExUnits& value);

    /**
     * @brief Gets max block ex units.
     * @return The current value.
     */
    ExUnits maxBlockExUnits() const;

    /**
     * @brief Sets max block ex units.
     * @param value The new value.
     */
    void setMaxBlockExUnits(const ExUnits& value);

    /**
     * @brief Gets max value size.
     * @return The current value.
     */
    std::uint64_t maxValueSize() const;

    /**
     * @brief Sets max value size.
     * @param value The new value.
     */
    void setMaxValueSize(std::uint64_t value);

    /**
     * @brief Gets collateral percentage.
     * @return The current value.
     */
    std::uint64_t collateralPercentage() const;

    /**
     * @brief Sets collateral percentage.
     * @param value The new value.
     */
    void setCollateralPercentage(std::uint64_t value);

    /**
     * @brief Gets max collateral inputs.
     * @return The current value.
     */
    std::uint64_t maxCollateralInputs() const;

    /**
     * @brief Sets max collateral inputs.
     * @param value The new value.
     */
    void setMaxCollateralInputs(std::uint64_t value);

    /**
     * @brief Gets pool voting thresholds.
     * @return The current value.
     */
    PoolVotingThresholds poolVotingThresholds() const;

    /**
     * @brief Sets pool voting thresholds.
     * @param value The new value.
     */
    void setPoolVotingThresholds(const PoolVotingThresholds& value);

    /**
     * @brief Gets drep voting thresholds.
     * @return The current value.
     */
    DRepVotingThresholds drepVotingThresholds() const;

    /**
     * @brief Sets drep voting thresholds.
     * @param value The new value.
     */
    void setDrepVotingThresholds(const DRepVotingThresholds& value);

    /**
     * @brief Gets min committee size.
     * @return The current value.
     */
    std::uint64_t minCommitteeSize() const;

    /**
     * @brief Sets min committee size.
     * @param value The new value.
     */
    void setMinCommitteeSize(std::uint64_t value);

    /**
     * @brief Gets committee term limit.
     * @return The current value.
     */
    std::uint64_t committeeTermLimit() const;

    /**
     * @brief Sets committee term limit.
     * @param value The new value.
     */
    void setCommitteeTermLimit(std::uint64_t value);

    /**
     * @brief Gets governance action validity period.
     * @return The current value.
     */
    std::uint64_t governanceActionValidityPeriod() const;

    /**
     * @brief Sets governance action validity period.
     * @param value The new value.
     */
    void setGovernanceActionValidityPeriod(std::uint64_t value);

    /**
     * @brief Gets governance action deposit.
     * @return The current value.
     */
    std::uint64_t governanceActionDeposit() const;

    /**
     * @brief Sets governance action deposit.
     * @param value The new value.
     */
    void setGovernanceActionDeposit(std::uint64_t value);

    /**
     * @brief Gets drep deposit.
     * @return The current value.
     */
    std::uint64_t drepDeposit() const;

    /**
     * @brief Sets drep deposit.
     * @param value The new value.
     */
    void setDrepDeposit(std::uint64_t value);

    /**
     * @brief Gets drep inactivity period.
     * @return The current value.
     */
    std::uint64_t drepInactivityPeriod() const;

    /**
     * @brief Sets drep inactivity period.
     * @param value The new value.
     */
    void setDrepInactivityPeriod(std::uint64_t value);

    /**
     * @brief Gets ref script cost per byte.
     * @return The current value.
     */
    UnitInterval refScriptCostPerByte() const;

    /**
     * @brief Sets ref script cost per byte.
     * @param value The new value.
     */
    void setRefScriptCostPerByte(const UnitInterval& value);

    /**
     * @brief Gets max ref script size per block.
     * @return The current value.
     */
    std::uint64_t maxRefScriptSizePerBlock() const;

    /**
     * @brief Sets max ref script size per block.
     * @param value The new value.
     */
    void setMaxRefScriptSizePerBlock(std::uint64_t value);

    /**
     * @brief Gets max ref script size per tx.
     * @return The current value.
     */
    std::uint64_t maxRefScriptSizePerTx() const;

    /**
     * @brief Sets max ref script size per tx.
     * @param value The new value.
     */
    void setMaxRefScriptSizePerTx(std::uint64_t value);

    /**
     * @brief Gets ref script cost stride.
     * @return The current value.
     */
    std::uint64_t refScriptCostStride() const;

    /**
     * @brief Sets ref script cost stride.
     * @param value The new value.
     */
    void setRefScriptCostStride(std::uint64_t value);

    /**
     * @brief Gets ref script cost multiplier.
     * @return The current value.
     */
    std::optional<UnitInterval> refScriptCostMultiplier() const;

    /**
     * @brief Sets ref script cost multiplier.
     * @param value The new value.
     */
    void setRefScriptCostMultiplier(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets max pledge leverage.
     * @return The current value.
     */
    std::optional<UnitInterval> maxPledgeLeverage() const;

    /**
     * @brief Sets max pledge leverage.
     * @param value The new value.
     */
    void setMaxPledgeLeverage(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets min pool margin.
     * @return The current value.
     */
    UnitInterval minPoolMargin() const;

    /**
     * @brief Sets min pool margin.
     * @param value The new value.
     */
    void setMinPoolMargin(const UnitInterval& value);

    /**
     * @brief Gets leios announcement period length.
     * @return The current value.
     */
    std::uint64_t leiosAnnouncementPeriodLength() const;

    /**
     * @brief Sets leios announcement period length.
     * @param value The new value.
     */
    void setLeiosAnnouncementPeriodLength(std::uint64_t value);

    /**
     * @brief Gets leios vote period length.
     * @return The current value.
     */
    std::uint64_t leiosVotePeriodLength() const;

    /**
     * @brief Sets leios vote period length.
     * @param value The new value.
     */
    void setLeiosVotePeriodLength(std::uint64_t value);

    /**
     * @brief Gets leios diffusion period length.
     * @return The current value.
     */
    std::uint64_t leiosDiffusionPeriodLength() const;

    /**
     * @brief Sets leios diffusion period length.
     * @param value The new value.
     */
    void setLeiosDiffusionPeriodLength(std::uint64_t value);

    /**
     * @brief Gets leios committee size.
     * @return The current value.
     */
    std::uint64_t leiosCommitteeSize() const;

    /**
     * @brief Sets leios committee size.
     * @param value The new value.
     */
    void setLeiosCommitteeSize(std::uint64_t value);

    /**
     * @brief Gets leios quorum stake threshold.
     * @return The current value.
     */
    UnitInterval leiosQuorumStakeThreshold() const;

    /**
     * @brief Sets leios quorum stake threshold.
     * @param value The new value.
     */
    void setLeiosQuorumStakeThreshold(const UnitInterval& value);

    /**
     * @brief Gets max endorser block references size.
     * @return The current value.
     */
    std::uint64_t maxEndorserBlockReferencesSize() const;

    /**
     * @brief Sets max endorser block references size.
     * @param value The new value.
     */
    void setMaxEndorserBlockReferencesSize(std::uint64_t value);

    /**
     * @brief Gets max endorser block txs size.
     * @return The current value.
     */
    std::uint64_t maxEndorserBlockTxsSize() const;

    /**
     * @brief Sets max endorser block txs size.
     * @param value The new value.
     */
    void setMaxEndorserBlockTxsSize(std::uint64_t value);

    /**
     * @brief Gets max endorser block execution units.
     * @return The current value.
     */
    ExUnits maxEndorserBlockExecutionUnits() const;

    /**
     * @brief Sets max endorser block execution units.
     * @param value The new value.
     */
    void setMaxEndorserBlockExecutionUnits(const ExUnits& value);

    /**
     * @brief Gets max ref script size per endorser block.
     * @return The current value.
     */
    std::uint64_t maxRefScriptSizePerEndorserBlock() const;

    /**
     * @brief Sets max ref script size per endorser block.
     * @param value The new value.
     */
    void setMaxRefScriptSizePerEndorserBlock(std::uint64_t value);

    /**
     * @brief Gets peras min candidate block age.
     * @return The current value.
     */
    std::uint64_t perasMinCandidateBlockAge() const;

    /**
     * @brief Sets peras min candidate block age.
     * @param value The new value.
     */
    void setPerasMinCandidateBlockAge(std::uint64_t value);

    /**
     * @brief Gets peras healing factor.
     * @return The current value.
     */
    std::optional<UnitInterval> perasHealingFactor() const;

    /**
     * @brief Sets peras healing factor.
     * @param value The new value.
     */
    void setPerasHealingFactor(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets peras cert boost.
     * @return The current value.
     */
    std::uint64_t perasCertBoost() const;

    /**
     * @brief Sets peras cert boost.
     * @param value The new value.
     */
    void setPerasCertBoost(std::uint64_t value);

    /**
     * @brief Gets peras target committee size.
     * @return The current value.
     */
    std::uint64_t perasTargetCommitteeSize() const;

    /**
     * @brief Sets peras target committee size.
     * @param value The new value.
     */
    void setPerasTargetCommitteeSize(std::uint64_t value);

    /**
     * @brief Gets peras bootstrap round.
     * @return The current value.
     */
    std::optional<std::uint64_t> perasBootstrapRound() const;

    /**
     * @brief Sets peras bootstrap round.
     * @param value The new value.
     */
    void setPerasBootstrapRound(std::optional<std::uint64_t> value);

    /**
     * @brief Gets peras quorum threshold safety margin.
     * @return The current value.
     */
    UnitInterval perasQuorumThresholdSafetyMargin() const;

    /**
     * @brief Sets peras quorum threshold safety margin.
     * @param value The new value.
     */
    void setPerasQuorumThresholdSafetyMargin(const UnitInterval& value);

    /**
     * @brief Gets ref inputs cost per multi asset policy.
     * @return The current value.
     */
    std::uint64_t refInputsCostPerMultiAssetPolicy() const;

    /**
     * @brief Sets ref inputs cost per multi asset policy.
     * @param value The new value.
     */
    void setRefInputsCostPerMultiAssetPolicy(std::uint64_t value);

    /**
     * @brief Gets ref inputs cost per datum byte.
     * @return The current value.
     */
    std::uint64_t refInputsCostPerDatumByte() const;

    /**
     * @brief Sets ref inputs cost per datum byte.
     * @param value The new value.
     */
    void setRefInputsCostPerDatumByte(std::uint64_t value);

    /**
     * @brief Exchanges parameter collections.
     * @param other The collection to exchange.
     */
    void swap(ProtocolParameters& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The parameters to copy.
     * @return This collection.
     */
    ProtocolParameters& operator=(const ProtocolParameters& other);

    /**
     * @brief Assigns moved parameters.
     * @param other The parameters to move.
     * @return This collection.
     */
    ProtocolParameters& operator=(ProtocolParameters&& other) noexcept;

    /**
     * @brief Compares parameter collections.
     * @param other The collection to compare.
     * @return True when every parameter is equal.
     */
    bool operator==(const ProtocolParameters& other) const;

    /**
     * @brief Compares parameter collections.
     * @param other The collection to compare.
     * @return True when any parameter differs.
     */
    bool operator!=(const ProtocolParameters& other) const;

  private:

    /** @brief Allows the provider bridge to copy parameters into native calls. */
    friend class ProviderAdapter;

    /**
     * @brief Gets the stored parameters.
     * @return The borrowed parameters.
     */
    cardano_protocol_parameters_t* checked() const;

  private:

    /** @brief The owned parameters. */
    cardano_protocol_parameters_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges parameter collections.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(ProtocolParameters& lhs, ProtocolParameters& rhs) noexcept;
} // namespace Cometa
