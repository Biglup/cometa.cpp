/**
 * @file TransactionConversions.cpp
 *
 * @brief Private transaction value and input conversions.
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

#include "TransactionConversions.h"
#include "../CardanoInterop.h"

#include <cardano/address/address.h>
#include <cardano/assets/multi_asset.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/transaction_body/value.h>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
Value
copyValueFromNative(cardano_value_t* value)
{
  CardanoObject<cardano_multi_asset_t, cardano_multi_asset_unref> assets(cardano_value_get_multi_asset(value));

  if (!assets)
  {
    return Value(cardano_value_get_coin(value));
  }

  return Value(cardano_value_get_coin(value), MultiAsset::fromCbor(encodeCbor(assets.get(), cardano_multi_asset_to_cbor)));
}

cardano_utxo_list_t*
copyUtxosToNative(const UtxoList& utxos)
{
  cardano_utxo_list_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_utxo_list_new(&value);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> owner(value);

  checkCardanoError(status, "UTxO list");

  for (const Utxo& utxo: utxos)
  {
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> item(
      decodeCbor<cardano_utxo_t, cardano_utxo_unref>(utxo.toCbor(), cardano_utxo_from_cbor));

    checkCardanoError(cardano_utxo_list_add(owner.get(), item.get()), "UTxO list insertion");
  }

  return owner.release();
}

cardano_address_t*
copyAddressToNative(const Address& address)
{
  const std::vector<std::uint8_t>                         bytes  = address.toBytes();
  cardano_address_t*                                      value  = nullptr;
  const cardano_error_t                                   status = cardano_address_from_bytes(bytes.data(), bytes.size(), &value);
  CardanoObject<cardano_address_t, cardano_address_unref> owner(value);

  checkCardanoError(status, "Address conversion");

  return owner.release();
}
} // namespace Cometa
