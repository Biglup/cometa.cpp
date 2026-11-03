/**
 * @file AccountDerivationPath.h
 *
 * @brief The purpose, coin type, and account portion of a wallet derivation path.
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

#include <Cometa/KeyHandlers/Cip1852.h>

#include <cstdint>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief The purpose, coin type, and account portion of a wallet derivation path.
 *
 * Use it to request an extended account public key. DerivationPath adds the role and address index
 * needed to identify an individual wallet key.
 */
struct AccountDerivationPath
{
    /** @brief Wallet purpose before hardening. */
    std::uint32_t purpose = Cip1852::StandardPurpose;

    /** @brief Coin type before hardening. */
    std::uint32_t coinType = Cip1852::CoinType;

    /** @brief Account index before hardening. */
    std::uint32_t account = 0;

    /**
     * @brief Compares account paths.
     * @param other The path to compare.
     * @return True when all segments match.
     */
    bool operator==(const AccountDerivationPath& other) const noexcept;

    /**
     * @brief Compares account paths.
     * @param other The path to compare.
     * @return True when any segment differs.
     */
    bool operator!=(const AccountDerivationPath& other) const noexcept;
};
} // namespace Cometa
