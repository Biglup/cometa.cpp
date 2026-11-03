/**
 * @file NativeTransactionEvaluator.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/TransactionBuilder/NativeTransactionEvaluator.h>

#include "../CardanoInterop.h"
#include "TransactionEvaluatorAdapter.h"

#include <cardano/protocol_params/costmdls.h>
#include <cardano/slot_config.h>
#include <cardano/transaction_builder/evaluation/native_tx_evaluator.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator.h>

#include <stdexcept>
#include <utility>

/* IMPLEMENTATION ************************************************************/

namespace Cometa
{
NativeTransactionEvaluator::NativeTransactionEvaluator(
  SlotConfig    slots,
  CostModels    costModels,
  std::uint64_t protocolMajor)
: m_slots(slots)
, m_costModels(std::move(costModels))
, m_protocolMajor(protocolMajor)
, m_subTransactionIndex(0)
, m_isSubTransaction(false)
{
  if (slots.slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be positive");
  }
}

NativeTransactionEvaluator::NativeTransactionEvaluator(
  SlotConfig    slots,
  CostModels    costModels,
  std::uint64_t protocolMajor,
  std::uint64_t subTransactionIndex)
: m_slots(slots)
, m_costModels(std::move(costModels))
, m_protocolMajor(protocolMajor)
, m_subTransactionIndex(subTransactionIndex)
, m_isSubTransaction(true)
{
  if (slots.slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be positive");
  }
}

std::string
NativeTransactionEvaluator::name() const
{
  return "native";
}

RedeemerList
NativeTransactionEvaluator::evaluate(const Transaction& transaction, const UtxoList& additionalUtxos) const
{
  const std::vector<std::uint8_t>                           costModelBytes = m_costModels.toCbor();
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> costModels(
    decodeCbor<cardano_costmdls_t, cardano_costmdls_unref>(costModelBytes, cardano_costmdls_from_cbor));
  const cardano_slot_config_t slots { m_slots.zeroTime, m_slots.zeroSlot, m_slots.slotLength };

  cardano_tx_evaluator_t* value  = nullptr;
  cardano_error_t         status = CARDANO_SUCCESS;

  if (m_isSubTransaction)
  {
    status = cardano_tx_evaluator_new_native_sub_transaction(
      &slots, costModels.get(), m_protocolMajor, m_subTransactionIndex, &value);
  }
  else
  {
    status = cardano_tx_evaluator_new_native(&slots, costModels.get(), m_protocolMajor, &value);
  }

  CardanoObject<cardano_tx_evaluator_t, cardano_tx_evaluator_unref> evaluator(value);
  checkCardanoError(status, "NativeTransactionEvaluator");

  return evaluateWithNative(evaluator.get(), transaction, additionalUtxos);
}
} // namespace Cometa
