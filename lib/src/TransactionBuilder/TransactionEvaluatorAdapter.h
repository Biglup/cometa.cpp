/**
 * @file TransactionEvaluatorAdapter.h
 *
 * @brief Native callback bridge for C++ transaction evaluators.
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

#include <Cometa/TransactionBuilder/TransactionEvaluator.h>

#include <memory>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_tx_evaluator_t;

/* FUNCTION DECLARATIONS *****************************************************/

namespace Cometa
{
/**
 * @brief Creates a native evaluator that forwards to a C++ implementation.
 * @param evaluator The implementation to retain.
 * @return An owned native evaluator.
 */
cardano_tx_evaluator_t* createTransactionEvaluatorAdapter(std::shared_ptr<TransactionEvaluator> evaluator);

/**
 * @brief Runs a native evaluator with C++ values.
 * @param evaluator The borrowed native evaluator.
 * @param transaction The transaction to evaluate.
 * @param additionalUtxos Outputs needed to resolve inputs.
 * @return Redeemers with execution budgets.
 */
RedeemerList evaluateWithNative(
  cardano_tx_evaluator_t* evaluator,
  const Transaction&      transaction,
  const UtxoList&         additionalUtxos);
} // namespace Cometa
