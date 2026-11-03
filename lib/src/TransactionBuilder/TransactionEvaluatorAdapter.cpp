/**
 * @file TransactionEvaluatorAdapter.cpp
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

/* INCLUDES ******************************************************************/

#include "TransactionEvaluatorAdapter.h"

#include "../CardanoInterop.h"

#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/object.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator_impl.h>
#include <cardano/witness_set/redeemer_list.h>

#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/** @brief Keeps a C++ evaluator alive while native calls can reach it. */
struct EvaluatorContext : cardano_object_t
{
    /** @brief The retained evaluator. */
    std::shared_ptr<TransactionEvaluator> evaluator;

    /** @brief Stores the evaluator. @param value The evaluator to retain. */
    explicit EvaluatorContext(std::shared_ptr<TransactionEvaluator> value)
    : cardano_object_t { 1, &EvaluatorContext::deallocate, {} }
    , evaluator(std::move(value))
    {
    }

    /** @brief Destroys the context. @param value The context to release. */
    static void
    deallocate(void* value)
    {
      delete static_cast<EvaluatorContext*>(static_cast<cardano_object_t*>(value));
    }
};

/**
 * @brief Copies a native UTxO list to C++ values.
 * @param value The list to copy, or null.
 * @return Independent UTxO values.
 */
UtxoList
copyUtxos(const cardano_utxo_list_t* value)
{
  UtxoList outputs;
  if (!value)
  {
    return outputs;
  }

  const std::size_t count = cardano_utxo_list_get_length(value);
  outputs.reserve(count);

  for (std::size_t index = 0; index < count; ++index)
  {
    cardano_utxo_t*                                   item   = nullptr;
    const cardano_error_t                             status = cardano_utxo_list_get(value, index, &item);
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> owner(item);
    checkCardanoError(status, "TransactionEvaluator UtxoList get");

    outputs.push_back(Utxo::fromCbor(encodeCbor(owner.get(), cardano_utxo_to_cbor)));
  }

  return outputs;
}

/**
 * @brief Copies C++ UTxOs into a native list.
 * @param values The values to copy.
 * @return An owned native list.
 */
cardano_utxo_list_t*
copyUtxos(const UtxoList& values)
{
  cardano_utxo_list_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_utxo_list_new(&value);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> owner(value);
  checkCardanoError(status, "TransactionEvaluator UtxoList");

  for (const Utxo& item: values)
  {
    const std::vector<std::uint8_t>                   bytes = item.toCbor();
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
      decodeCbor<cardano_utxo_t, cardano_utxo_unref>(bytes, cardano_utxo_from_cbor));

    checkCardanoError(cardano_utxo_list_add(owner.get(), native.get()), "TransactionEvaluator UtxoList add");
  }

  return owner.release();
}

/**
 * @brief Dispatches the native evaluator callback.
 * @param implementation The native implementation.
 * @param transaction The transaction to evaluate.
 * @param additionalUtxos Additional resolved outputs.
 * @param redeemers Receives the evaluated redeemers.
 * @return The native status.
 */
cardano_error_t
evaluate(
  cardano_tx_evaluator_impl_t* implementation,
  cardano_transaction_t*       transaction,
  cardano_utxo_list_t*         additionalUtxos,
  cardano_redeemer_list_t**    redeemers) noexcept
{
  try
  {
    const EvaluatorContext* context        = static_cast<const EvaluatorContext*>(implementation->context);
    const Transaction       cppTransaction = Transaction::fromCbor(encodeCbor(transaction, cardano_transaction_to_cbor));
    const RedeemerList      result         = context->evaluator->evaluate(cppTransaction, copyUtxos(additionalUtxos));

    *redeemers = decodeCbor<cardano_redeemer_list_t, cardano_redeemer_list_unref>(
      result.toCbor(), cardano_redeemer_list_from_cbor);
    return CARDANO_SUCCESS;
  }
  catch (const std::bad_alloc&)
  {
    return CARDANO_ERROR_MEMORY_ALLOCATION_FAILED;
  }
  catch (const std::exception& error)
  {
    std::snprintf(implementation->error_message, sizeof(implementation->error_message), "%s", error.what());
    return CARDANO_ERROR_GENERIC;
  }
  catch (...)
  {
    std::snprintf(implementation->error_message, sizeof(implementation->error_message), "%s", "Transaction evaluation failed");
    return CARDANO_ERROR_GENERIC;
  }
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

cardano_tx_evaluator_t*
createTransactionEvaluatorAdapter(std::shared_ptr<TransactionEvaluator> evaluator)
{
  if (!evaluator)
  {
    throw std::invalid_argument("TransactionEvaluator is required");
  }

  const std::string name = evaluator->name();
  if (name.empty() || name.size() >= 256 || name.find('\0') != std::string::npos)
  {
    throw std::invalid_argument("TransactionEvaluator name must contain 1 to 255 characters without NUL");
  }

  EvaluatorContext*           context = new EvaluatorContext(std::move(evaluator));
  cardano_tx_evaluator_impl_t implementation {};

  std::memcpy(implementation.name, name.data(), name.size());
  implementation.context  = context;
  implementation.evaluate = &evaluate;

  cardano_tx_evaluator_t* value  = nullptr;
  const cardano_error_t   status = cardano_tx_evaluator_new(implementation, &value);
  if (status != CARDANO_SUCCESS)
  {
    delete context;
    checkCardanoError(status, "TransactionEvaluator adapter");
  }

  return value;
}

RedeemerList
evaluateWithNative(cardano_tx_evaluator_t* evaluator, const Transaction& transaction, const UtxoList& additionalUtxos)
{
  if (!evaluator)
  {
    throw std::invalid_argument("Native TransactionEvaluator is required");
  }

  const std::vector<std::uint8_t>                                 transactionBytes = transaction.toCbor();
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transactionBytes, cardano_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeUtxos(copyUtxos(additionalUtxos));

  cardano_redeemer_list_t* resultValue = nullptr;
  const cardano_error_t    status      = cardano_tx_evaluator_evaluate(
    evaluator, nativeTransaction.get(), nativeUtxos.get(), &resultValue);
  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> result(resultValue);
  checkCardanoError(status, "TransactionEvaluator evaluate");

  return RedeemerList::fromCbor(encodeCbor(result.get(), cardano_redeemer_list_to_cbor));
}
} // namespace Cometa
