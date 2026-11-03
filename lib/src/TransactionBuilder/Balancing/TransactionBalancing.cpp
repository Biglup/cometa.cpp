/**
 * @file TransactionBalancing.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/TransactionBuilder/Balancing/TransactionBalancing.h>
#include <Cometa/TransactionBuilder/NativeTransactionEvaluator.h>
#include <Cometa/TransactionBuilder/RandomImproveCoinSelector.h>

#include "../../CardanoInterop.h"
#include "../CoinSelectorAdapter.h"
#include "../TransactionEvaluatorAdapter.h"
#include "../TransactionUtilityAccess.h"

#include <cardano/address/address.h>
#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_body/transaction_input.h>
#include <cardano/transaction_body/value.h>
#include <cardano/transaction_builder/balancing/input_to_redeemer_map.h>
#include <cardano/transaction_builder/balancing/transaction_balancing.h>
#include <cardano/transaction_builder/coin_selection/coin_selector.h>
#include <cardano/transaction_builder/evaluation/tx_evaluator.h>
#include <cardano/witness_set/redeemer.h>
#include <cardano/witness_set/redeemer_list.h>
#include <cardano/witness_set/witness_set.h>

#include <stdexcept>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
Value
computeTransactionImbalance(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeResolvedInputs(copyUtilityUtxos(resolvedInputs));

  cardano_value_t*                                    result = nullptr;
  const cardano_error_t                               status = cardano_compute_transaction_imbalance(nativeTransaction.get(), nativeResolvedInputs.get(), TransactionUtilityAccess::parameters(parameters), &result);
  CardanoObject<cardano_value_t, cardano_value_unref> owner(result);

  checkCardanoError(status, "computeTransactionImbalance");

  return TransactionUtilityAccess::copyValue(owner.get());
}

Value
computeSubTransactionImbalance(const SubTransaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> nativeTransaction(
    decodeCbor<cardano_sub_transaction_t, cardano_sub_transaction_unref>(transaction.toCbor(), cardano_sub_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeResolvedInputs(copyUtilityUtxos(resolvedInputs));

  cardano_value_t*                                    result = nullptr;
  const cardano_error_t                               status = cardano_compute_sub_transaction_imbalance(nativeTransaction.get(), nativeResolvedInputs.get(), TransactionUtilityAccess::parameters(parameters), &result);
  CardanoObject<cardano_value_t, cardano_value_unref> owner(result);

  checkCardanoError(status, "computeSubTransactionImbalance");

  return TransactionUtilityAccess::copyValue(owner.get());
}

Value
computeTransactionBatchImbalance(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeResolvedInputs(copyUtilityUtxos(resolvedInputs));

  cardano_value_t*                                    result = nullptr;
  const cardano_error_t                               status = cardano_compute_transaction_batch_imbalance(nativeTransaction.get(), nativeResolvedInputs.get(), TransactionUtilityAccess::parameters(parameters), &result);
  CardanoObject<cardano_value_t, cardano_value_unref> owner(result);

  checkCardanoError(status, "computeTransactionBatchImbalance");

  return TransactionUtilityAccess::copyValue(owner.get());
}

bool
isTransactionBalanced(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeResolvedInputs(copyUtilityUtxos(resolvedInputs));

  bool result {};
  checkCardanoError(cardano_is_transaction_balanced(nativeTransaction.get(), nativeResolvedInputs.get(), TransactionUtilityAccess::parameters(parameters), &result), "isTransactionBalanced");

  return result;
}

Transaction
balanceTransaction(const TransactionBalanceRequest& request)
{
  const WitnessSet                         witnesses = request.transaction.witnessSet();
  const std::optional<VkeyWitnessSet>      keys      = witnesses.vkeys();
  const std::optional<BootstrapWitnessSet> bootstrap = witnesses.bootstrap();

  if ((keys && !keys->empty()) || (bootstrap && !bootstrap->empty()))
  {
    throw std::invalid_argument("Balance an unsigned draft before collecting top-level signatures");
  }

  const TransactionInputSet inputs   = request.transaction.body().inputs();
  UtxoList                  selected = request.preselected;

  for (const TransactionInput& input: inputs)
  {
    bool found = false;

    for (const Utxo& utxo: selected)
    {
      if (utxo.input() == input)
      {
        found = true;
        break;
      }
    }

    if (!found)
    {
      for (const Utxo& utxo: request.available)
      {
        if (utxo.input() == input)
        {
          selected.push_back(utxo);
          found = true;
          break;
        }
      }
    }

    if (!found)
    {
      throw std::invalid_argument("An initial spending input is not resolved by preselected or available UTxOs");
    }
  }

  CardanoObject<cardano_transaction_t, cardano_transaction_unref> transaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(request.transaction.toCbor(), cardano_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> preselected(copyUtilityUtxos(selected));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> available(copyUtilityUtxos(request.available));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> references(copyUtilityUtxos(request.referenceInputs));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> collateral(copyUtilityUtxos(request.collateral));
  CardanoObject<cardano_address_t, cardano_address_unref>     change(copyUtilityAddress(request.changeAddress));
  CardanoObject<cardano_address_t, cardano_address_unref>     collateralChange(
    copyUtilityAddress(request.collateralChangeAddress ? *request.collateralChangeAddress : request.changeAddress));

  cardano_input_to_redeemer_map_t*                                                    mapValue  = nullptr;
  const cardano_error_t                                                               mapStatus = cardano_input_to_redeemer_map_new(&mapValue);
  CardanoObject<cardano_input_to_redeemer_map_t, cardano_input_to_redeemer_map_unref> inputRedeemers(mapValue);

  checkCardanoError(mapStatus, "Input redeemer associations");

  const std::optional<RedeemerList> redeemers = witnesses.redeemers();

  if (redeemers)
  {
    CardanoObject<cardano_witness_set_t, cardano_witness_set_unref>     nativeWitnesses(cardano_transaction_get_witness_set(transaction.get()));
    CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> nativeRedeemers(cardano_witness_set_get_redeemers(nativeWitnesses.get()));

    for (std::size_t index = 0; index < redeemers->size(); ++index)
    {
      const Redeemer redeemer = redeemers->at(index);

      if (redeemer.tag() != RedeemerTag::Spend)
      {
        continue;
      }

      if (redeemer.index() >= inputs.size())
      {
        throw std::invalid_argument("A spending redeemer refers to an input outside the draft");
      }

      const TransactionInput                                                      input = inputs.at(static_cast<std::size_t>(redeemer.index()));
      CardanoObject<cardano_transaction_input_t, cardano_transaction_input_unref> nativeInput(
        decodeCbor<cardano_transaction_input_t, cardano_transaction_input_unref>(input.toCbor(), cardano_transaction_input_from_cbor));

      cardano_redeemer_t*                                       value  = nullptr;
      const cardano_error_t                                     status = cardano_redeemer_list_get(nativeRedeemers.get(), index, &value);
      CardanoObject<cardano_redeemer_t, cardano_redeemer_unref> nativeRedeemer(value);

      checkCardanoError(status, "Spending redeemer lookup");

      checkCardanoError(cardano_input_to_redeemer_map_insert(inputRedeemers.get(), nativeInput.get(), nativeRedeemer.get()), "Input redeemer insertion");
    }
  }

  std::shared_ptr<CoinSelector>         selector  = request.coinSelector;
  std::shared_ptr<TransactionEvaluator> evaluator = request.evaluator;

  if (!selector)
  {
    selector = std::make_shared<RandomImproveCoinSelector>();
  }

  if (!evaluator)
  {
    evaluator = std::make_shared<NativeTransactionEvaluator>(
      request.slots, request.parameters.costModels(), request.parameters.protocolVersion().major());
  }

  CardanoObject<cardano_coin_selector_t, cardano_coin_selector_unref> nativeSelector(createCoinSelectorAdapter(selector));
  CardanoObject<cardano_tx_evaluator_t, cardano_tx_evaluator_unref>   nativeEvaluator(createTransactionEvaluatorAdapter(evaluator));

  cardano_transaction_clear_cbor_cache(transaction.get());

  const cardano_error_t status = cardano_balance_transaction(
    transaction.get(), request.foreignSignatureCount, TransactionUtilityAccess::parameters(request.parameters), references.get(), preselected.get(), inputRedeemers.get(), available.get(), nativeSelector.get(), change.get(), collateral.get(), collateralChange.get(), nativeEvaluator.get(), nullptr);

  checkCardanoError(status, "balanceTransaction");
  cardano_transaction_clear_cbor_cache(transaction.get());

  return Transaction::fromCbor(encodeCbor(transaction.get(), cardano_transaction_to_cbor));
}

} // namespace Cometa
