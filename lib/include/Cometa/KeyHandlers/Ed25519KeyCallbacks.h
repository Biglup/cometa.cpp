/**
 * @file Ed25519KeyCallbacks.h
 *
 * @brief Signing and public-key operations supplied by a single-key signer.
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

#include <Cometa/Crypto/Ed25519PublicKey.h>
#include <Cometa/Transaction/SubTransaction.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/WitnessSet/VkeyWitnessSet.h>

#include <cstdint>
#include <functional>
#include <vector>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Signing and public-key operations supplied by a single-key signer.
 *
 * Callbacks return witnesses for the supplied transaction or sub-transaction. Returning a witness does
 * not attach it to the transaction or submit it.
 */
struct Ed25519KeyCallbacks
{
    /** @brief Signs a top-level transaction. */
    std::function<VkeyWitnessSet(const Transaction&)> signTransaction;

    /** @brief Signs a sub-transaction. */
    std::function<VkeyWitnessSet(const SubTransaction&)> signSubTransaction;

    /** @brief Gets the handler's public key. */
    std::function<Ed25519PublicKey()> publicKey;

    /**
     * @brief Exports handler-specific state when serialization is supported.
     *
     * This callback is optional. The callback implementation defines the returned format and whether
     * it contains sensitive information; a custom handler's bytes are not automatically encrypted.
     */
    std::function<std::vector<std::uint8_t>()> serialize;
};
} // namespace Cometa
