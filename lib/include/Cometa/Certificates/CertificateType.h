/**
 * @file CertificateType.h
 *
 * @brief Certificate discriminators.
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

#include <string>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/** @brief Identifies the kind of ledger certificate. */
enum class CertificateType
{
  /** @brief StakeRegistration certificate. */
  StakeRegistration = 0,

  /** @brief StakeDeregistration certificate. */
  StakeDeregistration = 1,

  /** @brief StakeDelegation certificate. */
  StakeDelegation = 2,

  /** @brief PoolRegistration certificate. */
  PoolRegistration = 3,

  /** @brief PoolRetirement certificate. */
  PoolRetirement = 4,

  /** @brief GenesisKeyDelegation certificate. */
  GenesisKeyDelegation = 5,

  /** @brief MoveInstantaneousRewards certificate. */
  MoveInstantaneousRewards = 6,

  /** @brief Registration certificate. */
  Registration = 7,

  /** @brief Unregistration certificate. */
  Unregistration = 8,

  /** @brief VoteDelegation certificate. */
  VoteDelegation = 9,

  /** @brief StakeVoteDelegation certificate. */
  StakeVoteDelegation = 10,

  /** @brief StakeRegistrationDelegation certificate. */
  StakeRegistrationDelegation = 11,

  /** @brief VoteRegistrationDelegation certificate. */
  VoteRegistrationDelegation = 12,

  /** @brief StakeVoteRegistrationDelegation certificate. */
  StakeVoteRegistrationDelegation = 13,

  /** @brief AuthorizeCommitteeHot certificate. */
  AuthorizeCommitteeHot = 14,

  /** @brief ResignCommitteeCold certificate. */
  ResignCommitteeCold = 15,

  /** @brief DRepRegistration certificate. */
  DRepRegistration = 16,

  /** @brief DRepUnregistration certificate. */
  DRepUnregistration = 17,

  /** @brief UpdateDRep certificate. */
  UpdateDRep = 18
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Gets the certificate type name.
 *
 * @param value The certificate discriminator.
 *
 * @return The descriptive type name.
 */
std::string toString(CertificateType value);
} // namespace Cometa
