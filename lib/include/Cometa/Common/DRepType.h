/**
 * @file DRepType.h
 *
 * @brief Delegated representative kinds.
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

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/** @brief Identifies a delegated representative choice. */
enum class DRepType
{
  /** @brief A key hash represents the DRep. */
  KeyHash = 0,

  /** @brief A script hash represents the DRep. */
  ScriptHash = 1,

  /** @brief The voter abstains. */
  Abstain = 2,

  /** @brief The voter expresses no confidence. */
  NoConfidence = 3
};
} // namespace Cometa
