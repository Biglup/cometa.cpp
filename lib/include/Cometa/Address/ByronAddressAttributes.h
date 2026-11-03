/**
 * @file ByronAddressAttributes.h
 *
 * @brief Optional derivation information and network magic carried by a Byron address.
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
#include <optional>
#include <vector>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Optional derivation information and network magic carried by a Byron address.
 *
 * The derivation path payload is encoded address data, not a sequence of wallet key indices. Network
 * magic distinguishes Byron networks when present.
 */
struct ByronAddressAttributes
{
    /** @brief Encrypted derivation path, at most 64 bytes. */
    std::vector<std::uint8_t> derivationPath;

    /** @brief Nonnegative network magic; absent for mainnet. */
    std::optional<std::int64_t> magic;
};
} // namespace Cometa
