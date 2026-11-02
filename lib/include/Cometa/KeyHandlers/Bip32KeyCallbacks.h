/**
 * @file Bip32KeyCallbacks.h
 *
 * @brief C++ operations supplied by a BIP32 key handler.
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

#include <Cometa/Crypto/Bip32PublicKey.h>
#include <Cometa/KeyHandlers/AccountDerivationPath.h>
#include <Cometa/KeyHandlers/DerivationPath.h>
#include <Cometa/Transaction/SubTransaction.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/WitnessSet/VkeyWitnessSet.h>

#include <cstdint>
#include <functional>
#include <vector>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Operations delegated to a BIP32 signer. */
struct Bip32KeyCallbacks
{
    /** @brief Signs a top-level transaction for the requested key paths. */
    std::function<VkeyWitnessSet(const Transaction&, const std::vector<DerivationPath>&)> signTransaction;

    /** @brief Signs a sub-transaction for the requested key paths. */
    std::function<VkeyWitnessSet(const SubTransaction&, const std::vector<DerivationPath>&)> signSubTransaction;

    /** @brief Gets the extended public key at an account path. */
    std::function<Bip32PublicKey(const AccountDerivationPath&)> accountPublicKey;

    /** @brief Serializes the handler state, when supported. */
    std::function<std::vector<std::uint8_t>()> serialize;
};
} // namespace Cometa
