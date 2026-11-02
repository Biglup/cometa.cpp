/**
 * @file Blake2bHashSize.h
 *
 * @brief Common BLAKE2b digest lengths.
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

#include <cstdint>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Common BLAKE2b digest lengths.
 */
enum class Blake2bHashSize : std::uint8_t
{
  /** @brief A 224-bit digest (28 bytes). */
  Bits224 = 28,

  /** @brief A 256-bit digest (32 bytes). */
  Bits256 = 32,

  /** @brief A 512-bit digest (64 bytes). */
  Bits512 = 64
};
} // namespace Cometa
