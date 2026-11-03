/**
 * @file GovernanceActionType.h
 *
 * @brief Governance action discriminators.
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
 * @brief Identifies the ledger change requested by a governance proposal.
 *
 * The action category determines its voting and enactment rules. An informational action collects
 * votes without enacting a ledger change.
 */
enum class GovernanceActionType
{
  /** @brief Changes protocol parameters. */
  ParameterChange = 0,

  /** @brief Initiates a hard fork. */
  HardForkInitiation = 1,

  /** @brief Withdraws treasury funds. */
  TreasuryWithdrawals = 2,

  /** @brief Expresses no confidence in the committee. */
  NoConfidence = 3,

  /** @brief Changes committee membership or quorum. */
  UpdateCommittee = 4,

  /** @brief Replaces the constitution. */
  NewConstitution = 5,

  /** @brief Records information on chain. */
  Info = 6
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Gets the governance action type name.
 * @param value The action discriminator.
 * @return The descriptive type name.
 */
std::string toString(GovernanceActionType value);
} // namespace Cometa
