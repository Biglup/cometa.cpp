/**
 * @file ProtocolParamUpdate.h
 *
 * @brief A proposal to change selected protocol parameters.
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
 * @brief A proposal to change selected protocol parameters.
 *
 * An absent field normally leaves the parameter unchanged. Some fields distinguish absence from an
 * explicit null proposal; use their presence and null-state accessors when inspecting updates.
 *
 * @par Example
 * @code{.cpp}
 * #include <Cometa/Cometa.h>
 *
 * Cometa::ProtocolParamUpdate
 * feeProposal(std::uint64_t lovelacePerByte)
 * {
 *   Cometa::ProtocolParamUpdate update;
 *
 *   update.setMinFeeA(lovelacePerByte);
 *
 *   return update;
 * }
 * @endcode
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
     * @brief Gets the linear transaction-size fee coefficient proposed by this update.
     * @return Lovelace per serialized transaction byte. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> minFeeA() const;

    /**
     * @brief Sets the linear transaction-size fee coefficient in this proposal.
     * @param value Lovelace per serialized transaction byte. Use std::nullopt to omit the proposal.
     */
    void setMinFeeA(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the constant transaction fee term proposed by this update.
     * @return Lovelace added to the size-dependent fee. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> minFeeB() const;

    /**
     * @brief Sets the constant transaction fee term in this proposal.
     * @param value Lovelace added to the size-dependent fee. Use std::nullopt to omit the proposal.
     */
    void setMinFeeB(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum serialized block body size proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxBlockBodySize() const;

    /**
     * @brief Sets the maximum serialized block body size in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxBlockBodySize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum serialized transaction size proposed by this update.
     * @return The size limit in bytes, including witnesses. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxTxSize() const;

    /**
     * @brief Sets the maximum serialized transaction size in this proposal.
     * @param value The size limit in bytes, including witnesses. Use std::nullopt to omit the proposal.
     */
    void setMaxTxSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum serialized block header size proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxBlockHeaderSize() const;

    /**
     * @brief Sets the maximum serialized block header size in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxBlockHeaderSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the stake credential registration deposit proposed by this update.
     * @return The required deposit in lovelace. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> keyDeposit() const;

    /**
     * @brief Sets the stake credential registration deposit in this proposal.
     * @param value The required deposit in lovelace. Use std::nullopt to omit the proposal.
     */
    void setKeyDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the stake pool registration deposit proposed by this update.
     * @return The required deposit in lovelace. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> poolDeposit() const;

    /**
     * @brief Sets the stake pool registration deposit in this proposal.
     * @param value The required deposit in lovelace. Use std::nullopt to omit the proposal.
     */
    void setPoolDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum pool retirement horizon proposed by this update.
     * @return The maximum number of epochs ahead that a pool may schedule retirement. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxEpoch() const;

    /**
     * @brief Sets the maximum pool retirement horizon in this proposal.
     * @param value The maximum number of epochs ahead that a pool may schedule retirement. Use std::nullopt to omit the proposal.
     */
    void setMaxEpoch(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the desired number of stake pools proposed by this update.
     * @return The target pool count used by the reward calculation. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> nOpt() const;

    /**
     * @brief Sets the desired number of stake pools in this proposal.
     * @param value The target pool count used by the reward calculation. Use std::nullopt to omit the proposal.
     */
    void setNOpt(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the influence of pool pledge on rewards proposed by this update.
     * @return The dimensionless pledge influence factor. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> poolPledgeInfluence() const;

    /**
     * @brief Sets the influence of pool pledge on rewards in this proposal.
     * @param value The dimensionless pledge influence factor. Use std::nullopt to omit the proposal.
     */
    void setPoolPledgeInfluence(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the monetary expansion rate proposed by this update.
     * @return The fraction of reserves used for epoch monetary expansion. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> expansionRate() const;

    /**
     * @brief Sets the monetary expansion rate in this proposal.
     * @param value The fraction of reserves used for epoch monetary expansion. Use std::nullopt to omit the proposal.
     */
    void setExpansionRate(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the treasury share of monetary expansion proposed by this update.
     * @return The fraction allocated to the treasury. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> treasuryGrowthRate() const;

    /**
     * @brief Sets the treasury share of monetary expansion in this proposal.
     * @param value The fraction allocated to the treasury. Use std::nullopt to omit the proposal.
     */
    void setTreasuryGrowthRate(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the legacy decentralization parameter proposed by this update.
     * @return The fraction of blocks assigned to the legacy federated mechanism. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> d() const;

    /**
     * @brief Sets the legacy decentralization parameter in this proposal.
     * @param value The fraction of blocks assigned to the legacy federated mechanism. Use std::nullopt to omit the proposal.
     */
    void setD(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the legacy additional randomness contribution proposed by this update.
     * @return The optional nonce bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::vector<std::uint8_t>> extraEntropy() const;

    /**
     * @brief Sets the legacy additional randomness contribution in this proposal.
     * @param value The optional nonce bytes. Use std::nullopt to omit the proposal.
     */
    void setExtraEntropy(const std::optional<std::vector<std::uint8_t>>& value);

    /**
     * @brief Gets the ledger protocol version proposed by this update.
     * @return The major and minor version. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<ProtocolVersion> protocolVersion() const;

    /**
     * @brief Sets the ledger protocol version in this proposal.
     * @param value The major and minor version. Use std::nullopt to omit the proposal.
     */
    void setProtocolVersion(const std::optional<ProtocolVersion>& value);

    /**
     * @brief Gets the minimum stake pool fixed cost proposed by this update.
     * @return The minimum declared cost per epoch in lovelace. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> minPoolCost() const;

    /**
     * @brief Sets the minimum stake pool fixed cost in this proposal.
     * @param value The minimum declared cost per epoch in lovelace. Use std::nullopt to omit the proposal.
     */
    void setMinPoolCost(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the minimum-UTxO byte cost proposed by this update.
     * @return Lovelace per accounted byte of an unspent output. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> adaPerUtxoByte() const;

    /**
     * @brief Sets the minimum-UTxO byte cost in this proposal.
     * @param value Lovelace per accounted byte of an unspent output. Use std::nullopt to omit the proposal.
     */
    void setAdaPerUtxoByte(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Plutus execution cost models proposed by this update.
     * @return The models indexed by language version. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<CostModels> costModels() const;

    /**
     * @brief Sets the Plutus execution cost models in this proposal.
     * @param value The models indexed by language version. Use std::nullopt to omit the proposal.
     */
    void setCostModels(const std::optional<CostModels>& value);

    /**
     * @brief Gets the prices of Plutus execution units proposed by this update.
     * @return The rational prices in lovelace per memory unit and execution step. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<ExUnitPrices> executionCosts() const;

    /**
     * @brief Sets the prices of Plutus execution units in this proposal.
     * @param value The rational prices in lovelace per memory unit and execution step. Use std::nullopt to omit the proposal.
     */
    void setExecutionCosts(const std::optional<ExUnitPrices>& value);

    /**
     * @brief Gets the maximum execution budget per transaction proposed by this update.
     * @return The combined memory and step limits for one transaction. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<ExUnits> maxTxExUnits() const;

    /**
     * @brief Sets the maximum execution budget per transaction in this proposal.
     * @param value The combined memory and step limits for one transaction. Use std::nullopt to omit the proposal.
     */
    void setMaxTxExUnits(const std::optional<ExUnits>& value);

    /**
     * @brief Gets the maximum execution budget per block proposed by this update.
     * @return The combined memory and step limits for one block. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<ExUnits> maxBlockExUnits() const;

    /**
     * @brief Sets the maximum execution budget per block in this proposal.
     * @param value The combined memory and step limits for one block. Use std::nullopt to omit the proposal.
     */
    void setMaxBlockExUnits(const std::optional<ExUnits>& value);

    /**
     * @brief Gets the maximum serialized output value size proposed by this update.
     * @return The limit in bytes for the lovelace and token bundle. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxValueSize() const;

    /**
     * @brief Sets the maximum serialized output value size in this proposal.
     * @param value The limit in bytes for the lovelace and token bundle. Use std::nullopt to omit the proposal.
     */
    void setMaxValueSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the required collateral percentage of the transaction fee proposed by this update.
     * @return The percentage as an integer, not a fraction between zero and one. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> collateralPercentage() const;

    /**
     * @brief Sets the required collateral percentage of the transaction fee in this proposal.
     * @param value The percentage as an integer, not a fraction between zero and one. Use std::nullopt to omit the proposal.
     */
    void setCollateralPercentage(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum number of collateral inputs proposed by this update.
     * @return The input count limit. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxCollateralInputs() const;

    /**
     * @brief Sets the maximum number of collateral inputs in this proposal.
     * @param value The input count limit. Use std::nullopt to omit the proposal.
     */
    void setMaxCollateralInputs(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the pool approval thresholds for governance actions proposed by this update.
     * @return The action-specific stake pool voting thresholds. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<PoolVotingThresholds> poolVotingThresholds() const;

    /**
     * @brief Sets the pool approval thresholds for governance actions in this proposal.
     * @param value The action-specific stake pool voting thresholds. Use std::nullopt to omit the proposal.
     */
    void setPoolVotingThresholds(const std::optional<PoolVotingThresholds>& value);

    /**
     * @brief Gets the DRep approval thresholds for governance actions proposed by this update.
     * @return The action-specific DRep voting thresholds. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<DRepVotingThresholds> drepVotingThresholds() const;

    /**
     * @brief Sets the DRep approval thresholds for governance actions in this proposal.
     * @param value The action-specific DRep voting thresholds. Use std::nullopt to omit the proposal.
     */
    void setDrepVotingThresholds(const std::optional<DRepVotingThresholds>& value);

    /**
     * @brief Gets the minimum constitutional committee size proposed by this update.
     * @return The minimum number of committee members. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> minCommitteeSize() const;

    /**
     * @brief Sets the minimum constitutional committee size in this proposal.
     * @param value The minimum number of committee members. Use std::nullopt to omit the proposal.
     */
    void setMinCommitteeSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum constitutional committee term proposed by this update.
     * @return The term limit in epochs. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> committeeTermLimit() const;

    /**
     * @brief Sets the maximum constitutional committee term in this proposal.
     * @param value The term limit in epochs. Use std::nullopt to omit the proposal.
     */
    void setCommitteeTermLimit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the governance proposal lifetime proposed by this update.
     * @return The number of epochs a proposal remains eligible for ratification. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> governanceActionValidityPeriod() const;

    /**
     * @brief Sets the governance proposal lifetime in this proposal.
     * @param value The number of epochs a proposal remains eligible for ratification. Use std::nullopt to omit the proposal.
     */
    void setGovernanceActionValidityPeriod(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the governance proposal deposit proposed by this update.
     * @return The deposit in lovelace. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> governanceActionDeposit() const;

    /**
     * @brief Sets the governance proposal deposit in this proposal.
     * @param value The deposit in lovelace. Use std::nullopt to omit the proposal.
     */
    void setGovernanceActionDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the DRep registration deposit proposed by this update.
     * @return The deposit in lovelace. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> drepDeposit() const;

    /**
     * @brief Sets the DRep registration deposit in this proposal.
     * @param value The deposit in lovelace. Use std::nullopt to omit the proposal.
     */
    void setDrepDeposit(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the DRep inactivity interval proposed by this update.
     * @return The number of epochs used to determine DRep activity. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> drepInactivityPeriod() const;

    /**
     * @brief Sets the DRep inactivity interval in this proposal.
     * @param value The number of epochs used to determine DRep activity. Use std::nullopt to omit the proposal.
     */
    void setDrepInactivityPeriod(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the base reference-script fee rate proposed by this update.
     * @return The rational price in lovelace per reference-script byte. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> refScriptCostPerByte() const;

    /**
     * @brief Sets the base reference-script fee rate in this proposal.
     * @param value The rational price in lovelace per reference-script byte. Use std::nullopt to omit the proposal.
     */
    void setRefScriptCostPerByte(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the maximum total reference-script size per block proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxRefScriptSizePerBlock() const;

    /**
     * @brief Sets the maximum total reference-script size per block in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxRefScriptSizePerBlock(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum total reference-script size per transaction proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxRefScriptSizePerTx() const;

    /**
     * @brief Sets the maximum total reference-script size per transaction in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxRefScriptSizePerTx(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the reference-script fee tier width proposed by this update.
     * @return The number of bytes in each pricing tier. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> refScriptCostStride() const;

    /**
     * @brief Sets the reference-script fee tier width in this proposal.
     * @param value The number of bytes in each pricing tier. Use std::nullopt to omit the proposal.
     */
    void setRefScriptCostStride(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the reference-script fee increase between tiers proposed by this update.
     * @return The optional multiplier applied at each pricing tier. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> refScriptCostMultiplier() const;

    /**
     * @brief Sets the reference-script fee increase between tiers in this proposal.
     * @param value The optional multiplier applied at each pricing tier. Use std::nullopt to omit the proposal.
     */
    void setRefScriptCostMultiplier(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the pool pledge leverage limit proposed by this update.
     *
     * A missing optional value can mean either an omitted field or an explicit null proposal. Use the
     * corresponding presence and null-state accessors to distinguish them.
     *
     * @return The proposed limit on stake relative to pledge. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> maxPledgeLeverage() const;

    /**
     * @brief Sets the pool pledge leverage limit in this proposal.
     *
     * Passing std::nullopt omits this field from the update. Use the separate explicit-null setter to
     * propose a ledger null value.
     *
     * @param value The proposed limit on stake relative to pledge.
     */
    void setMaxPledgeLeverage(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the minimum stake pool reward margin proposed by this update.
     * @return The minimum fractional margin. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> minPoolMargin() const;

    /**
     * @brief Sets the minimum stake pool reward margin in this proposal.
     * @param value The minimum fractional margin. Use std::nullopt to omit the proposal.
     */
    void setMinPoolMargin(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the Leios endorser-block announcement window proposed by this update.
     * @return The duration in milliseconds. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> leiosAnnouncementPeriodLength() const;

    /**
     * @brief Sets the Leios endorser-block announcement window in this proposal.
     * @param value The duration in milliseconds. Use std::nullopt to omit the proposal.
     */
    void setLeiosAnnouncementPeriodLength(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Leios endorser-block voting window proposed by this update.
     * @return The duration in milliseconds. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> leiosVotePeriodLength() const;

    /**
     * @brief Sets the Leios endorser-block voting window in this proposal.
     * @param value The duration in milliseconds. Use std::nullopt to omit the proposal.
     */
    void setLeiosVotePeriodLength(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Leios endorser-block propagation window proposed by this update.
     * @return The duration in milliseconds. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> leiosDiffusionPeriodLength() const;

    /**
     * @brief Sets the Leios endorser-block propagation window in this proposal.
     * @param value The duration in milliseconds. Use std::nullopt to omit the proposal.
     */
    void setLeiosDiffusionPeriodLength(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the target Leios voting committee size proposed by this update.
     * @return The target number of committee members. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> leiosCommitteeSize() const;

    /**
     * @brief Sets the target Leios voting committee size in this proposal.
     * @param value The target number of committee members. Use std::nullopt to omit the proposal.
     */
    void setLeiosCommitteeSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Leios voting quorum threshold proposed by this update.
     * @return The stake fraction required for a quorum. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> leiosQuorumStakeThreshold() const;

    /**
     * @brief Sets the Leios voting quorum threshold in this proposal.
     * @param value The stake fraction required for a quorum. Use std::nullopt to omit the proposal.
     */
    void setLeiosQuorumStakeThreshold(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the maximum endorser-block reference payload size proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxEndorserBlockReferencesSize() const;

    /**
     * @brief Sets the maximum endorser-block reference payload size in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxEndorserBlockReferencesSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum endorser-block transaction payload size proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxEndorserBlockTxsSize() const;

    /**
     * @brief Sets the maximum endorser-block transaction payload size in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxEndorserBlockTxsSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the maximum endorser-block execution budget proposed by this update.
     * @return The combined memory and step limits. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<ExUnits> maxEndorserBlockExecutionUnits() const;

    /**
     * @brief Sets the maximum endorser-block execution budget in this proposal.
     * @param value The combined memory and step limits. Use std::nullopt to omit the proposal.
     */
    void setMaxEndorserBlockExecutionUnits(const std::optional<ExUnits>& value);

    /**
     * @brief Gets the maximum reference-script size per endorser block proposed by this update.
     * @return The size limit in bytes. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> maxRefScriptSizePerEndorserBlock() const;

    /**
     * @brief Sets the maximum reference-script size per endorser block in this proposal.
     * @param value The size limit in bytes. Use std::nullopt to omit the proposal.
     */
    void setMaxRefScriptSizePerEndorserBlock(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the minimum block age for Peras voting proposed by this update.
     * @return The minimum age in slots. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> perasMinCandidateBlockAge() const;

    /**
     * @brief Sets the minimum block age for Peras voting in this proposal.
     * @param value The minimum age in slots. Use std::nullopt to omit the proposal.
     */
    void setPerasMinCandidateBlockAge(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Peras cooldown duration coefficient proposed by this update.
     * @return The optional positive factor used after a round without quorum. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> perasHealingFactor() const;

    /**
     * @brief Sets the Peras cooldown duration coefficient in this proposal.
     * @param value The optional positive factor used after a round without quorum. Use std::nullopt to omit the proposal.
     */
    void setPerasHealingFactor(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the chain-selection weight of a Peras certificate proposed by this update.
     * @return The certificate's weight boost. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> perasCertBoost() const;

    /**
     * @brief Sets the chain-selection weight of a Peras certificate in this proposal.
     * @param value The certificate's weight boost. Use std::nullopt to omit the proposal.
     */
    void setPerasCertBoost(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the target Peras voting committee size proposed by this update.
     * @return The target number of committee members. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> perasTargetCommitteeSize() const;

    /**
     * @brief Sets the target Peras voting committee size in this proposal.
     * @param value The target number of committee members. Use std::nullopt to omit the proposal.
     */
    void setPerasTargetCommitteeSize(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Peras bootstrap round proposed by this update.
     *
     * A missing optional value can mean either an omitted field or an explicit null proposal. Use the
     * corresponding presence and null-state accessors to distinguish them.
     *
     * @return The optional round number. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> perasBootstrapRound() const;

    /**
     * @brief Sets the Peras bootstrap round in this proposal.
     *
     * Passing std::nullopt omits this field from the update. Use the separate explicit-null setter to
     * propose a ledger null value.
     *
     * @param value The optional round number.
     */
    void setPerasBootstrapRound(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the Peras quorum safety margin proposed by this update.
     * @return The rational safety margin for quorum selection. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<UnitInterval> perasQuorumThresholdSafetyMargin() const;

    /**
     * @brief Sets the Peras quorum safety margin in this proposal.
     * @param value The rational safety margin for quorum selection. Use std::nullopt to omit the proposal.
     */
    void setPerasQuorumThresholdSafetyMargin(const std::optional<UnitInterval>& value);

    /**
     * @brief Gets the reference-input fee parameter for asset policies proposed by this update.
     * @return Lovelace per multi-asset policy in reference inputs. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> refInputsCostPerMultiAssetPolicy() const;

    /**
     * @brief Sets the reference-input fee parameter for asset policies in this proposal.
     * @param value Lovelace per multi-asset policy in reference inputs. Use std::nullopt to omit the proposal.
     */
    void setRefInputsCostPerMultiAssetPolicy(const std::optional<std::uint64_t>& value);

    /**
     * @brief Gets the reference-input fee parameter for datum bytes proposed by this update.
     * @return Lovelace per datum byte in reference inputs. An absent proposal leaves the current parameter unchanged.
     */
    std::optional<std::uint64_t> refInputsCostPerDatumByte() const;

    /**
     * @brief Sets the reference-input fee parameter for datum bytes in this proposal.
     * @param value Lovelace per datum byte in reference inputs. Use std::nullopt to omit the proposal.
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

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static ProtocolParamUpdate adopt(cardano_protocol_param_update_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_protocol_param_update_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_protocol_param_update_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_protocol_param_update_t* release() noexcept;

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
