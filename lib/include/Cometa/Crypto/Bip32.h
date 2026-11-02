/**
 * @file Bip32.h
 *
 * @brief BIP32 derivation indices.
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

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Marks a child index as hardened.
 *
 * @param index The child index; an already hardened index is unchanged.
 *
 * @return The index with its most significant bit set.
 */
std::uint32_t hardenIndex(std::uint32_t index) noexcept;
} // namespace Cometa
