/**
 * @file SelectionStrategy.h
 *
 * @brief Random-improve selection target.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

/* ENUM DECLARATION **********************************************************/

namespace Cometa
{
/** @brief Target amount selected by random-improve. */
enum class SelectionStrategy
{
  /** @brief Selects just enough to cover the target. */
  Minimal,

  /** @brief Prefers approximately twice the target. */
  Optimal
};
} // namespace Cometa
