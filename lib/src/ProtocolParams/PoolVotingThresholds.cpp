/**
 * @file PoolVotingThresholds.cpp
 *
 * @brief Governance voting thresholds.
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

#include <Cometa/ProtocolParams/PoolVotingThresholds.h>

#include "../CardanoInterop.h"

#include <cardano/common/unit_interval.h>
#include <cardano/protocol_params/pool_voting_thresholds.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
PoolVotingThresholds::PoolVotingThresholds(
  const UnitInterval& motionNoConfidence,
  const UnitInterval& committeeNormal,
  const UnitInterval& committeeNoConfidence,
  const UnitInterval& hardForkInitiation,
  const UnitInterval& securityRelevantParam)
: m_value(nullptr)
{
  const UnitInterval                motionNoConfidenceCopy(motionNoConfidence);
  const UnitInterval                committeeNormalCopy(committeeNormal);
  const UnitInterval                committeeNoConfidenceCopy(committeeNoConfidence);
  const UnitInterval                hardForkInitiationCopy(hardForkInitiation);
  const UnitInterval                securityRelevantParamCopy(securityRelevantParam);
  cardano_pool_voting_thresholds_t* value  = nullptr;
  const cardano_error_t             status = cardano_pool_voting_thresholds_new(
    checkedNativeHandle(motionNoConfidenceCopy),
    checkedNativeHandle(committeeNormalCopy),
    checkedNativeHandle(committeeNoConfidenceCopy),
    checkedNativeHandle(hardForkInitiationCopy),
    checkedNativeHandle(securityRelevantParamCopy),
    &value);
  CardanoObject<cardano_pool_voting_thresholds_t, cardano_pool_voting_thresholds_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds");
  m_value = owner.release();
}

PoolVotingThresholds::PoolVotingThresholds(const PoolVotingThresholds& other)
: m_value(nullptr)
{
  cardano_pool_voting_thresholds_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_pool_voting_thresholds_deep_clone(other.checked(), &value);
  CardanoObject<cardano_pool_voting_thresholds_t, cardano_pool_voting_thresholds_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds copy");
  m_value = owner.release();
}

PoolVotingThresholds::PoolVotingThresholds(PoolVotingThresholds&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

PoolVotingThresholds::~PoolVotingThresholds()
{
  cardano_pool_voting_thresholds_unref(&m_value);
}

PoolVotingThresholds
PoolVotingThresholds::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return PoolVotingThresholds(decodeCbor<cardano_pool_voting_thresholds_t, cardano_pool_voting_thresholds_unref>(cbor, cardano_pool_voting_thresholds_from_cbor));
}

PoolVotingThresholds
PoolVotingThresholds::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

UnitInterval
PoolVotingThresholds::motionNoConfidence() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_voting_thresholds_get_motion_no_confidence(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds motionNoConfidence");
  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
PoolVotingThresholds::setMotionNoConfidence(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_pool_voting_thresholds_set_motion_no_confidence(checked(), checkedNativeHandle(copy)), "PoolVotingThresholds setMotionNoConfidence");
}

UnitInterval
PoolVotingThresholds::committeeNormal() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_voting_thresholds_get_committee_normal(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds committeeNormal");
  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
PoolVotingThresholds::setCommitteeNormal(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_pool_voting_thresholds_set_committee_normal(checked(), checkedNativeHandle(copy)), "PoolVotingThresholds setCommitteeNormal");
}

UnitInterval
PoolVotingThresholds::committeeNoConfidence() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_voting_thresholds_get_committee_no_confidence(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds committeeNoConfidence");
  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
PoolVotingThresholds::setCommitteeNoConfidence(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_pool_voting_thresholds_set_committee_no_confidence(checked(), checkedNativeHandle(copy)), "PoolVotingThresholds setCommitteeNoConfidence");
}

UnitInterval
PoolVotingThresholds::hardForkInitiation() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_voting_thresholds_get_hard_fork_initiation(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds hardForkInitiation");
  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
PoolVotingThresholds::setHardForkInitiation(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_pool_voting_thresholds_set_hard_fork_initiation(checked(), checkedNativeHandle(copy)), "PoolVotingThresholds setHardForkInitiation");
}

UnitInterval
PoolVotingThresholds::securityRelevantParam() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_pool_voting_thresholds_get_security_relevant_param(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "PoolVotingThresholds securityRelevantParam");
  const UnitInterval result = UnitInterval::adopt(owner.release());
  return UnitInterval(result);
}

void
PoolVotingThresholds::setSecurityRelevantParam(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_pool_voting_thresholds_set_security_relevant_param(checked(), checkedNativeHandle(copy)), "PoolVotingThresholds setSecurityRelevantParam");
}

std::vector<std::uint8_t>
PoolVotingThresholds::toCbor() const
{
  return encodeCbor(checked(), cardano_pool_voting_thresholds_to_cbor);
}

std::string
PoolVotingThresholds::toJson() const
{
  return encodeJson(checked(), cardano_pool_voting_thresholds_to_cip116_json);
}

void
PoolVotingThresholds::swap(PoolVotingThresholds& other) noexcept
{
  std::swap(m_value, other.m_value);
}

PoolVotingThresholds&
PoolVotingThresholds::operator=(const PoolVotingThresholds& other)
{
  if (this != &other)
  {
    PoolVotingThresholds copy(other);
    swap(copy);
  }
  return *this;
}

PoolVotingThresholds&
PoolVotingThresholds::operator=(PoolVotingThresholds&& other) noexcept
{
  if (this != &other)
  {
    cardano_pool_voting_thresholds_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
PoolVotingThresholds::operator==(const PoolVotingThresholds& other) const
{
  return toCbor() == other.toCbor();
}

bool
PoolVotingThresholds::operator!=(const PoolVotingThresholds& other) const
{
  return !(*this == other);
}

PoolVotingThresholds
PoolVotingThresholds::adopt(cardano_pool_voting_thresholds_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null PoolVotingThresholds");
  }

  return PoolVotingThresholds(value);
}

cardano_pool_voting_thresholds_t*
PoolVotingThresholds::nativeHandle() noexcept
{
  return m_value;
}

const cardano_pool_voting_thresholds_t*
PoolVotingThresholds::nativeHandle() const noexcept
{
  return m_value;
}

cardano_pool_voting_thresholds_t*
PoolVotingThresholds::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

PoolVotingThresholds::PoolVotingThresholds(cardano_pool_voting_thresholds_t* value) noexcept
: m_value(value)
{
}

cardano_pool_voting_thresholds_t*
PoolVotingThresholds::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from PoolVotingThresholds");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(PoolVotingThresholds& lhs, PoolVotingThresholds& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
