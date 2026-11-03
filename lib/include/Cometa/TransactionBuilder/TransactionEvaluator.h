/**
 * @file TransactionEvaluator.h
 *
 * @brief Transaction execution budget interface.
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

#include <Cometa/Common/UtxoList.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/WitnessSet/RedeemerList.h>

#include <string>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Calculates script execution budgets for a transaction. */
class TransactionEvaluator
{
  public:

    /** @brief Destroys the evaluator. */
    virtual ~TransactionEvaluator() = default;

  public:

    /** @brief Gets the evaluator name. @return The name. */
    virtual std::string name() const = 0;

    /**
     * @brief Evaluates script execution budgets.
     * @param transaction The transaction to evaluate.
     * @param additionalUtxos Outputs needed to resolve inputs.
     * @return Redeemers with execution budgets.
     */
    virtual RedeemerList evaluate(const Transaction& transaction, const UtxoList& additionalUtxos) const = 0;
};
} // namespace Cometa
