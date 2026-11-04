/**
 * @file ImplicitCoin.cpp
 *
 * @brief Implicit transaction lovelace accounting.
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

#include <Cometa/TransactionBuilder/Balancing/ImplicitCoin.h>

#include "../../CardanoInterop.h"

#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_builder/balancing/implicit_coin.h>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
ImplicitCoin
computeImplicitCoin(const Transaction& transaction, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));

  cardano_implicit_coin_t result {};
  checkCardanoError(cardano_compute_implicit_coin(nativeTransaction.get(), checkedNativeHandle(parameters), &result), "computeImplicitCoin");

  return ImplicitCoin { result.withdrawals, result.deposits, result.reclaim_deposits };
}

ImplicitCoin
computeImplicitCoin(const SubTransaction& transaction, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> nativeTransaction(
    decodeCbor<cardano_sub_transaction_t, cardano_sub_transaction_unref>(transaction.toCbor(), cardano_sub_transaction_from_cbor));

  cardano_implicit_coin_t result {};
  checkCardanoError(cardano_compute_sub_transaction_implicit_coin(nativeTransaction.get(), checkedNativeHandle(parameters), &result), "computeImplicitCoin");

  return ImplicitCoin { result.withdrawals, result.deposits, result.reclaim_deposits };
}
} // namespace Cometa
