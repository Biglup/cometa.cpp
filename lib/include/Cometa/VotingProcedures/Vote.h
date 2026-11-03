/**
 * @file Vote.h
 *
 * @brief Voting choices.
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
 * @brief Records support, opposition, or abstention for a governance proposal.
 *
 * The vote is associated with a voter and action through VotingProcedures.
 */
enum class Vote
{
  /** @brief Opposes a governance action. */
  No = 0,

  /** @brief Supports a governance action. */
  Yes = 1,

  /** @brief Abstains from a governance decision. */
  Abstain = 2
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Gets the Vote name.
 * @param value The value to name.
 * @return The descriptive name.
 */
std::string toString(Vote value);
} // namespace Cometa
