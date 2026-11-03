/**
 * @file NetworkExample.h
 *
 * @brief Shared setup for the preprod transaction examples.
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

#include <Cometa/Address/Address.h>
#include <Cometa/KeyHandlers/SecureKeyHandler.h>
#include <Cometa/Providers/BlockfrostProvider.h>
#include <Cometa/Transaction/Transaction.h>

#include <cstdint>

/* PROTOTYPES ****************************************************************/

namespace CometaExamples
{
/** @brief Restores the Cardano C example signer. @return The signer. */
Cometa::SecureKeyHandler sampleSigner();

/** @brief Derives the Cardano C example payment address. @param signer The signer. @return The address. */
Cometa::Address paymentAddress(const Cometa::SecureKeyHandler& signer);

/** @brief Connects to preprod using BLOCKFROST_API_KEY. @return The provider. */
Cometa::BlockfrostProvider preprodProvider();

/** @brief Gets a preprod slot two hours from now. @return The slot. */
std::uint64_t expirySlot();

/** @brief Signs, submits, and awaits confirmation. @param signer The signer. @param provider The provider. @param transaction The transaction to complete. */
void signAndSubmit(
  const Cometa::SecureKeyHandler&   signer,
  const Cometa::BlockfrostProvider& provider,
  Cometa::Transaction&              transaction);
} // namespace CometaExamples
