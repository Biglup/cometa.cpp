/**
 * @file NativeScriptType.h
 *
 * @brief Declaration of the NativeScriptType enumeration.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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
/** @brief Identifies a native script requirement. */
enum class NativeScriptType
{
  /** @brief Pubkey requirement. */
  RequirePubkey = 0,

  /** @brief All requirement. */
  RequireAllOf = 1,

  /** @brief Any requirement. */
  RequireAnyOf = 2,

  /** @brief NOfK requirement. */
  RequireNOfK = 3,

  /** @brief InvalidBefore requirement. */
  InvalidBefore = 4,

  /** @brief InvalidAfter requirement. */
  InvalidAfter = 5,

  /** @brief RequireGuard requirement. */
  RequireGuard = 6
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Gets the native script type name.
 *
 * @param value The script discriminator.
 *
 * @return The descriptive type name.
 */
std::string toString(NativeScriptType value);
} // namespace Cometa
