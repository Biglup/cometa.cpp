/**
 * @file ProtocolParameters.h
 *
 * @brief A snapshot of a network's ledger limits, fees, deposits, and governance settings.
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
#include <Cometa/Common/UtxoList.h>
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
class Transaction;
class SubTransaction;
class Value;
struct ImplicitCoin;
struct TransactionBalanceRequest;

/**
 * @brief A snapshot of a network's ledger limits, fees, deposits, and governance settings.
 *
 * Obtain current values from Provider::getParameters() before building transactions. Default
 * construction is not a substitute for a network snapshot. Field availability and applicability depend
 * on the protocol version.
 */
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
     * @brief Gets the linear transaction-size fee coefficient.
     * @return Lovelace per serialized transaction byte.
     */
    std::uint64_t minFeeA() const;

    /**
     * @brief Sets the linear transaction-size fee coefficient.
     * @param value Lovelace per serialized transaction byte.
     */
    void setMinFeeA(std::uint64_t value);

    /**
     * @brief Gets the constant transaction fee term.
     * @return Lovelace added to the size-dependent fee.
     */
    std::uint64_t minFeeB() const;

    /**
     * @brief Sets the constant transaction fee term.
     * @param value Lovelace added to the size-dependent fee.
     */
    void setMinFeeB(std::uint64_t value);

    /**
     * @brief Gets the maximum serialized block body size.
     * @return The size limit in bytes.
     */
    std::uint64_t maxBlockBodySize() const;

    /**
     * @brief Sets the maximum serialized block body size.
     * @param value The size limit in bytes.
     */
    void setMaxBlockBodySize(std::uint64_t value);

    /**
     * @brief Gets the maximum serialized transaction size.
     * @return The size limit in bytes, including witnesses.
     */
    std::uint64_t maxTxSize() const;

    /**
     * @brief Sets the maximum serialized transaction size.
     * @param value The size limit in bytes, including witnesses.
     */
    void setMaxTxSize(std::uint64_t value);

    /**
     * @brief Gets the maximum serialized block header size.
     * @return The size limit in bytes.
     */
    std::uint64_t maxBlockHeaderSize() const;

    /**
     * @brief Sets the maximum serialized block header size.
     * @param value The size limit in bytes.
     */
    void setMaxBlockHeaderSize(std::uint64_t value);

    /**
     * @brief Gets the stake credential registration deposit.
     * @return The required deposit in lovelace.
     */
    std::uint64_t keyDeposit() const;

    /**
     * @brief Sets the stake credential registration deposit.
     * @param value The required deposit in lovelace.
     */
    void setKeyDeposit(std::uint64_t value);

    /**
     * @brief Gets the stake pool registration deposit.
     * @return The required deposit in lovelace.
     */
    std::uint64_t poolDeposit() const;

    /**
     * @brief Sets the stake pool registration deposit.
     * @param value The required deposit in lovelace.
     */
    void setPoolDeposit(std::uint64_t value);

    /**
     * @brief Gets the maximum pool retirement horizon.
     * @return The maximum number of epochs ahead that a pool may schedule retirement.
     */
    std::uint64_t maxEpoch() const;

    /**
     * @brief Sets the maximum pool retirement horizon.
     * @param value The maximum number of epochs ahead that a pool may schedule retirement.
     */
    void setMaxEpoch(std::uint64_t value);

    /**
     * @brief Gets the desired number of stake pools.
     * @return The target pool count used by the reward calculation.
     */
    std::uint64_t nOpt() const;

    /**
     * @brief Sets the desired number of stake pools.
     * @param value The target pool count used by the reward calculation.
     */
    void setNOpt(std::uint64_t value);

    /**
     * @brief Gets the influence of pool pledge on rewards.
     * @return The dimensionless pledge influence factor.
     */
    UnitInterval poolPledgeInfluence() const;

    /**
     * @brief Sets the influence of pool pledge on rewards.
     * @param value The dimensionless pledge influence factor.
     */
    void setPoolPledgeInfluence(const UnitInterval& value);

    /**
     * @brief Gets the monetary expansion rate.
     * @return The fraction of reserves used for epoch monetary expansion.
     */
    UnitInterval expansionRate() const;

    /**
     * @brief Sets the monetary expansion rate.
     * @param value The fraction of reserves used for epoch monetary expansion.
     */
    void setExpansionRate(const UnitInterval& value);

    /**
     * @brief Gets the treasury share of monetary expansion.
     * @return The fraction allocated to the treasury.
     */
    UnitInterval treasuryGrowthRate() const;

    /**
     * @brief Sets the treasury share of monetary expansion.
     * @param value The fraction allocated to the treasury.
     */
    void setTreasuryGrowthRate(const UnitInterval& value);

    /**
     * @brief Gets the legacy decentralization parameter.
     * @return The fraction of blocks assigned to the legacy federated mechanism.
     */
    UnitInterval d() const;

    /**
     * @brief Sets the legacy decentralization parameter.
     * @param value The fraction of blocks assigned to the legacy federated mechanism.
     */
    void setD(const UnitInterval& value);

    /**
     * @brief Gets the legacy additional randomness contribution.
     * @return The optional nonce bytes.
     */
    std::vector<std::uint8_t> extraEntropy() const;

    /**
     * @brief Sets the legacy additional randomness contribution.
     * @param value The optional nonce bytes.
     */
    void setExtraEntropy(const std::vector<std::uint8_t>& value);

    /**
     * @brief Gets the ledger protocol version.
     * @return The major and minor version.
     */
    ProtocolVersion protocolVersion() const;

    /**
     * @brief Sets the ledger protocol version.
     * @param value The major and minor version.
     */
    void setProtocolVersion(const ProtocolVersion& value);

    /**
     * @brief Gets the minimum stake pool fixed cost.
     * @return The minimum declared cost per epoch in lovelace.
     */
    std::uint64_t minPoolCost() const;

    /**
     * @brief Sets the minimum stake pool fixed cost.
     * @param value The minimum declared cost per epoch in lovelace.
     */
    void setMinPoolCost(std::uint64_t value);

    /**
     * @brief Gets the minimum-UTxO byte cost.
     * @return Lovelace per accounted byte of an unspent output.
     */
    std::uint64_t adaPerUtxoByte() const;

    /**
     * @brief Sets the minimum-UTxO byte cost.
     * @param value Lovelace per accounted byte of an unspent output.
     */
    void setAdaPerUtxoByte(std::uint64_t value);

    /**
     * @brief Gets the Plutus execution cost models.
     * @return The models indexed by language version.
     */
    CostModels costModels() const;

    /**
     * @brief Sets the Plutus execution cost models.
     * @param value The models indexed by language version.
     */
    void setCostModels(const CostModels& value);

    /**
     * @brief Gets the prices of Plutus execution units.
     * @return The rational prices in lovelace per memory unit and execution step.
     */
    ExUnitPrices executionCosts() const;

    /**
     * @brief Sets the prices of Plutus execution units.
     * @param value The rational prices in lovelace per memory unit and execution step.
     */
    void setExecutionCosts(const ExUnitPrices& value);

    /**
     * @brief Gets the maximum execution budget per transaction.
     * @return The combined memory and step limits for one transaction.
     */
    ExUnits maxTxExUnits() const;

    /**
     * @brief Sets the maximum execution budget per transaction.
     * @param value The combined memory and step limits for one transaction.
     */
    void setMaxTxExUnits(const ExUnits& value);

    /**
     * @brief Gets the maximum execution budget per block.
     * @return The combined memory and step limits for one block.
     */
    ExUnits maxBlockExUnits() const;

    /**
     * @brief Sets the maximum execution budget per block.
     * @param value The combined memory and step limits for one block.
     */
    void setMaxBlockExUnits(const ExUnits& value);

    /**
     * @brief Gets the maximum serialized output value size.
     * @return The limit in bytes for the lovelace and token bundle.
     */
    std::uint64_t maxValueSize() const;

    /**
     * @brief Sets the maximum serialized output value size.
     * @param value The limit in bytes for the lovelace and token bundle.
     */
    void setMaxValueSize(std::uint64_t value);

    /**
     * @brief Gets the required collateral percentage of the transaction fee.
     * @return The percentage as an integer, not a fraction between zero and one.
     */
    std::uint64_t collateralPercentage() const;

    /**
     * @brief Sets the required collateral percentage of the transaction fee.
     * @param value The percentage as an integer, not a fraction between zero and one.
     */
    void setCollateralPercentage(std::uint64_t value);

    /**
     * @brief Gets the maximum number of collateral inputs.
     * @return The input count limit.
     */
    std::uint64_t maxCollateralInputs() const;

    /**
     * @brief Sets the maximum number of collateral inputs.
     * @param value The input count limit.
     */
    void setMaxCollateralInputs(std::uint64_t value);

    /**
     * @brief Gets the pool approval thresholds for governance actions.
     * @return The action-specific stake pool voting thresholds.
     */
    PoolVotingThresholds poolVotingThresholds() const;

    /**
     * @brief Sets the pool approval thresholds for governance actions.
     * @param value The action-specific stake pool voting thresholds.
     */
    void setPoolVotingThresholds(const PoolVotingThresholds& value);

    /**
     * @brief Gets the DRep approval thresholds for governance actions.
     * @return The action-specific DRep voting thresholds.
     */
    DRepVotingThresholds drepVotingThresholds() const;

    /**
     * @brief Sets the DRep approval thresholds for governance actions.
     * @param value The action-specific DRep voting thresholds.
     */
    void setDrepVotingThresholds(const DRepVotingThresholds& value);

    /**
     * @brief Gets the minimum constitutional committee size.
     * @return The minimum number of committee members.
     */
    std::uint64_t minCommitteeSize() const;

    /**
     * @brief Sets the minimum constitutional committee size.
     * @param value The minimum number of committee members.
     */
    void setMinCommitteeSize(std::uint64_t value);

    /**
     * @brief Gets the maximum constitutional committee term.
     * @return The term limit in epochs.
     */
    std::uint64_t committeeTermLimit() const;

    /**
     * @brief Sets the maximum constitutional committee term.
     * @param value The term limit in epochs.
     */
    void setCommitteeTermLimit(std::uint64_t value);

    /**
     * @brief Gets the governance proposal lifetime.
     * @return The number of epochs a proposal remains eligible for ratification.
     */
    std::uint64_t governanceActionValidityPeriod() const;

    /**
     * @brief Sets the governance proposal lifetime.
     * @param value The number of epochs a proposal remains eligible for ratification.
     */
    void setGovernanceActionValidityPeriod(std::uint64_t value);

    /**
     * @brief Gets the governance proposal deposit.
     * @return The deposit in lovelace.
     */
    std::uint64_t governanceActionDeposit() const;

    /**
     * @brief Sets the governance proposal deposit.
     * @param value The deposit in lovelace.
     */
    void setGovernanceActionDeposit(std::uint64_t value);

    /**
     * @brief Gets the DRep registration deposit.
     * @return The deposit in lovelace.
     */
    std::uint64_t drepDeposit() const;

    /**
     * @brief Sets the DRep registration deposit.
     * @param value The deposit in lovelace.
     */
    void setDrepDeposit(std::uint64_t value);

    /**
     * @brief Gets the DRep inactivity interval.
     * @return The number of epochs used to determine DRep activity.
     */
    std::uint64_t drepInactivityPeriod() const;

    /**
     * @brief Sets the DRep inactivity interval.
     * @param value The number of epochs used to determine DRep activity.
     */
    void setDrepInactivityPeriod(std::uint64_t value);

    /**
     * @brief Gets the base reference-script fee rate.
     * @return The rational price in lovelace per reference-script byte.
     */
    UnitInterval refScriptCostPerByte() const;

    /**
     * @brief Sets the base reference-script fee rate.
     * @param value The rational price in lovelace per reference-script byte.
     */
    void setRefScriptCostPerByte(const UnitInterval& value);

    /**
     * @brief Gets the maximum total reference-script size per block.
     * @return The size limit in bytes.
     */
    std::uint64_t maxRefScriptSizePerBlock() const;

    /**
     * @brief Sets the maximum total reference-script size per block.
     * @param value The size limit in bytes.
     */
    void setMaxRefScriptSizePerBlock(std::uint64_t value);

    /**
     * @brief Gets the maximum total reference-script size per transaction.
     * @return The size limit in bytes.
     */
    std::uint64_t maxRefScriptSizePerTx() const;

    /**
     * @brief Sets the maximum total reference-script size per transaction.
     * @param value The size limit in bytes.
     */
    void setMaxRefScriptSizePerTx(std::uint64_t value);

    /**
     * @brief Gets the reference-script fee tier width.
     * @return The number of bytes in each pricing tier.
     */
    std::uint64_t refScriptCostStride() const;

    /**
     * @brief Sets the reference-script fee tier width.
     * @param value The number of bytes in each pricing tier.
     */
    void setRefScriptCostStride(std::uint64_t value);

    /**
     * @brief Gets the reference-script fee increase between tiers.
     * @return The optional multiplier applied at each pricing tier.
     */
    std::optional<UnitInterval> refScriptCostMultiplier() const;

    /**
     * @brief Sets the reference-script fee increase between tiers.
     * @param value The optional multiplier applied at each pricing tier.
     */
    void setRefScriptCostMultiplier(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the pool pledge leverage limit.
     * @return The optional limit on stake relative to pledge; std::nullopt represents no bound.
     */
    std::optional<UnitInterval> maxPledgeLeverage() const;

    /**
     * @brief Sets the pool pledge leverage limit.
     * @param value The optional limit on stake relative to pledge; std::nullopt represents no bound.
     */
    void setMaxPledgeLeverage(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the minimum stake pool reward margin.
     * @return The minimum fractional margin.
     */
    UnitInterval minPoolMargin() const;

    /**
     * @brief Sets the minimum stake pool reward margin.
     * @param value The minimum fractional margin.
     */
    void setMinPoolMargin(const UnitInterval& value);

    /**
     * @brief Gets the Leios endorser-block announcement window.
     * @return The duration in milliseconds.
     */
    std::uint64_t leiosAnnouncementPeriodLength() const;

    /**
     * @brief Sets the Leios endorser-block announcement window.
     * @param value The duration in milliseconds.
     */
    void setLeiosAnnouncementPeriodLength(std::uint64_t value);

    /**
     * @brief Gets the Leios endorser-block voting window.
     * @return The duration in milliseconds.
     */
    std::uint64_t leiosVotePeriodLength() const;

    /**
     * @brief Sets the Leios endorser-block voting window.
     * @param value The duration in milliseconds.
     */
    void setLeiosVotePeriodLength(std::uint64_t value);

    /**
     * @brief Gets the Leios endorser-block propagation window.
     * @return The duration in milliseconds.
     */
    std::uint64_t leiosDiffusionPeriodLength() const;

    /**
     * @brief Sets the Leios endorser-block propagation window.
     * @param value The duration in milliseconds.
     */
    void setLeiosDiffusionPeriodLength(std::uint64_t value);

    /**
     * @brief Gets the target Leios voting committee size.
     * @return The target number of committee members.
     */
    std::uint64_t leiosCommitteeSize() const;

    /**
     * @brief Sets the target Leios voting committee size.
     * @param value The target number of committee members.
     */
    void setLeiosCommitteeSize(std::uint64_t value);

    /**
     * @brief Gets the Leios voting quorum threshold.
     * @return The stake fraction required for a quorum.
     */
    UnitInterval leiosQuorumStakeThreshold() const;

    /**
     * @brief Sets the Leios voting quorum threshold.
     * @param value The stake fraction required for a quorum.
     */
    void setLeiosQuorumStakeThreshold(const UnitInterval& value);

    /**
     * @brief Gets the maximum endorser-block reference payload size.
     * @return The size limit in bytes.
     */
    std::uint64_t maxEndorserBlockReferencesSize() const;

    /**
     * @brief Sets the maximum endorser-block reference payload size.
     * @param value The size limit in bytes.
     */
    void setMaxEndorserBlockReferencesSize(std::uint64_t value);

    /**
     * @brief Gets the maximum endorser-block transaction payload size.
     * @return The size limit in bytes.
     */
    std::uint64_t maxEndorserBlockTxsSize() const;

    /**
     * @brief Sets the maximum endorser-block transaction payload size.
     * @param value The size limit in bytes.
     */
    void setMaxEndorserBlockTxsSize(std::uint64_t value);

    /**
     * @brief Gets the maximum endorser-block execution budget.
     * @return The combined memory and step limits.
     */
    ExUnits maxEndorserBlockExecutionUnits() const;

    /**
     * @brief Sets the maximum endorser-block execution budget.
     * @param value The combined memory and step limits.
     */
    void setMaxEndorserBlockExecutionUnits(const ExUnits& value);

    /**
     * @brief Gets the maximum reference-script size per endorser block.
     * @return The size limit in bytes.
     */
    std::uint64_t maxRefScriptSizePerEndorserBlock() const;

    /**
     * @brief Sets the maximum reference-script size per endorser block.
     * @param value The size limit in bytes.
     */
    void setMaxRefScriptSizePerEndorserBlock(std::uint64_t value);

    /**
     * @brief Gets the minimum block age for Peras voting.
     * @return The minimum age in slots.
     */
    std::uint64_t perasMinCandidateBlockAge() const;

    /**
     * @brief Sets the minimum block age for Peras voting.
     * @param value The minimum age in slots.
     */
    void setPerasMinCandidateBlockAge(std::uint64_t value);

    /**
     * @brief Gets the Peras cooldown duration coefficient.
     * @return The optional positive factor used after a round without quorum.
     */
    std::optional<UnitInterval> perasHealingFactor() const;

    /**
     * @brief Sets the Peras cooldown duration coefficient.
     * @param value The optional positive factor used after a round without quorum.
     */
    void setPerasHealingFactor(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the chain-selection weight of a Peras certificate.
     * @return The certificate's weight boost.
     */
    std::uint64_t perasCertBoost() const;

    /**
     * @brief Sets the chain-selection weight of a Peras certificate.
     * @param value The certificate's weight boost.
     */
    void setPerasCertBoost(std::uint64_t value);

    /**
     * @brief Gets the target Peras voting committee size.
     * @return The target number of committee members.
     */
    std::uint64_t perasTargetCommitteeSize() const;

    /**
     * @brief Sets the target Peras voting committee size.
     * @param value The target number of committee members.
     */
    void setPerasTargetCommitteeSize(std::uint64_t value);

    /**
     * @brief Gets the Peras bootstrap round.
     * @return The optional round number.
     */
    std::optional<std::uint64_t> perasBootstrapRound() const;

    /**
     * @brief Sets the Peras bootstrap round.
     * @param value The optional round number.
     */
    void setPerasBootstrapRound(std::optional<std::uint64_t> value);

    /**
     * @brief Gets the Peras quorum safety margin.
     * @return The rational safety margin for quorum selection.
     */
    UnitInterval perasQuorumThresholdSafetyMargin() const;

    /**
     * @brief Sets the Peras quorum safety margin.
     * @param value The rational safety margin for quorum selection.
     */
    void setPerasQuorumThresholdSafetyMargin(const UnitInterval& value);

    /**
     * @brief Gets the reference-input fee parameter for asset policies.
     *
     * This parameter is exposed for inspection; the transaction builder does not currently include it in
     * fee calculation.
     *
     * @return Lovelace per multi-asset policy in reference inputs.
     */
    std::uint64_t refInputsCostPerMultiAssetPolicy() const;

    /**
     * @brief Sets the reference-input fee parameter for asset policies.
     * @param value Lovelace per multi-asset policy in reference inputs.
     */
    void setRefInputsCostPerMultiAssetPolicy(std::uint64_t value);

    /**
     * @brief Gets the reference-input fee parameter for datum bytes.
     *
     * This parameter is exposed for inspection; the transaction builder does not currently include it in
     * fee calculation.
     *
     * @return Lovelace per datum byte in reference inputs.
     */
    std::uint64_t refInputsCostPerDatumByte() const;

    /**
     * @brief Sets the reference-input fee parameter for datum bytes.
     * @param value Lovelace per datum byte in reference inputs.
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

    /** @brief Grants computeTransactionFee access to stored protocol parameters. */
    friend std::uint64_t computeTransactionFee(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

    /** @brief Grants computeScriptRefFee access to stored protocol parameters. */
    friend std::uint64_t computeScriptRefFee(const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

    /** @brief Grants computeImplicitCoin access to stored protocol parameters. */
    friend ImplicitCoin computeImplicitCoin(const Transaction& transaction, const ProtocolParameters& parameters);

    /** @brief Grants computeImplicitCoin access to stored protocol parameters. */
    friend ImplicitCoin computeImplicitCoin(const SubTransaction& transaction, const ProtocolParameters& parameters);

    /** @brief Grants computeTransactionImbalance access to stored protocol parameters. */
    friend Value computeTransactionImbalance(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

    /** @brief Grants computeSubTransactionImbalance access to stored protocol parameters. */
    friend Value computeSubTransactionImbalance(const SubTransaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

    /** @brief Grants computeTransactionBatchImbalance access to stored protocol parameters. */
    friend Value computeTransactionBatchImbalance(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

    /** @brief Grants isTransactionBalanced access to stored protocol parameters. */
    friend bool isTransactionBalanced(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

    /** @brief Grants balanceTransaction access to stored protocol parameters. */
    friend Transaction balanceTransaction(const TransactionBalanceRequest& request);

    /** @brief Grants copyProtocolParametersToNative access to stored protocol parameters. */
    friend cardano_protocol_parameters_t* copyProtocolParametersToNative(const ProtocolParameters& parameters);

    /** @brief Grants copyProtocolParametersFromNative access to stored protocol parameters. */
    friend ProtocolParameters copyProtocolParametersFromNative(const cardano_protocol_parameters_t* parameters);

    /** @brief Allows transaction builders to retain protocol parameters. */
    friend class TransactionBuilder;

    /** @brief Allows sub-transaction builders to retain protocol parameters. */
    friend class SubTransactionBuilder;

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
