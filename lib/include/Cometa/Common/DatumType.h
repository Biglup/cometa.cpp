/**
 * @file DatumType.h
 *
 * @brief Declaration of the DatumType enumeration.
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

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Distinguishes an output datum hash from inline validator data.
 *
 * A hash requires the data to be obtained separately when needed; inline data is carried by the
 * output.
 */
enum class DatumType
{
  /** @brief A 32-byte datum hash. */
  DataHash = 0,

  /** @brief Inline Plutus data. */
  InlineData = 1
};
} // namespace Cometa
