/**
 * @file ScriptLanguage.h
 *
 * @brief Identifies a script language.
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
/** @brief Identifies a native or Plutus script language. */
enum class ScriptLanguage
{
  /** @brief Native scripts. */
  Native = 0,

  /** @brief PlutusV1 scripts. */
  PlutusV1 = 1,

  /** @brief PlutusV2 scripts. */
  PlutusV2 = 2,

  /** @brief PlutusV3 scripts. */
  PlutusV3 = 3,

  /** @brief PlutusV4 scripts. */
  PlutusV4 = 4
};
} // namespace Cometa
