/**
 * @file NativeTransactionEvaluator.h
 *
 * @brief Local transaction execution budget evaluator.
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

#include <Cometa/ProtocolParams/CostModels.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/TransactionBuilder/TransactionEvaluator.h>

#include <cstdint>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Evaluates Plutus scripts locally. */
class NativeTransactionEvaluator final : public TransactionEvaluator
{
  public:

    /**
     * @brief Creates a transaction evaluator.
     * @param slots The network slot timeline.
     * @param costModels Plutus cost models.
     * @param protocolMajor The current protocol major version.
     */
    NativeTransactionEvaluator(SlotConfig slots, CostModels costModels, std::uint64_t protocolMajor);

    /**
     * @brief Creates an evaluator for one sub-transaction in a batch.
     * @param slots The network slot timeline.
     * @param costModels Plutus cost models.
     * @param protocolMajor The current protocol major version.
     * @param subTransactionIndex The sub-transaction index in its batch.
     */
    NativeTransactionEvaluator(
      SlotConfig    slots,
      CostModels    costModels,
      std::uint64_t protocolMajor,
      std::uint64_t subTransactionIndex);

  public:

    /** @brief Gets the evaluator name. @return The name. */
    std::string name() const override;

    /**
     * @brief Evaluates script execution budgets locally.
     * @param transaction The transaction to evaluate.
     * @param additionalUtxos Outputs needed to resolve inputs.
     * @return Redeemers with execution budgets.
     */
    RedeemerList evaluate(const Transaction& transaction, const UtxoList& additionalUtxos) const override;

  private:

    /** @brief Slot timeline. */
    SlotConfig m_slots;

    /** @brief Cost models for supported Plutus versions. */
    CostModels m_costModels;

    /** @brief Current protocol major version. */
    std::uint64_t m_protocolMajor;

    /** @brief Index of the sub-transaction, when evaluating a batch. */
    std::uint64_t m_subTransactionIndex;

    /** @brief Whether to evaluate a single sub-transaction. */
    bool m_isSubTransaction;
};
} // namespace Cometa
