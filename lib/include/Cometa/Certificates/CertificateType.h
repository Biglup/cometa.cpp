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
/**
 * @brief Identifies the ledger action carried by a Certificate.
 *
 * Use it to choose the corresponding typed accessor. Certificate forms have different authorization,
 * deposit, and era requirements.
 */
enum class CertificateType
{
  /** @brief Registers a staking credential using the legacy form. */
  StakeRegistration = 0,

  /** @brief Deregisters a staking credential using the legacy form. */
  StakeDeregistration = 1,

  /** @brief Delegates stake to a pool. */
  StakeDelegation = 2,

  /** @brief Registers a pool or updates its parameters. */
  PoolRegistration = 3,

  /** @brief Schedules a pool retirement. */
  PoolRetirement = 4,

  /** @brief Changes a historical genesis delegation. */
  GenesisKeyDelegation = 5,

  /** @brief Allocates historical instantaneous rewards. */
  MoveInstantaneousRewards = 6,

  /** @brief Registers a staking credential with a deposit. */
  Registration = 7,

  /** @brief Deregisters a staking credential with a refund. */
  Unregistration = 8,

  /** @brief Assigns a governance voting delegate. */
  VoteDelegation = 9,

  /** @brief Assigns pool and voting delegates. */
  StakeVoteDelegation = 10,

  /** @brief Registers a credential and assigns its pool. */
  StakeRegistrationDelegation = 11,

  /** @brief Registers a credential and assigns its voting delegate. */
  VoteRegistrationDelegation = 12,

  /** @brief Registers a credential and assigns both delegates. */
  StakeVoteRegistrationDelegation = 13,

  /** @brief Authorizes a committee member's voting credential. */
  AuthorizeCommitteeHot = 14,

  /** @brief Resigns from the constitutional committee. */
  ResignCommitteeCold = 15,

  /** @brief Registers a DRep with a deposit. */
  DRepRegistration = 16,

  /** @brief Deregisters a DRep with a refund. */
  DRepUnregistration = 17,

  /** @brief Updates a DRep metadata anchor. */
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
