/**
 * @file ProtocolParamUpdate.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/ProtocolParams/ProtocolParamUpdate.h>

#include "../CardanoInterop.h"

#include <cardano/buffer.h>
#include <cardano/common/ex_units.h>
#include <cardano/common/protocol_version.h>
#include <cardano/common/unit_interval.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/protocol_params/costmdls.h>
#include <cardano/protocol_params/drep_voting_thresholds.h>
#include <cardano/protocol_params/ex_unit_prices.h>
#include <cardano/protocol_params/pool_voting_thresholds.h>
#include <cardano/protocol_params/protocol_param_update.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ProtocolParamUpdate::ProtocolParamUpdate()
: m_value(nullptr)
{
  cardano_protocol_param_update_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_protocol_param_update_new(&value);
  CardanoObject<cardano_protocol_param_update_t, cardano_protocol_param_update_unref> owner(value);

  checkCardanoError(status, "ProtocolParamUpdate");
  m_value = owner.release();
}

ProtocolParamUpdate::ProtocolParamUpdate(const ProtocolParamUpdate& other)
: m_value(nullptr)
{
  cardano_protocol_param_update_t*                                                    value  = nullptr;
  const cardano_error_t                                                               status = cardano_protocol_param_update_deep_clone(other.checked(), &value);
  CardanoObject<cardano_protocol_param_update_t, cardano_protocol_param_update_unref> owner(value);

  checkCardanoError(status, "ProtocolParamUpdate copy");
  m_value = owner.release();
}

ProtocolParamUpdate::ProtocolParamUpdate(ProtocolParamUpdate&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ProtocolParamUpdate::~ProtocolParamUpdate()
{
  cardano_protocol_param_update_unref(&m_value);
}

ProtocolParamUpdate
ProtocolParamUpdate::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ProtocolParamUpdate(decodeCbor<cardano_protocol_param_update_t, cardano_protocol_param_update_unref>(cbor, cardano_protocol_param_update_from_cbor));
}

ProtocolParamUpdate
ProtocolParamUpdate::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::optional<std::uint64_t>
ProtocolParamUpdate::minFeeA() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_min_fee_a(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate minFeeA");
  return value;
}

void
ProtocolParamUpdate::setMinFeeA(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_min_fee_a(checked(), data), "ProtocolParamUpdate setMinFeeA");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::minFeeB() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_min_fee_b(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate minFeeB");
  return value;
}

void
ProtocolParamUpdate::setMinFeeB(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_min_fee_b(checked(), data), "ProtocolParamUpdate setMinFeeB");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxBlockBodySize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_block_body_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxBlockBodySize");
  return value;
}

void
ProtocolParamUpdate::setMaxBlockBodySize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_block_body_size(checked(), data), "ProtocolParamUpdate setMaxBlockBodySize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxTxSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_tx_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxTxSize");
  return value;
}

void
ProtocolParamUpdate::setMaxTxSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_tx_size(checked(), data), "ProtocolParamUpdate setMaxTxSize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxBlockHeaderSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_block_header_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxBlockHeaderSize");
  return value;
}

void
ProtocolParamUpdate::setMaxBlockHeaderSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_block_header_size(checked(), data), "ProtocolParamUpdate setMaxBlockHeaderSize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::keyDeposit() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_key_deposit(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate keyDeposit");
  return value;
}

void
ProtocolParamUpdate::setKeyDeposit(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_key_deposit(checked(), data), "ProtocolParamUpdate setKeyDeposit");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::poolDeposit() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_pool_deposit(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate poolDeposit");
  return value;
}

void
ProtocolParamUpdate::setPoolDeposit(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_pool_deposit(checked(), data), "ProtocolParamUpdate setPoolDeposit");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxEpoch() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_epoch(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxEpoch");
  return value;
}

void
ProtocolParamUpdate::setMaxEpoch(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_epoch(checked(), data), "ProtocolParamUpdate setMaxEpoch");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::nOpt() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_n_opt(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate nOpt");
  return value;
}

void
ProtocolParamUpdate::setNOpt(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_n_opt(checked(), data), "ProtocolParamUpdate setNOpt");
}

std::optional<UnitInterval>
ProtocolParamUpdate::poolPledgeInfluence() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_pool_pledge_influence(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate poolPledgeInfluence");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setPoolPledgeInfluence(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_pool_pledge_influence(checked(), data), "ProtocolParamUpdate setPoolPledgeInfluence");
}

std::optional<UnitInterval>
ProtocolParamUpdate::expansionRate() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_expansion_rate(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate expansionRate");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setExpansionRate(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_expansion_rate(checked(), data), "ProtocolParamUpdate setExpansionRate");
}

std::optional<UnitInterval>
ProtocolParamUpdate::treasuryGrowthRate() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_treasury_growth_rate(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate treasuryGrowthRate");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setTreasuryGrowthRate(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_treasury_growth_rate(checked(), data), "ProtocolParamUpdate setTreasuryGrowthRate");
}

std::optional<UnitInterval>
ProtocolParamUpdate::d() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_d(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate d");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setD(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_d(checked(), data), "ProtocolParamUpdate setD");
}

std::optional<std::vector<std::uint8_t>>
ProtocolParamUpdate::extraEntropy() const
{
  cardano_buffer_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_protocol_param_update_get_extra_entropy(checked(), &value);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate extraEntropy");
  const std::size_t length = cardano_buffer_get_size(owner.get());

  if (length == 0)
  {
    return std::vector<std::uint8_t> {};
  }

  const std::uint8_t* bytes = cardano_buffer_get_data(owner.get());
  return std::vector<std::uint8_t>(bytes, bytes + length);
}

void
ProtocolParamUpdate::setExtraEntropy(const std::optional<std::vector<std::uint8_t>>& value)
{
  cardano_buffer_t*                                     buffer = value ? cardano_buffer_new(value->size()) : nullptr;
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(buffer);

  if (value && !buffer)
  {
    throw std::bad_alloc();
  }

  if (value && !value->empty())
  {
    checkCardanoError(cardano_buffer_write(buffer, value->data(), value->size()), "ProtocolParamUpdate setExtraEntropy bytes");
  }

  checkCardanoError(cardano_protocol_param_update_set_extra_entropy(checked(), buffer), "ProtocolParamUpdate setExtraEntropy");
}

std::optional<ProtocolVersion>
ProtocolParamUpdate::protocolVersion() const
{
  cardano_protocol_version_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_protocol_param_update_get_protocol_version(checked(), &value);
  CardanoObject<cardano_protocol_version_t, cardano_protocol_version_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate protocolVersion");

  if (!value)
  {
    return std::nullopt;
  }

  const ProtocolVersion result = ProtocolVersion::adopt(owner.release());
  return ProtocolVersion(result);
}

void
ProtocolParamUpdate::setProtocolVersion(const std::optional<ProtocolVersion>& value)
{
  const std::optional<ProtocolVersion> copy(value);
  cardano_protocol_version_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_protocol_version(checked(), data), "ProtocolParamUpdate setProtocolVersion");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::minPoolCost() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_min_pool_cost(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate minPoolCost");
  return value;
}

void
ProtocolParamUpdate::setMinPoolCost(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_min_pool_cost(checked(), data), "ProtocolParamUpdate setMinPoolCost");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::adaPerUtxoByte() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_ada_per_utxo_byte(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate adaPerUtxoByte");
  return value;
}

void
ProtocolParamUpdate::setAdaPerUtxoByte(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_ada_per_utxo_byte(checked(), data), "ProtocolParamUpdate setAdaPerUtxoByte");
}

std::optional<CostModels>
ProtocolParamUpdate::costModels() const
{
  cardano_costmdls_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_protocol_param_update_get_cost_models(checked(), &value);
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate costModels");

  if (!value)
  {
    return std::nullopt;
  }

  const CostModels result = CostModels::adopt(owner.release());
  return CostModels(result);
}

void
ProtocolParamUpdate::setCostModels(const std::optional<CostModels>& value)
{
  const std::optional<CostModels> copy(value);
  cardano_costmdls_t*             data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_cost_models(checked(), data), "ProtocolParamUpdate setCostModels");
}

std::optional<ExUnitPrices>
ProtocolParamUpdate::executionCosts() const
{
  cardano_ex_unit_prices_t*                                             value  = nullptr;
  const cardano_error_t                                                 status = cardano_protocol_param_update_get_execution_costs(checked(), &value);
  CardanoObject<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate executionCosts");

  if (!value)
  {
    return std::nullopt;
  }

  const ExUnitPrices result = ExUnitPrices::adopt(owner.release());
  return ExUnitPrices(result);
}

void
ProtocolParamUpdate::setExecutionCosts(const std::optional<ExUnitPrices>& value)
{
  const std::optional<ExUnitPrices> copy(value);
  cardano_ex_unit_prices_t*         data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_execution_costs(checked(), data), "ProtocolParamUpdate setExecutionCosts");
}

std::optional<ExUnits>
ProtocolParamUpdate::maxTxExUnits() const
{
  cardano_ex_units_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_protocol_param_update_get_max_tx_ex_units(checked(), &value);
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxTxExUnits");

  if (!value)
  {
    return std::nullopt;
  }

  const ExUnits result = ExUnits::adopt(owner.release());
  return ExUnits(result);
}

void
ProtocolParamUpdate::setMaxTxExUnits(const std::optional<ExUnits>& value)
{
  const std::optional<ExUnits> copy(value);
  cardano_ex_units_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_tx_ex_units(checked(), data), "ProtocolParamUpdate setMaxTxExUnits");
}

std::optional<ExUnits>
ProtocolParamUpdate::maxBlockExUnits() const
{
  cardano_ex_units_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_protocol_param_update_get_max_block_ex_units(checked(), &value);
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxBlockExUnits");

  if (!value)
  {
    return std::nullopt;
  }

  const ExUnits result = ExUnits::adopt(owner.release());
  return ExUnits(result);
}

void
ProtocolParamUpdate::setMaxBlockExUnits(const std::optional<ExUnits>& value)
{
  const std::optional<ExUnits> copy(value);
  cardano_ex_units_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_block_ex_units(checked(), data), "ProtocolParamUpdate setMaxBlockExUnits");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxValueSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_value_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxValueSize");
  return value;
}

void
ProtocolParamUpdate::setMaxValueSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_value_size(checked(), data), "ProtocolParamUpdate setMaxValueSize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::collateralPercentage() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_collateral_percentage(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate collateralPercentage");
  return value;
}

void
ProtocolParamUpdate::setCollateralPercentage(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_collateral_percentage(checked(), data), "ProtocolParamUpdate setCollateralPercentage");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxCollateralInputs() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_collateral_inputs(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxCollateralInputs");
  return value;
}

void
ProtocolParamUpdate::setMaxCollateralInputs(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_collateral_inputs(checked(), data), "ProtocolParamUpdate setMaxCollateralInputs");
}

std::optional<PoolVotingThresholds>
ProtocolParamUpdate::poolVotingThresholds() const
{
  cardano_pool_voting_thresholds_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_protocol_param_update_get_pool_voting_thresholds(checked(), &value);
  CardanoObject<cardano_pool_voting_thresholds_t, cardano_pool_voting_thresholds_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate poolVotingThresholds");

  if (!value)
  {
    return std::nullopt;
  }

  const PoolVotingThresholds result = PoolVotingThresholds::adopt(owner.release());
  return PoolVotingThresholds(result);
}

void
ProtocolParamUpdate::setPoolVotingThresholds(const std::optional<PoolVotingThresholds>& value)
{
  const std::optional<PoolVotingThresholds> copy(value);
  cardano_pool_voting_thresholds_t*         data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_pool_voting_thresholds(checked(), data), "ProtocolParamUpdate setPoolVotingThresholds");
}

std::optional<DRepVotingThresholds>
ProtocolParamUpdate::drepVotingThresholds() const
{
  cardano_drep_voting_thresholds_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_protocol_param_update_get_drep_voting_thresholds(checked(), &value);
  CardanoObject<cardano_drep_voting_thresholds_t, cardano_drep_voting_thresholds_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate drepVotingThresholds");

  if (!value)
  {
    return std::nullopt;
  }

  const DRepVotingThresholds result = DRepVotingThresholds::adopt(owner.release());
  return DRepVotingThresholds(result);
}

void
ProtocolParamUpdate::setDrepVotingThresholds(const std::optional<DRepVotingThresholds>& value)
{
  const std::optional<DRepVotingThresholds> copy(value);
  cardano_drep_voting_thresholds_t*         data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_drep_voting_thresholds(checked(), data), "ProtocolParamUpdate setDrepVotingThresholds");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::minCommitteeSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_min_committee_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate minCommitteeSize");
  return value;
}

void
ProtocolParamUpdate::setMinCommitteeSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_min_committee_size(checked(), data), "ProtocolParamUpdate setMinCommitteeSize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::committeeTermLimit() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_committee_term_limit(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate committeeTermLimit");
  return value;
}

void
ProtocolParamUpdate::setCommitteeTermLimit(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_committee_term_limit(checked(), data), "ProtocolParamUpdate setCommitteeTermLimit");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::governanceActionValidityPeriod() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_governance_action_validity_period(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate governanceActionValidityPeriod");
  return value;
}

void
ProtocolParamUpdate::setGovernanceActionValidityPeriod(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_governance_action_validity_period(checked(), data), "ProtocolParamUpdate setGovernanceActionValidityPeriod");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::governanceActionDeposit() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_governance_action_deposit(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate governanceActionDeposit");
  return value;
}

void
ProtocolParamUpdate::setGovernanceActionDeposit(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_governance_action_deposit(checked(), data), "ProtocolParamUpdate setGovernanceActionDeposit");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::drepDeposit() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_drep_deposit(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate drepDeposit");
  return value;
}

void
ProtocolParamUpdate::setDrepDeposit(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_drep_deposit(checked(), data), "ProtocolParamUpdate setDrepDeposit");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::drepInactivityPeriod() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_drep_inactivity_period(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate drepInactivityPeriod");
  return value;
}

void
ProtocolParamUpdate::setDrepInactivityPeriod(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_drep_inactivity_period(checked(), data), "ProtocolParamUpdate setDrepInactivityPeriod");
}

std::optional<UnitInterval>
ProtocolParamUpdate::refScriptCostPerByte() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_ref_script_cost_per_byte(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate refScriptCostPerByte");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setRefScriptCostPerByte(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_ref_script_cost_per_byte(checked(), data), "ProtocolParamUpdate setRefScriptCostPerByte");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxRefScriptSizePerBlock() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_ref_script_size_per_block(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxRefScriptSizePerBlock");
  return value;
}

void
ProtocolParamUpdate::setMaxRefScriptSizePerBlock(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_ref_script_size_per_block(checked(), data), "ProtocolParamUpdate setMaxRefScriptSizePerBlock");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxRefScriptSizePerTx() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_ref_script_size_per_tx(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxRefScriptSizePerTx");
  return value;
}

void
ProtocolParamUpdate::setMaxRefScriptSizePerTx(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_ref_script_size_per_tx(checked(), data), "ProtocolParamUpdate setMaxRefScriptSizePerTx");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::refScriptCostStride() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_ref_script_cost_stride(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate refScriptCostStride");
  return value;
}

void
ProtocolParamUpdate::setRefScriptCostStride(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_ref_script_cost_stride(checked(), data), "ProtocolParamUpdate setRefScriptCostStride");
}

std::optional<UnitInterval>
ProtocolParamUpdate::refScriptCostMultiplier() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_ref_script_cost_multiplier(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate refScriptCostMultiplier");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setRefScriptCostMultiplier(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_ref_script_cost_multiplier(checked(), data), "ProtocolParamUpdate setRefScriptCostMultiplier");
}

std::optional<UnitInterval>
ProtocolParamUpdate::maxPledgeLeverage() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_max_pledge_leverage(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxPledgeLeverage");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setMaxPledgeLeverage(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_pledge_leverage(checked(), data), "ProtocolParamUpdate setMaxPledgeLeverage");
}

std::optional<UnitInterval>
ProtocolParamUpdate::minPoolMargin() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_min_pool_margin(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate minPoolMargin");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setMinPoolMargin(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_min_pool_margin(checked(), data), "ProtocolParamUpdate setMinPoolMargin");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::leiosAnnouncementPeriodLength() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_leios_announcement_period_length(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate leiosAnnouncementPeriodLength");
  return value;
}

void
ProtocolParamUpdate::setLeiosAnnouncementPeriodLength(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_leios_announcement_period_length(checked(), data), "ProtocolParamUpdate setLeiosAnnouncementPeriodLength");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::leiosVotePeriodLength() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_leios_vote_period_length(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate leiosVotePeriodLength");
  return value;
}

void
ProtocolParamUpdate::setLeiosVotePeriodLength(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_leios_vote_period_length(checked(), data), "ProtocolParamUpdate setLeiosVotePeriodLength");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::leiosDiffusionPeriodLength() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_leios_diffusion_period_length(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate leiosDiffusionPeriodLength");
  return value;
}

void
ProtocolParamUpdate::setLeiosDiffusionPeriodLength(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_leios_diffusion_period_length(checked(), data), "ProtocolParamUpdate setLeiosDiffusionPeriodLength");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::leiosCommitteeSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_leios_committee_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate leiosCommitteeSize");
  return value;
}

void
ProtocolParamUpdate::setLeiosCommitteeSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_leios_committee_size(checked(), data), "ProtocolParamUpdate setLeiosCommitteeSize");
}

std::optional<UnitInterval>
ProtocolParamUpdate::leiosQuorumStakeThreshold() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_leios_quorum_stake_threshold(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate leiosQuorumStakeThreshold");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setLeiosQuorumStakeThreshold(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_leios_quorum_stake_threshold(checked(), data), "ProtocolParamUpdate setLeiosQuorumStakeThreshold");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxEndorserBlockReferencesSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_endorser_block_references_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxEndorserBlockReferencesSize");
  return value;
}

void
ProtocolParamUpdate::setMaxEndorserBlockReferencesSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_endorser_block_references_size(checked(), data), "ProtocolParamUpdate setMaxEndorserBlockReferencesSize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxEndorserBlockTxsSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_endorser_block_txs_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxEndorserBlockTxsSize");
  return value;
}

void
ProtocolParamUpdate::setMaxEndorserBlockTxsSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_endorser_block_txs_size(checked(), data), "ProtocolParamUpdate setMaxEndorserBlockTxsSize");
}

std::optional<ExUnits>
ProtocolParamUpdate::maxEndorserBlockExecutionUnits() const
{
  cardano_ex_units_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_protocol_param_update_get_max_endorser_block_execution_units(checked(), &value);
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxEndorserBlockExecutionUnits");

  if (!value)
  {
    return std::nullopt;
  }

  const ExUnits result = ExUnits::adopt(owner.release());
  return ExUnits(result);
}

void
ProtocolParamUpdate::setMaxEndorserBlockExecutionUnits(const std::optional<ExUnits>& value)
{
  const std::optional<ExUnits> copy(value);
  cardano_ex_units_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_endorser_block_execution_units(checked(), data), "ProtocolParamUpdate setMaxEndorserBlockExecutionUnits");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::maxRefScriptSizePerEndorserBlock() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_max_ref_script_size_per_endorser_block(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate maxRefScriptSizePerEndorserBlock");
  return value;
}

void
ProtocolParamUpdate::setMaxRefScriptSizePerEndorserBlock(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_max_ref_script_size_per_endorser_block(checked(), data), "ProtocolParamUpdate setMaxRefScriptSizePerEndorserBlock");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::perasMinCandidateBlockAge() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_peras_min_candidate_block_age(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate perasMinCandidateBlockAge");
  return value;
}

void
ProtocolParamUpdate::setPerasMinCandidateBlockAge(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_peras_min_candidate_block_age(checked(), data), "ProtocolParamUpdate setPerasMinCandidateBlockAge");
}

std::optional<UnitInterval>
ProtocolParamUpdate::perasHealingFactor() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_peras_healing_factor(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate perasHealingFactor");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setPerasHealingFactor(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_peras_healing_factor(checked(), data), "ProtocolParamUpdate setPerasHealingFactor");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::perasCertBoost() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_peras_cert_boost(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate perasCertBoost");
  return value;
}

void
ProtocolParamUpdate::setPerasCertBoost(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_peras_cert_boost(checked(), data), "ProtocolParamUpdate setPerasCertBoost");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::perasTargetCommitteeSize() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_peras_target_committee_size(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate perasTargetCommitteeSize");
  return value;
}

void
ProtocolParamUpdate::setPerasTargetCommitteeSize(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_peras_target_committee_size(checked(), data), "ProtocolParamUpdate setPerasTargetCommitteeSize");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::perasBootstrapRound() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_peras_bootstrap_round(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate perasBootstrapRound");
  return value;
}

void
ProtocolParamUpdate::setPerasBootstrapRound(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_peras_bootstrap_round(checked(), data), "ProtocolParamUpdate setPerasBootstrapRound");
}

std::optional<UnitInterval>
ProtocolParamUpdate::perasQuorumThresholdSafetyMargin() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_protocol_param_update_get_peras_quorum_threshold_safety_margin(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate perasQuorumThresholdSafetyMargin");

  if (!value)
  {
    return std::nullopt;
  }

  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
ProtocolParamUpdate::setPerasQuorumThresholdSafetyMargin(const std::optional<UnitInterval>& value)
{
  const std::optional<UnitInterval> copy(value);
  cardano_unit_interval_t*          data = copy ? checkedNativeHandle(*copy) : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_peras_quorum_threshold_safety_margin(checked(), data), "ProtocolParamUpdate setPerasQuorumThresholdSafetyMargin");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::refInputsCostPerMultiAssetPolicy() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_ref_inputs_cost_per_multi_asset_policy(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate refInputsCostPerMultiAssetPolicy");
  return value;
}

void
ProtocolParamUpdate::setRefInputsCostPerMultiAssetPolicy(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_ref_inputs_cost_per_multi_asset_policy(checked(), data), "ProtocolParamUpdate setRefInputsCostPerMultiAssetPolicy");
}

std::optional<std::uint64_t>
ProtocolParamUpdate::refInputsCostPerDatumByte() const
{
  std::uint64_t         value  = 0;
  const cardano_error_t status = cardano_protocol_param_update_get_ref_inputs_cost_per_datum_byte(checked(), &value);

  if (status == CARDANO_ERROR_ELEMENT_NOT_FOUND)
  {
    return std::nullopt;
  }

  checkCardanoError(status, "ProtocolParamUpdate refInputsCostPerDatumByte");
  return value;
}

void
ProtocolParamUpdate::setRefInputsCostPerDatumByte(const std::optional<std::uint64_t>& value)
{
  const std::uint64_t* data = value ? &*value : nullptr;
  checkCardanoError(cardano_protocol_param_update_set_ref_inputs_cost_per_datum_byte(checked(), data), "ProtocolParamUpdate setRefInputsCostPerDatumByte");
}

bool
ProtocolParamUpdate::hasMaxPledgeLeverage() const
{
  return cardano_protocol_param_update_has_max_pledge_leverage(checked());
}

bool
ProtocolParamUpdate::isMaxPledgeLeverageUnbounded() const
{
  return hasMaxPledgeLeverage() && !maxPledgeLeverage().has_value();
}

void
ProtocolParamUpdate::setMaxPledgeLeverageUnbounded()
{
  checkCardanoError(cardano_protocol_param_update_set_max_pledge_leverage_unbounded(checked()), "ProtocolParamUpdate setMaxPledgeLeverageUnbounded");
}

void
ProtocolParamUpdate::clearMaxPledgeLeverage()
{
  checkCardanoError(cardano_protocol_param_update_clear_max_pledge_leverage(checked()), "ProtocolParamUpdate clearMaxPledgeLeverage");
}

bool
ProtocolParamUpdate::hasPerasBootstrapRound() const
{
  return cardano_protocol_param_update_has_peras_bootstrap_round(checked());
}

bool
ProtocolParamUpdate::isPerasBootstrapRoundNull() const
{
  return cardano_protocol_param_update_is_peras_bootstrap_round_null(checked());
}

void
ProtocolParamUpdate::setPerasBootstrapRoundNull()
{
  checkCardanoError(cardano_protocol_param_update_set_peras_bootstrap_round_null(checked()), "ProtocolParamUpdate setPerasBootstrapRoundNull");
}

void
ProtocolParamUpdate::clearPerasBootstrapRound()
{
  checkCardanoError(cardano_protocol_param_update_clear_peras_bootstrap_round(checked()), "ProtocolParamUpdate clearPerasBootstrapRound");
}

std::vector<std::uint8_t>
ProtocolParamUpdate::toCbor() const
{
  return encodeCbor(checked(), cardano_protocol_param_update_to_cbor);
}

PlutusData
ProtocolParamUpdate::toPlutusData() const
{
  cardano_plutus_data_t*                                          value  = nullptr;
  const cardano_error_t                                           status = cardano_protocol_param_update_to_plutus_data(checked(), &value);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> owner(value);

  checkCardanoError(status, "ProtocolParamUpdate toPlutusData");
  return PlutusData::adopt(owner.release());
}

std::string
ProtocolParamUpdate::toJson() const
{
  return encodeJson(checked(), cardano_protocol_param_update_to_cip116_json);
}

void
ProtocolParamUpdate::swap(ProtocolParamUpdate& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ProtocolParamUpdate&
ProtocolParamUpdate::operator=(const ProtocolParamUpdate& other)
{
  if (this != &other)
  {
    ProtocolParamUpdate copy(other);
    swap(copy);
  }
  return *this;
}

ProtocolParamUpdate&
ProtocolParamUpdate::operator=(ProtocolParamUpdate&& other) noexcept
{
  if (this != &other)
  {
    cardano_protocol_param_update_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
ProtocolParamUpdate::operator==(const ProtocolParamUpdate& other) const
{
  return toCbor() == other.toCbor();
}

bool
ProtocolParamUpdate::operator!=(const ProtocolParamUpdate& other) const
{
  return !(*this == other);
}

ProtocolParamUpdate
ProtocolParamUpdate::adopt(cardano_protocol_param_update_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null ProtocolParamUpdate");
  }

  return ProtocolParamUpdate(value);
}

cardano_protocol_param_update_t*
ProtocolParamUpdate::nativeHandle() noexcept
{
  return m_value;
}

const cardano_protocol_param_update_t*
ProtocolParamUpdate::nativeHandle() const noexcept
{
  return m_value;
}

cardano_protocol_param_update_t*
ProtocolParamUpdate::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

ProtocolParamUpdate::ProtocolParamUpdate(cardano_protocol_param_update_t* value) noexcept
: m_value(value)
{
}

cardano_protocol_param_update_t*
ProtocolParamUpdate::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ProtocolParamUpdate");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ProtocolParamUpdate& lhs, ProtocolParamUpdate& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
