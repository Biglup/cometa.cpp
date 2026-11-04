/**
 * @file ProtocolParameters.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/ProtocolParams/ProtocolParameters.h>

#include "../CardanoInterop.h"
#include "ProtocolParametersInterop.h"

#include <cardano/buffer.h>

#include <cardano/protocol_params/protocol_parameters.h>
#include <limits>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ProtocolParameters::ProtocolParameters()
: m_value(nullptr)
{
  cardano_protocol_parameters_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_protocol_parameters_new(&value);
  CardanoObject<cardano_protocol_parameters_t, cardano_protocol_parameters_unref> owner(value);

  checkCardanoError(status, "ProtocolParameters");
  m_value = owner.release();
}

ProtocolParameters::ProtocolParameters(const ProtocolParameters& other)
: m_value(nullptr)
{
  cardano_protocol_parameters_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_protocol_parameters_deep_clone(other.checked(), &value);
  CardanoObject<cardano_protocol_parameters_t, cardano_protocol_parameters_unref> owner(value);

  checkCardanoError(status, "ProtocolParameters copy");
  m_value = owner.release();
}

ProtocolParameters::ProtocolParameters(ProtocolParameters&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ProtocolParameters::~ProtocolParameters()
{
  cardano_protocol_parameters_unref(&m_value);
}

std::uint64_t
ProtocolParameters::minFeeA() const
{
  return cardano_protocol_parameters_get_min_fee_a(checked());
}

void
ProtocolParameters::setMinFeeA(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_min_fee_a(checked(), value), "ProtocolParameters setMinFeeA");
}

std::uint64_t
ProtocolParameters::minFeeB() const
{
  return cardano_protocol_parameters_get_min_fee_b(checked());
}

void
ProtocolParameters::setMinFeeB(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_min_fee_b(checked(), value), "ProtocolParameters setMinFeeB");
}

std::uint64_t
ProtocolParameters::maxBlockBodySize() const
{
  return cardano_protocol_parameters_get_max_block_body_size(checked());
}

void
ProtocolParameters::setMaxBlockBodySize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_block_body_size(checked(), value), "ProtocolParameters setMaxBlockBodySize");
}

std::uint64_t
ProtocolParameters::maxTxSize() const
{
  return cardano_protocol_parameters_get_max_tx_size(checked());
}

void
ProtocolParameters::setMaxTxSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_tx_size(checked(), value), "ProtocolParameters setMaxTxSize");
}

std::uint64_t
ProtocolParameters::maxBlockHeaderSize() const
{
  return cardano_protocol_parameters_get_max_block_header_size(checked());
}

void
ProtocolParameters::setMaxBlockHeaderSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_block_header_size(checked(), value), "ProtocolParameters setMaxBlockHeaderSize");
}

std::uint64_t
ProtocolParameters::keyDeposit() const
{
  return cardano_protocol_parameters_get_key_deposit(checked());
}

void
ProtocolParameters::setKeyDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_key_deposit(checked(), value), "ProtocolParameters setKeyDeposit");
}

std::uint64_t
ProtocolParameters::poolDeposit() const
{
  return cardano_protocol_parameters_get_pool_deposit(checked());
}

void
ProtocolParameters::setPoolDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_pool_deposit(checked(), value), "ProtocolParameters setPoolDeposit");
}

std::uint64_t
ProtocolParameters::maxEpoch() const
{
  return cardano_protocol_parameters_get_max_epoch(checked());
}

void
ProtocolParameters::setMaxEpoch(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_epoch(checked(), value), "ProtocolParameters setMaxEpoch");
}

std::uint64_t
ProtocolParameters::nOpt() const
{
  return cardano_protocol_parameters_get_n_opt(checked());
}

void
ProtocolParameters::setNOpt(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_n_opt(checked(), value), "ProtocolParameters setNOpt");
}

UnitInterval
ProtocolParameters::poolPledgeInfluence() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_pool_pledge_influence(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters poolPledgeInfluence is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setPoolPledgeInfluence(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_pool_pledge_influence(checked(), data), "ProtocolParameters setPoolPledgeInfluence");
}

UnitInterval
ProtocolParameters::expansionRate() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_expansion_rate(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters expansionRate is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setExpansionRate(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_expansion_rate(checked(), data), "ProtocolParameters setExpansionRate");
}

UnitInterval
ProtocolParameters::treasuryGrowthRate() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_treasury_growth_rate(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters treasuryGrowthRate is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setTreasuryGrowthRate(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_treasury_growth_rate(checked(), data), "ProtocolParameters setTreasuryGrowthRate");
}

UnitInterval
ProtocolParameters::d() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_d(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters d is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setD(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_d(checked(), data), "ProtocolParameters setD");
}

std::vector<std::uint8_t>
ProtocolParameters::extraEntropy() const
{
  cardano_buffer_t*                                     value = cardano_protocol_parameters_get_extra_entropy(checked());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters extraEntropy is unavailable");
  }

  const std::size_t size = cardano_buffer_get_size(value);
  if (size == 0)
  {
    return {};
  }

  const std::uint8_t* data = cardano_buffer_get_data(value);
  return std::vector<std::uint8_t>(data, data + size);
}

void
ProtocolParameters::setExtraEntropy(const std::vector<std::uint8_t>& value)
{
  cardano_buffer_t*                                     buffer = cardano_buffer_new(value.size());
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(buffer);
  if (!buffer)
  {
    throw std::bad_alloc();
  }
  if (!value.empty())
  {
    checkCardanoError(cardano_buffer_write(buffer, value.data(), value.size()), "ProtocolParameters setExtraEntropy bytes");
  }
  checkCardanoError(cardano_protocol_parameters_set_extra_entropy(checked(), buffer), "ProtocolParameters setExtraEntropy");
}

ProtocolVersion
ProtocolParameters::protocolVersion() const
{
  cardano_protocol_version_t*                                               value = cardano_protocol_parameters_get_protocol_version(checked());
  CardanoObject<cardano_protocol_version_t, cardano_protocol_version_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters protocolVersion is unavailable");
  }

  const ProtocolVersion found(owner.release());
  return ProtocolVersion(found);
}

void
ProtocolParameters::setProtocolVersion(const ProtocolVersion& value)
{
  const ProtocolVersion       copy(value);
  cardano_protocol_version_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_protocol_version(checked(), data), "ProtocolParameters setProtocolVersion");
}

std::uint64_t
ProtocolParameters::minPoolCost() const
{
  return cardano_protocol_parameters_get_min_pool_cost(checked());
}

void
ProtocolParameters::setMinPoolCost(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_min_pool_cost(checked(), value), "ProtocolParameters setMinPoolCost");
}

std::uint64_t
ProtocolParameters::adaPerUtxoByte() const
{
  return cardano_protocol_parameters_get_ada_per_utxo_byte(checked());
}

void
ProtocolParameters::setAdaPerUtxoByte(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_ada_per_utxo_byte(checked(), value), "ProtocolParameters setAdaPerUtxoByte");
}

CostModels
ProtocolParameters::costModels() const
{
  cardano_costmdls_t*                                       value = cardano_protocol_parameters_get_cost_models(checked());
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters costModels is unavailable");
  }

  const CostModels found(owner.release());
  return CostModels(found);
}

void
ProtocolParameters::setCostModels(const CostModels& value)
{
  const CostModels    copy(value);
  cardano_costmdls_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_cost_models(checked(), data), "ProtocolParameters setCostModels");
}

ExUnitPrices
ProtocolParameters::executionCosts() const
{
  cardano_ex_unit_prices_t*                                             value = cardano_protocol_parameters_get_execution_costs(checked());
  CardanoObject<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters executionCosts is unavailable");
  }

  const ExUnitPrices found(owner.release());
  return ExUnitPrices(found);
}

void
ProtocolParameters::setExecutionCosts(const ExUnitPrices& value)
{
  const ExUnitPrices        copy(value);
  cardano_ex_unit_prices_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_execution_costs(checked(), data), "ProtocolParameters setExecutionCosts");
}

ExUnits
ProtocolParameters::maxTxExUnits() const
{
  cardano_ex_units_t*                                       value = cardano_protocol_parameters_get_max_tx_ex_units(checked());
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters maxTxExUnits is unavailable");
  }

  const ExUnits found(owner.release());
  return ExUnits(found);
}

void
ProtocolParameters::setMaxTxExUnits(const ExUnits& value)
{
  const ExUnits       copy(value);
  cardano_ex_units_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_max_tx_ex_units(checked(), data), "ProtocolParameters setMaxTxExUnits");
}

ExUnits
ProtocolParameters::maxBlockExUnits() const
{
  cardano_ex_units_t*                                       value = cardano_protocol_parameters_get_max_block_ex_units(checked());
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters maxBlockExUnits is unavailable");
  }

  const ExUnits found(owner.release());
  return ExUnits(found);
}

void
ProtocolParameters::setMaxBlockExUnits(const ExUnits& value)
{
  const ExUnits       copy(value);
  cardano_ex_units_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_max_block_ex_units(checked(), data), "ProtocolParameters setMaxBlockExUnits");
}

std::uint64_t
ProtocolParameters::maxValueSize() const
{
  return cardano_protocol_parameters_get_max_value_size(checked());
}

void
ProtocolParameters::setMaxValueSize(std::uint64_t value)
{
  if (value > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
  {
    throw std::out_of_range("maxValueSize exceeds int64 range");
  }

  checkCardanoError(cardano_protocol_parameters_set_max_value_size(checked(), static_cast<std::int64_t>(value)), "ProtocolParameters setMaxValueSize");
}

std::uint64_t
ProtocolParameters::collateralPercentage() const
{
  return cardano_protocol_parameters_get_collateral_percentage(checked());
}

void
ProtocolParameters::setCollateralPercentage(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_collateral_percentage(checked(), value), "ProtocolParameters setCollateralPercentage");
}

std::uint64_t
ProtocolParameters::maxCollateralInputs() const
{
  return cardano_protocol_parameters_get_max_collateral_inputs(checked());
}

void
ProtocolParameters::setMaxCollateralInputs(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_collateral_inputs(checked(), value), "ProtocolParameters setMaxCollateralInputs");
}

PoolVotingThresholds
ProtocolParameters::poolVotingThresholds() const
{
  cardano_pool_voting_thresholds_t*                                                     value = cardano_protocol_parameters_get_pool_voting_thresholds(checked());
  CardanoObject<cardano_pool_voting_thresholds_t, cardano_pool_voting_thresholds_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters poolVotingThresholds is unavailable");
  }

  const PoolVotingThresholds found(owner.release());
  return PoolVotingThresholds(found);
}

void
ProtocolParameters::setPoolVotingThresholds(const PoolVotingThresholds& value)
{
  const PoolVotingThresholds        copy(value);
  cardano_pool_voting_thresholds_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_pool_voting_thresholds(checked(), data), "ProtocolParameters setPoolVotingThresholds");
}

DRepVotingThresholds
ProtocolParameters::drepVotingThresholds() const
{
  cardano_drep_voting_thresholds_t*                                                     value = cardano_protocol_parameters_get_drep_voting_thresholds(checked());
  CardanoObject<cardano_drep_voting_thresholds_t, cardano_drep_voting_thresholds_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters drepVotingThresholds is unavailable");
  }

  const DRepVotingThresholds found(owner.release());
  return DRepVotingThresholds(found);
}

void
ProtocolParameters::setDrepVotingThresholds(const DRepVotingThresholds& value)
{
  const DRepVotingThresholds        copy(value);
  cardano_drep_voting_thresholds_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_drep_voting_thresholds(checked(), data), "ProtocolParameters setDrepVotingThresholds");
}

std::uint64_t
ProtocolParameters::minCommitteeSize() const
{
  return cardano_protocol_parameters_get_min_committee_size(checked());
}

void
ProtocolParameters::setMinCommitteeSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_min_committee_size(checked(), value), "ProtocolParameters setMinCommitteeSize");
}

std::uint64_t
ProtocolParameters::committeeTermLimit() const
{
  return cardano_protocol_parameters_get_committee_term_limit(checked());
}

void
ProtocolParameters::setCommitteeTermLimit(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_committee_term_limit(checked(), value), "ProtocolParameters setCommitteeTermLimit");
}

std::uint64_t
ProtocolParameters::governanceActionValidityPeriod() const
{
  return cardano_protocol_parameters_get_governance_action_validity_period(checked());
}

void
ProtocolParameters::setGovernanceActionValidityPeriod(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_governance_action_validity_period(checked(), value), "ProtocolParameters setGovernanceActionValidityPeriod");
}

std::uint64_t
ProtocolParameters::governanceActionDeposit() const
{
  return cardano_protocol_parameters_get_governance_action_deposit(checked());
}

void
ProtocolParameters::setGovernanceActionDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_governance_action_deposit(checked(), value), "ProtocolParameters setGovernanceActionDeposit");
}

std::uint64_t
ProtocolParameters::drepDeposit() const
{
  return cardano_protocol_parameters_get_drep_deposit(checked());
}

void
ProtocolParameters::setDrepDeposit(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_drep_deposit(checked(), value), "ProtocolParameters setDrepDeposit");
}

std::uint64_t
ProtocolParameters::drepInactivityPeriod() const
{
  return cardano_protocol_parameters_get_drep_inactivity_period(checked());
}

void
ProtocolParameters::setDrepInactivityPeriod(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_drep_inactivity_period(checked(), value), "ProtocolParameters setDrepInactivityPeriod");
}

UnitInterval
ProtocolParameters::refScriptCostPerByte() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_ref_script_cost_per_byte(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters refScriptCostPerByte is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setRefScriptCostPerByte(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_ref_script_cost_per_byte(checked(), data), "ProtocolParameters setRefScriptCostPerByte");
}

std::uint64_t
ProtocolParameters::maxRefScriptSizePerBlock() const
{
  return cardano_protocol_parameters_get_max_ref_script_size_per_block(checked());
}

void
ProtocolParameters::setMaxRefScriptSizePerBlock(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_ref_script_size_per_block(checked(), value), "ProtocolParameters setMaxRefScriptSizePerBlock");
}

std::uint64_t
ProtocolParameters::maxRefScriptSizePerTx() const
{
  return cardano_protocol_parameters_get_max_ref_script_size_per_tx(checked());
}

void
ProtocolParameters::setMaxRefScriptSizePerTx(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_ref_script_size_per_tx(checked(), value), "ProtocolParameters setMaxRefScriptSizePerTx");
}

std::uint64_t
ProtocolParameters::refScriptCostStride() const
{
  return cardano_protocol_parameters_get_ref_script_cost_stride(checked());
}

void
ProtocolParameters::setRefScriptCostStride(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_ref_script_cost_stride(checked(), value), "ProtocolParameters setRefScriptCostStride");
}

std::optional<UnitInterval>
ProtocolParameters::refScriptCostMultiplier() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_ref_script_cost_multiplier(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setRefScriptCostMultiplier(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_protocol_parameters_set_ref_script_cost_multiplier(checked(), data), "ProtocolParameters setRefScriptCostMultiplier");
}

std::optional<UnitInterval>
ProtocolParameters::maxPledgeLeverage() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_max_pledge_leverage(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setMaxPledgeLeverage(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_protocol_parameters_set_max_pledge_leverage(checked(), data), "ProtocolParameters setMaxPledgeLeverage");
}

UnitInterval
ProtocolParameters::minPoolMargin() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_min_pool_margin(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters minPoolMargin is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setMinPoolMargin(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_min_pool_margin(checked(), data), "ProtocolParameters setMinPoolMargin");
}

std::uint64_t
ProtocolParameters::leiosAnnouncementPeriodLength() const
{
  return cardano_protocol_parameters_get_leios_announcement_period_length(checked());
}

void
ProtocolParameters::setLeiosAnnouncementPeriodLength(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_leios_announcement_period_length(checked(), value), "ProtocolParameters setLeiosAnnouncementPeriodLength");
}

std::uint64_t
ProtocolParameters::leiosVotePeriodLength() const
{
  return cardano_protocol_parameters_get_leios_vote_period_length(checked());
}

void
ProtocolParameters::setLeiosVotePeriodLength(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_leios_vote_period_length(checked(), value), "ProtocolParameters setLeiosVotePeriodLength");
}

std::uint64_t
ProtocolParameters::leiosDiffusionPeriodLength() const
{
  return cardano_protocol_parameters_get_leios_diffusion_period_length(checked());
}

void
ProtocolParameters::setLeiosDiffusionPeriodLength(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_leios_diffusion_period_length(checked(), value), "ProtocolParameters setLeiosDiffusionPeriodLength");
}

std::uint64_t
ProtocolParameters::leiosCommitteeSize() const
{
  return cardano_protocol_parameters_get_leios_committee_size(checked());
}

void
ProtocolParameters::setLeiosCommitteeSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_leios_committee_size(checked(), value), "ProtocolParameters setLeiosCommitteeSize");
}

UnitInterval
ProtocolParameters::leiosQuorumStakeThreshold() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_leios_quorum_stake_threshold(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters leiosQuorumStakeThreshold is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setLeiosQuorumStakeThreshold(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_leios_quorum_stake_threshold(checked(), data), "ProtocolParameters setLeiosQuorumStakeThreshold");
}

std::uint64_t
ProtocolParameters::maxEndorserBlockReferencesSize() const
{
  return cardano_protocol_parameters_get_max_endorser_block_references_size(checked());
}

void
ProtocolParameters::setMaxEndorserBlockReferencesSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_endorser_block_references_size(checked(), value), "ProtocolParameters setMaxEndorserBlockReferencesSize");
}

std::uint64_t
ProtocolParameters::maxEndorserBlockTxsSize() const
{
  return cardano_protocol_parameters_get_max_endorser_block_txs_size(checked());
}

void
ProtocolParameters::setMaxEndorserBlockTxsSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_endorser_block_txs_size(checked(), value), "ProtocolParameters setMaxEndorserBlockTxsSize");
}

ExUnits
ProtocolParameters::maxEndorserBlockExecutionUnits() const
{
  cardano_ex_units_t*                                       value = cardano_protocol_parameters_get_max_endorser_block_execution_units(checked());
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters maxEndorserBlockExecutionUnits is unavailable");
  }

  const ExUnits found(owner.release());
  return ExUnits(found);
}

void
ProtocolParameters::setMaxEndorserBlockExecutionUnits(const ExUnits& value)
{
  const ExUnits       copy(value);
  cardano_ex_units_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_max_endorser_block_execution_units(checked(), data), "ProtocolParameters setMaxEndorserBlockExecutionUnits");
}

std::uint64_t
ProtocolParameters::maxRefScriptSizePerEndorserBlock() const
{
  return cardano_protocol_parameters_get_max_ref_script_size_per_endorser_block(checked());
}

void
ProtocolParameters::setMaxRefScriptSizePerEndorserBlock(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_max_ref_script_size_per_endorser_block(checked(), value), "ProtocolParameters setMaxRefScriptSizePerEndorserBlock");
}

std::uint64_t
ProtocolParameters::perasMinCandidateBlockAge() const
{
  return cardano_protocol_parameters_get_peras_min_candidate_block_age(checked());
}

void
ProtocolParameters::setPerasMinCandidateBlockAge(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_peras_min_candidate_block_age(checked(), value), "ProtocolParameters setPerasMinCandidateBlockAge");
}

std::optional<UnitInterval>
ProtocolParameters::perasHealingFactor() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_peras_healing_factor(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setPerasHealingFactor(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? copy->checked() : nullptr;
  checkCardanoError(cardano_protocol_parameters_set_peras_healing_factor(checked(), data), "ProtocolParameters setPerasHealingFactor");
}

std::uint64_t
ProtocolParameters::perasCertBoost() const
{
  return cardano_protocol_parameters_get_peras_cert_boost(checked());
}

void
ProtocolParameters::setPerasCertBoost(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_peras_cert_boost(checked(), value), "ProtocolParameters setPerasCertBoost");
}

std::uint64_t
ProtocolParameters::perasTargetCommitteeSize() const
{
  return cardano_protocol_parameters_get_peras_target_committee_size(checked());
}

void
ProtocolParameters::setPerasTargetCommitteeSize(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_peras_target_committee_size(checked(), value), "ProtocolParameters setPerasTargetCommitteeSize");
}

std::optional<std::uint64_t>
ProtocolParameters::perasBootstrapRound() const
{
  const std::uint64_t* value = cardano_protocol_parameters_get_peras_bootstrap_round(checked());
  return value ? std::optional<std::uint64_t>(*value) : std::nullopt;
}

void
ProtocolParameters::setPerasBootstrapRound(std::optional<std::uint64_t> value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_parameters_set_peras_bootstrap_round(checked(), data), "ProtocolParameters setPerasBootstrapRound");
}

UnitInterval
ProtocolParameters::perasQuorumThresholdSafetyMargin() const
{
  cardano_unit_interval_t*                                            value = cardano_protocol_parameters_get_peras_quorum_threshold_safety_margin(checked());
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (!value)
  {
    throw std::logic_error("ProtocolParameters perasQuorumThresholdSafetyMargin is unavailable");
  }

  const UnitInterval found(owner.release());
  return UnitInterval(found);
}

void
ProtocolParameters::setPerasQuorumThresholdSafetyMargin(const UnitInterval& value)
{
  const UnitInterval       copy(value);
  cardano_unit_interval_t* data = copy.checked();
  checkCardanoError(cardano_protocol_parameters_set_peras_quorum_threshold_safety_margin(checked(), data), "ProtocolParameters setPerasQuorumThresholdSafetyMargin");
}

std::uint64_t
ProtocolParameters::refInputsCostPerMultiAssetPolicy() const
{
  return cardano_protocol_parameters_get_ref_inputs_cost_per_multi_asset_policy(checked());
}

void
ProtocolParameters::setRefInputsCostPerMultiAssetPolicy(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_ref_inputs_cost_per_multi_asset_policy(checked(), value), "ProtocolParameters setRefInputsCostPerMultiAssetPolicy");
}

std::uint64_t
ProtocolParameters::refInputsCostPerDatumByte() const
{
  return cardano_protocol_parameters_get_ref_inputs_cost_per_datum_byte(checked());
}

void
ProtocolParameters::setRefInputsCostPerDatumByte(std::uint64_t value)
{
  checkCardanoError(cardano_protocol_parameters_set_ref_inputs_cost_per_datum_byte(checked(), value), "ProtocolParameters setRefInputsCostPerDatumByte");
}

void
ProtocolParameters::swap(ProtocolParameters& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ProtocolParameters&
ProtocolParameters::operator=(const ProtocolParameters& other)
{
  if (this != &other)
  {
    ProtocolParameters copy(other);
    swap(copy);
  }
  return *this;
}

ProtocolParameters&
ProtocolParameters::operator=(ProtocolParameters&& other) noexcept
{
  if (this != &other)
  {
    cardano_protocol_parameters_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ProtocolParameters::operator==(const ProtocolParameters& other) const
{
  if (minFeeA() != other.minFeeA())
  {
    return false;
  }

  if (minFeeB() != other.minFeeB())
  {
    return false;
  }

  if (maxBlockBodySize() != other.maxBlockBodySize())
  {
    return false;
  }

  if (maxTxSize() != other.maxTxSize())
  {
    return false;
  }

  if (maxBlockHeaderSize() != other.maxBlockHeaderSize())
  {
    return false;
  }

  if (keyDeposit() != other.keyDeposit())
  {
    return false;
  }

  if (poolDeposit() != other.poolDeposit())
  {
    return false;
  }

  if (maxEpoch() != other.maxEpoch())
  {
    return false;
  }

  if (nOpt() != other.nOpt())
  {
    return false;
  }

  if (poolPledgeInfluence() != other.poolPledgeInfluence())
  {
    return false;
  }

  if (expansionRate() != other.expansionRate())
  {
    return false;
  }

  if (treasuryGrowthRate() != other.treasuryGrowthRate())
  {
    return false;
  }

  if (d() != other.d())
  {
    return false;
  }

  if (extraEntropy() != other.extraEntropy())
  {
    return false;
  }

  if (protocolVersion() != other.protocolVersion())
  {
    return false;
  }

  if (minPoolCost() != other.minPoolCost())
  {
    return false;
  }

  if (adaPerUtxoByte() != other.adaPerUtxoByte())
  {
    return false;
  }

  if (costModels() != other.costModels())
  {
    return false;
  }

  if (executionCosts() != other.executionCosts())
  {
    return false;
  }

  if (maxTxExUnits() != other.maxTxExUnits())
  {
    return false;
  }

  if (maxBlockExUnits() != other.maxBlockExUnits())
  {
    return false;
  }

  if (maxValueSize() != other.maxValueSize())
  {
    return false;
  }

  if (collateralPercentage() != other.collateralPercentage())
  {
    return false;
  }

  if (maxCollateralInputs() != other.maxCollateralInputs())
  {
    return false;
  }

  if (poolVotingThresholds() != other.poolVotingThresholds())
  {
    return false;
  }

  if (drepVotingThresholds() != other.drepVotingThresholds())
  {
    return false;
  }

  if (minCommitteeSize() != other.minCommitteeSize())
  {
    return false;
  }

  if (committeeTermLimit() != other.committeeTermLimit())
  {
    return false;
  }

  if (governanceActionValidityPeriod() != other.governanceActionValidityPeriod())
  {
    return false;
  }

  if (governanceActionDeposit() != other.governanceActionDeposit())
  {
    return false;
  }

  if (drepDeposit() != other.drepDeposit())
  {
    return false;
  }

  if (drepInactivityPeriod() != other.drepInactivityPeriod())
  {
    return false;
  }

  if (refScriptCostPerByte() != other.refScriptCostPerByte())
  {
    return false;
  }

  if (maxRefScriptSizePerBlock() != other.maxRefScriptSizePerBlock())
  {
    return false;
  }

  if (maxRefScriptSizePerTx() != other.maxRefScriptSizePerTx())
  {
    return false;
  }

  if (refScriptCostStride() != other.refScriptCostStride())
  {
    return false;
  }

  if (refScriptCostMultiplier() != other.refScriptCostMultiplier())
  {
    return false;
  }

  if (maxPledgeLeverage() != other.maxPledgeLeverage())
  {
    return false;
  }

  if (minPoolMargin() != other.minPoolMargin())
  {
    return false;
  }

  if (leiosAnnouncementPeriodLength() != other.leiosAnnouncementPeriodLength())
  {
    return false;
  }

  if (leiosVotePeriodLength() != other.leiosVotePeriodLength())
  {
    return false;
  }

  if (leiosDiffusionPeriodLength() != other.leiosDiffusionPeriodLength())
  {
    return false;
  }

  if (leiosCommitteeSize() != other.leiosCommitteeSize())
  {
    return false;
  }

  if (leiosQuorumStakeThreshold() != other.leiosQuorumStakeThreshold())
  {
    return false;
  }

  if (maxEndorserBlockReferencesSize() != other.maxEndorserBlockReferencesSize())
  {
    return false;
  }

  if (maxEndorserBlockTxsSize() != other.maxEndorserBlockTxsSize())
  {
    return false;
  }

  if (maxEndorserBlockExecutionUnits() != other.maxEndorserBlockExecutionUnits())
  {
    return false;
  }

  if (maxRefScriptSizePerEndorserBlock() != other.maxRefScriptSizePerEndorserBlock())
  {
    return false;
  }

  if (perasMinCandidateBlockAge() != other.perasMinCandidateBlockAge())
  {
    return false;
  }

  if (perasHealingFactor() != other.perasHealingFactor())
  {
    return false;
  }

  if (perasCertBoost() != other.perasCertBoost())
  {
    return false;
  }

  if (perasTargetCommitteeSize() != other.perasTargetCommitteeSize())
  {
    return false;
  }

  if (perasBootstrapRound() != other.perasBootstrapRound())
  {
    return false;
  }

  if (perasQuorumThresholdSafetyMargin() != other.perasQuorumThresholdSafetyMargin())
  {
    return false;
  }

  if (refInputsCostPerMultiAssetPolicy() != other.refInputsCostPerMultiAssetPolicy())
  {
    return false;
  }

  if (refInputsCostPerDatumByte() != other.refInputsCostPerDatumByte())
  {
    return false;
  }

  return true;
}

bool
ProtocolParameters::operator!=(const ProtocolParameters& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

cardano_protocol_parameters_t*
ProtocolParameters::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe moved-from ProtocolParameters");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

cardano_protocol_parameters_t*
copyProtocolParametersToNative(const ProtocolParameters& parameters)
{
  cardano_protocol_parameters_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_protocol_parameters_deep_clone(parameters.checked(), &value);
  CardanoObject<cardano_protocol_parameters_t, cardano_protocol_parameters_unref> owner(value);

  checkCardanoError(status, "Copy protocol parameters to native");

  return owner.release();
}

ProtocolParameters
copyProtocolParametersFromNative(const cardano_protocol_parameters_t* parameters)
{
  ProtocolParameters result;

  cardano_protocol_parameters_t*                                                  value  = nullptr;
  const cardano_error_t                                                           status = cardano_protocol_parameters_deep_clone(parameters, &value);
  CardanoObject<cardano_protocol_parameters_t, cardano_protocol_parameters_unref> owner(value);

  checkCardanoError(status, "Copy protocol parameters from native");
  cardano_protocol_parameters_unref(&result.m_value);
  result.m_value = owner.release();

  return result;
}

void
swap(ProtocolParameters& lhs, ProtocolParameters& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
