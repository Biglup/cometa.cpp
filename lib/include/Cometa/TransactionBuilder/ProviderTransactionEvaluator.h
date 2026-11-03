/**
 * @file ProviderTransactionEvaluator.h
 *
 * @brief Estimates Plutus execution budgets through a Provider.
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

#include <Cometa/Providers/Provider.h>
#include <Cometa/TransactionBuilder/TransactionEvaluator.h>

#include <memory>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Estimates Plutus execution budgets through a Provider.
 *
 * Use it when evaluation should be delegated to the connected service. The provider must support the
 * transaction's script versions and protocol features.
 */
class ProviderTransactionEvaluator final : public TransactionEvaluator
{
  public:

    /**
     * @brief Creates a provider-backed evaluator.
     * @param provider The provider to use.
     */
    explicit ProviderTransactionEvaluator(std::shared_ptr<Provider> provider);

  public:

    /**
     * @brief Gets the evaluator name.
     * @return The name.
     */
    std::string name() const override;

    /**
     * @brief Evaluates script execution budgets through the provider.
     *
     * Return budgets keyed by each redeemer's original purpose and index. A successful evaluation does not
     * verify every ledger rule or guarantee that submission will succeed.
     *
     * @param transaction The transaction to evaluate.
     * @param additionalUtxos Outputs needed to resolve inputs.
     * @return Redeemers with execution budgets.
     */
    RedeemerList evaluate(const Transaction& transaction, const UtxoList& additionalUtxos) const override;

  private:

    /** @brief Provider retained for evaluation calls. */
    std::shared_ptr<Provider> m_provider;
};
} // namespace Cometa
