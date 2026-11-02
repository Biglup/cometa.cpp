/**
 * @file NetworkMagic.h
 *
 * @brief Network magic for a named Cardano network.
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

#include <string>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Network magic for a named Cardano network.
 */
enum class NetworkMagic : std::uint32_t
{
  /** @brief Pre-production network. */
  Preprod = 1,

  /** @brief Preview network. */
  Preview = 2,

  /** @brief SanchoNet network. */
  SanchoNet = 4,

  /** @brief Main network. */
  Mainnet = 764824073
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Gets the network name.
 *
 * @param network The network magic.
 *
 * @return The lowercase network name, or unknown for an unrecognized value.
 */
std::string toString(NetworkMagic network);
} // namespace Cometa
