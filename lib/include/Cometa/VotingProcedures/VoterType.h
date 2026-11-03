/**
 * @file VoterType.h
 *
 * @brief Governance voter discriminators.
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
 * @brief Identifies the governance role and authorization kind of a voter.
 *
 * Committee voters use hot credentials; DReps use their registered credentials; pool voters use
 * operator keys.
 */
enum class VoterType
{
  /** @brief Committee member identified by a key hash. */
  ConstitutionalCommitteeKeyHash = 0,

  /** @brief Committee member identified by a script hash. */
  ConstitutionalCommitteeScriptHash = 1,

  /** @brief Delegated representative identified by a key hash. */
  DRepKeyHash = 2,

  /** @brief Delegated representative identified by a script hash. */
  DRepScriptHash = 3,

  /** @brief Stake pool identified by a key hash. */
  StakePoolKeyHash = 4
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Gets the VoterType name.
 * @param value The value to name.
 * @return The descriptive name.
 */
std::string toString(VoterType value);
} // namespace Cometa
