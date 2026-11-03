/**
 * @file NetworkId.h
 *
 * @brief Network identifier encoded in a Shelley address.
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
 * @brief Identifies mainnet or a test network in Shelley addresses.
 *
 * Preview and preprod share the testnet identifier. Use NetworkMagic when selecting a particular
 * network service or slot configuration.
 */
enum class NetworkId : std::uint8_t
{
  /** @brief A test network. */
  Testnet = 0,

  /** @brief The main network. */
  Mainnet = 1
};
} // namespace Cometa
