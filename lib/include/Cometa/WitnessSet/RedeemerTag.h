/**
 * @file RedeemerTag.h
 *
 * @brief Script purposes for redeemers.
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
/** @brief Script purposes for redeemers. */
enum class RedeemerTag
{
  /** @brief Spending a script-locked output. */
  Spend = 0,

  /** @brief Minting or burning an asset. */
  Mint = 1,

  /** @brief Processing a certificate. */
  Certifying = 2,

  /** @brief Withdrawing rewards. */
  Reward = 3,

  /** @brief Casting a governance vote. */
  Voting = 4,

  /** @brief Submitting a governance proposal. */
  Proposing = 5,

  /** @brief Satisfying a script guard. */
  Guarding = 6
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Gets the redeemer purpose name.
 * @param value The purpose to name.
 * @return The descriptive name.
 */
std::string toString(RedeemerTag value);
} // namespace Cometa
