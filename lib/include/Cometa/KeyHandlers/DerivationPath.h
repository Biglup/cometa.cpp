/**
 * @file DerivationPath.h
 *
 * @brief BIP32 path to a Cardano key.
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

#include <Cometa/KeyHandlers/AccountDerivationPath.h>
#include <Cometa/KeyHandlers/DerivationRole.h>

#include <cstdint>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief The five segments of a Cardano wallet key path. */
struct DerivationPath
{
    /** @brief Wallet purpose before hardening. */
    std::uint32_t purpose = Cip1852::StandardPurpose;

    /** @brief Coin type before hardening. */
    std::uint32_t coinType = Cip1852::CoinType;

    /** @brief Account index before hardening. */
    std::uint32_t account = 0;

    /** @brief Key role. */
    DerivationRole role = DerivationRole::External;

    /** @brief Key index. */
    std::uint32_t index = 0;

    /**
     * @brief Gets the account portion of this path.
     * @return The first three segments.
     */
    AccountDerivationPath accountPath() const noexcept;

    /**
     * @brief Compares key paths.
     * @param other The path to compare.
     * @return True when all segments match.
     */
    bool operator==(const DerivationPath& other) const noexcept;

    /**
     * @brief Compares key paths.
     * @param other The path to compare.
     * @return True when any segment differs.
     */
    bool operator!=(const DerivationPath& other) const noexcept;
};
} // namespace Cometa
