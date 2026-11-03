/**
 * @file ByronAddressType.h
 *
 * @brief ByronAddressType value.
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
 * @brief Identifies the spending mechanism of a Byron address.
 *
 * These variants describe legacy bootstrap addresses rather than Shelley credential types.
 */
enum class ByronAddressType : std::uint8_t
{
  /** @brief Public-key spending. */
  PublicKey = 0,

  /** @brief Script spending. */
  Script = 1,

  /** @brief Genesis redemption. */
  Redeem = 2
};
} // namespace Cometa
