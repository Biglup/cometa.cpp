/**
 * @file Cip1852.h
 *
 * @brief Standard Cardano derivation path values.
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

/* INCLUDES ******************************************************************/

#include <cstdint>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/** @brief Common CIP-1852 derivation values. */
namespace Cip1852
{
/** @brief Standard wallet purpose. */
constexpr std::uint32_t StandardPurpose = 1852;

/** @brief Multisignature wallet purpose. */
constexpr std::uint32_t MultisigPurpose = 1854;

/** @brief Cardano coin type. */
constexpr std::uint32_t CoinType = 1815;
} // namespace Cip1852
} // namespace Cometa
