/**
 * @file DeferredRedeemerAdapter.h
 *
 * @brief Native callback bridge for deferred redeemers.
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

#include <Cometa/TransactionBuilder/DeferredRedeemer.h>

#include <cardano/error.h>

#include <exception>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_transaction_t;
struct cardano_utxo_list_t;
struct cardano_plutus_data_t;

/* STRUCTURE DECLARATION *****************************************************/

namespace Cometa
{
/** @brief Retains a user callback and its last exception for one build. */
struct DeferredRedeemerContext
{
    /** @brief User-supplied deterministic function. */
    DeferredRedeemer callback;

    /** @brief Exception captured from the last invocation, if any. */
    std::exception_ptr error;
};

/**
 * @brief Calls a C++ deferred redeemer from the native balancing loop.
 * @param context The retained callback context.
 * @param draft The balanced draft transaction.
 * @param resolvedInputs The resolved inputs.
 * @param redeemer Receives the owned redeemer payload.
 * @return The native status.
 */
cardano_error_t invokeDeferredRedeemer(
  void*                   context,
  cardano_transaction_t*  draft,
  cardano_utxo_list_t*    resolvedInputs,
  cardano_plutus_data_t** redeemer) noexcept;
} // namespace Cometa
