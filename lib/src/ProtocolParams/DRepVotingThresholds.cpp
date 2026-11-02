/**
 * @file DRepVotingThresholds.cpp
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

#include <Cometa/ProtocolParams/DRepVotingThresholds.h>

#include "../CardanoInterop.h"

#include <cardano/common/unit_interval.h>
#include <cardano/protocol_params/drep_voting_thresholds.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
DRepVotingThresholds::DRepVotingThresholds(
  const UnitInterval& motionNoConfidence,
  const UnitInterval& committeeNormal,
  const UnitInterval& committeeNoConfidence,
  const UnitInterval& updateConstitution,
  const UnitInterval& hardForkInitiation,
  const UnitInterval& ppNetworkGroup,
  const UnitInterval& ppEconomicGroup,
  const UnitInterval& ppTechnicalGroup,
  const UnitInterval& ppGovernanceGroup,
  const UnitInterval& treasuryWithdrawal)
: m_value(nullptr)
{
  const UnitInterval                motionNoConfidenceCopy(motionNoConfidence);
  const UnitInterval                committeeNormalCopy(committeeNormal);
  const UnitInterval                committeeNoConfidenceCopy(committeeNoConfidence);
  const UnitInterval                updateConstitutionCopy(updateConstitution);
  const UnitInterval                hardForkInitiationCopy(hardForkInitiation);
  const UnitInterval                ppNetworkGroupCopy(ppNetworkGroup);
  const UnitInterval                ppEconomicGroupCopy(ppEconomicGroup);
  const UnitInterval                ppTechnicalGroupCopy(ppTechnicalGroup);
  const UnitInterval                ppGovernanceGroupCopy(ppGovernanceGroup);
  const UnitInterval                treasuryWithdrawalCopy(treasuryWithdrawal);
  cardano_drep_voting_thresholds_t* value  = nullptr;
  const cardano_error_t             status = cardano_drep_voting_thresholds_new(
    motionNoConfidenceCopy.checked(),
    committeeNormalCopy.checked(),
    committeeNoConfidenceCopy.checked(),
    updateConstitutionCopy.checked(),
    hardForkInitiationCopy.checked(),
    ppNetworkGroupCopy.checked(),
    ppEconomicGroupCopy.checked(),
    ppTechnicalGroupCopy.checked(),
    ppGovernanceGroupCopy.checked(),
    treasuryWithdrawalCopy.checked(),
    &value);
  CardanoObject<cardano_drep_voting_thresholds_t, cardano_drep_voting_thresholds_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds");
  m_value = owner.release();
}

DRepVotingThresholds::DRepVotingThresholds(const DRepVotingThresholds& other)
: m_value(nullptr)
{
  cardano_drep_voting_thresholds_t*                                                     value  = nullptr;
  const cardano_error_t                                                                 status = cardano_drep_voting_thresholds_deep_clone(other.checked(), &value);
  CardanoObject<cardano_drep_voting_thresholds_t, cardano_drep_voting_thresholds_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds copy");
  m_value = owner.release();
}

DRepVotingThresholds::DRepVotingThresholds(DRepVotingThresholds&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

DRepVotingThresholds::~DRepVotingThresholds()
{
  cardano_drep_voting_thresholds_unref(&m_value);
}

DRepVotingThresholds
DRepVotingThresholds::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return DRepVotingThresholds(decodeCbor<cardano_drep_voting_thresholds_t, cardano_drep_voting_thresholds_unref>(cbor, cardano_drep_voting_thresholds_from_cbor));
}

DRepVotingThresholds
DRepVotingThresholds::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

UnitInterval
DRepVotingThresholds::motionNoConfidence() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_motion_no_confidence(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds motionNoConfidence");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setMotionNoConfidence(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_motion_no_confidence(checked(), copy.checked()), "DRepVotingThresholds setMotionNoConfidence");
}

UnitInterval
DRepVotingThresholds::committeeNormal() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_committee_normal(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds committeeNormal");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setCommitteeNormal(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_committee_normal(checked(), copy.checked()), "DRepVotingThresholds setCommitteeNormal");
}

UnitInterval
DRepVotingThresholds::committeeNoConfidence() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_committee_no_confidence(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds committeeNoConfidence");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setCommitteeNoConfidence(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_committee_no_confidence(checked(), copy.checked()), "DRepVotingThresholds setCommitteeNoConfidence");
}

UnitInterval
DRepVotingThresholds::updateConstitution() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_update_constitution(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds updateConstitution");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setUpdateConstitution(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_update_constitution(checked(), copy.checked()), "DRepVotingThresholds setUpdateConstitution");
}

UnitInterval
DRepVotingThresholds::hardForkInitiation() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_hard_fork_initiation(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds hardForkInitiation");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setHardForkInitiation(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_hard_fork_initiation(checked(), copy.checked()), "DRepVotingThresholds setHardForkInitiation");
}

UnitInterval
DRepVotingThresholds::ppNetworkGroup() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_pp_network_group(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds ppNetworkGroup");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setPpNetworkGroup(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_pp_network_group(checked(), copy.checked()), "DRepVotingThresholds setPpNetworkGroup");
}

UnitInterval
DRepVotingThresholds::ppEconomicGroup() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_pp_economic_group(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds ppEconomicGroup");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setPpEconomicGroup(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_pp_economic_group(checked(), copy.checked()), "DRepVotingThresholds setPpEconomicGroup");
}

UnitInterval
DRepVotingThresholds::ppTechnicalGroup() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_pp_technical_group(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds ppTechnicalGroup");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setPpTechnicalGroup(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_pp_technical_group(checked(), copy.checked()), "DRepVotingThresholds setPpTechnicalGroup");
}

UnitInterval
DRepVotingThresholds::ppGovernanceGroup() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_pp_governance_group(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds ppGovernanceGroup");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setPpGovernanceGroup(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_pp_governance_group(checked(), copy.checked()), "DRepVotingThresholds setPpGovernanceGroup");
}

UnitInterval
DRepVotingThresholds::treasuryWithdrawal() const
{
  cardano_unit_interval_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_drep_voting_thresholds_get_treasury_withdrawal(checked(), &value);
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> owner(value);

  checkCardanoError(status, "DRepVotingThresholds treasuryWithdrawal");
  const UnitInterval result(owner.release());
  return UnitInterval(result);
}

void
DRepVotingThresholds::setTreasuryWithdrawal(const UnitInterval& value)
{
  const UnitInterval copy(value);
  checkCardanoError(cardano_drep_voting_thresholds_set_treasury_withdrawal(checked(), copy.checked()), "DRepVotingThresholds setTreasuryWithdrawal");
}

std::vector<std::uint8_t>
DRepVotingThresholds::toCbor() const
{
  return encodeCbor(checked(), cardano_drep_voting_thresholds_to_cbor);
}

std::string
DRepVotingThresholds::toJson() const
{
  return encodeJson(checked(), cardano_drep_voting_thresholds_to_cip116_json);
}

void
DRepVotingThresholds::swap(DRepVotingThresholds& other) noexcept
{
  std::swap(m_value, other.m_value);
}

DRepVotingThresholds&
DRepVotingThresholds::operator=(const DRepVotingThresholds& other)
{
  if (this != &other)
  {
    DRepVotingThresholds copy(other);
    swap(copy);
  }
  return *this;
}

DRepVotingThresholds&
DRepVotingThresholds::operator=(DRepVotingThresholds&& other) noexcept
{
  if (this != &other)
  {
    cardano_drep_voting_thresholds_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
DRepVotingThresholds::operator==(const DRepVotingThresholds& other) const
{
  return toCbor() == other.toCbor();
}

bool
DRepVotingThresholds::operator!=(const DRepVotingThresholds& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

DRepVotingThresholds::DRepVotingThresholds(cardano_drep_voting_thresholds_t* value) noexcept
: m_value(value)
{
}

cardano_drep_voting_thresholds_t*
DRepVotingThresholds::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from DRepVotingThresholds");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(DRepVotingThresholds& lhs, DRepVotingThresholds& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
