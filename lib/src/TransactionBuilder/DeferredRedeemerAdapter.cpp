/**
 * @file DeferredRedeemerAdapter.cpp
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

/* INCLUDES ******************************************************************/

#include "DeferredRedeemerAdapter.h"

#include "../CardanoInterop.h"

#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/transaction/transaction.h>

#include <new>

/* IMPLEMENTATION ************************************************************/

namespace Cometa
{
cardano_error_t
invokeDeferredRedeemer(
  void*                   context,
  cardano_transaction_t*  draft,
  cardano_utxo_list_t*    resolvedInputs,
  cardano_plutus_data_t** redeemer) noexcept
{
  DeferredRedeemerContext* callbackContext = static_cast<DeferredRedeemerContext*>(context);

  try
  {
    cardano_transaction_ref(draft);
    CardanoObject<cardano_transaction_t, cardano_transaction_unref> draftOwner(draft);

    cardano_utxo_list_ref(resolvedInputs);
    CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> inputOwner(resolvedInputs);

    const Transaction transaction = Transaction::fromCbor(encodeCbor(draftOwner.get(), cardano_transaction_to_cbor));
    UtxoList          inputs;
    const std::size_t count = cardano_utxo_list_get_length(inputOwner.get());
    inputs.reserve(count);

    for (std::size_t index = 0; index < count; ++index)
    {
      cardano_utxo_t*                                   item   = nullptr;
      const cardano_error_t                             status = cardano_utxo_list_get(inputOwner.get(), index, &item);
      CardanoObject<cardano_utxo_t, cardano_utxo_unref> owner(item);
      checkCardanoError(status, "DeferredRedeemer UtxoList get");
      inputs.push_back(Utxo::fromCbor(encodeCbor(owner.get(), cardano_utxo_to_cbor)));
    }

    const PlutusData result = callbackContext->callback(transaction, inputs);
    *redeemer               = decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(
      result.toCbor(), cardano_plutus_data_from_cbor);
    return CARDANO_SUCCESS;
  }
  catch (const std::bad_alloc&)
  {
    callbackContext->error = std::current_exception();
    return CARDANO_ERROR_MEMORY_ALLOCATION_FAILED;
  }
  catch (...)
  {
    callbackContext->error = std::current_exception();
    return CARDANO_ERROR_GENERIC;
  }
}
} // namespace Cometa
