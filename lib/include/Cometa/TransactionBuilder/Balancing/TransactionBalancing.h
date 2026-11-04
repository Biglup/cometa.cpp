/**
 * @file TransactionBalancing.h
 *
 * @brief Transaction value conservation and standalone balancing.
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

#include <Cometa/TransactionBuilder/Balancing/TransactionBalanceRequest.h>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/**
 * @brief Computes consumed minus produced value for the top-level transaction body.
 *
 * This is value accounting, not script or full ledger validation.
 * @param transaction The transaction to inspect.
 * @param resolvedInputs The resolved spending inputs.
 * @param parameters The current protocol parameters for the target network.
 * @return Positive quantities are surplus; negative quantities are a deficit.
 * @throws Error The operation cannot be completed with the supplied values.
 */
Value computeTransactionImbalance(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

/**
 * @brief Computes consumed minus produced value for a sub-transaction.
 *
 * This is value accounting, not script or full ledger validation.
 * @param transaction The transaction to inspect.
 * @param resolvedInputs The resolved spending inputs.
 * @param parameters The current protocol parameters for the target network.
 * @return Positive quantities are surplus; negative quantities are a deficit.
 * @throws Error The operation cannot be completed with the supplied values.
 */
Value computeSubTransactionImbalance(const SubTransaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

/**
 * @brief Computes the combined value imbalance of a transaction and its sub-transactions.
 *
 * This is value accounting, not script or full ledger validation.
 * @param transaction The transaction to inspect.
 * @param resolvedInputs The resolved spending inputs, including batch inputs when computing batch imbalance.
 * @param parameters The current protocol parameters for the target network.
 * @return Positive quantities are surplus; negative quantities are a deficit.
 * @throws Error The operation cannot be completed with the supplied values.
 */
Value computeTransactionBatchImbalance(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

/**
 * @brief Checks whether the transaction and its sub-transactions have zero combined imbalance.
 *
 * This does not check fee adequacy, signatures, or script validity.
 * @param transaction The transaction to inspect.
 * @param resolvedInputs The resolved spending inputs of the transaction and any sub-transactions.
 * @param parameters The current protocol parameters for the target network.
 * @return True when consumed and produced values match.
 * @throws Error The operation cannot be completed with the supplied values.
 */
bool isTransactionBalanced(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

/**
 * @brief Balances a copy of an unsigned transaction.
 *
 * Existing spending redeemers are associated with their original inputs and reindexed after selection.
 * The request and its transaction remain unchanged on success or failure.
 * Use TransactionBuilder for deferred redeemer producers.
 * @param request The draft, resolved UTxOs, network context, and optional services.
 * @return A balanced transaction ready to sign.
 * @throws Error The operation cannot be completed with the supplied values.
 * @throws std::invalid_argument The draft contains key signatures, an unresolved initial input, or an invalid spending redeemer index.
 */
Transaction balanceTransaction(const TransactionBalanceRequest& request);
} // namespace Cometa
