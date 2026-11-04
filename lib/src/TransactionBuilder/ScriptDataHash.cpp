/**
 * @file ScriptDataHash.cpp
 *
 * @brief Transaction script-data commitments.
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

#include <Cometa/TransactionBuilder/ScriptDataHash.h>

#include "../CardanoInterop.h"

#include <cardano/crypto/blake2b_hash.h>
#include <cardano/transaction_builder/script_data_hash.h>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::optional<Blake2bHash>
computeScriptDataHash(const CostModels& costModels, const RedeemerList& redeemers, const PlutusDataSet& datums)
{
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> nativeCostModels(
    decodeCbor<cardano_costmdls_t, cardano_costmdls_unref>(costModels.toCbor(), cardano_costmdls_from_cbor));
  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> nativeRedeemers(
    decodeCbor<cardano_redeemer_list_t, cardano_redeemer_list_unref>(redeemers.toCbor(), cardano_redeemer_list_from_cbor));
  CardanoObject<cardano_plutus_data_set_t, cardano_plutus_data_set_unref> nativeDatums(
    decodeCbor<cardano_plutus_data_set_t, cardano_plutus_data_set_unref>(datums.toCbor(), cardano_plutus_data_set_from_cbor));

  cardano_blake2b_hash_t*                                           result = nullptr;
  const cardano_error_t                                             status = cardano_compute_script_data_hash(nativeCostModels.get(), nativeRedeemers.get(), nativeDatums.get(), &result);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(result);

  checkCardanoError(status, "computeScriptDataHash");

  if (!owner)
  {
    return std::nullopt;
  }

  return Blake2bHash::fromCbor(encodeCbor(owner.get(), cardano_blake2b_hash_to_cbor));
}
} // namespace Cometa
