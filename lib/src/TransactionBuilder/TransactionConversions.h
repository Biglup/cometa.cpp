/**
 * @file TransactionConversions.h
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Address/Address.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/TransactionBody/Value.h>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_utxo_list_t;
struct cardano_address_t;

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Copies a signed value without changing negative quantities.
 * @param value The borrowed native value.
 * @return An independent C++ value.
 */
Value copyValueFromNative(cardano_value_t* value);

/**
 * @brief Copies resolved outputs into an owned native list.
 * @param utxos The outputs to copy.
 * @return The owned list.
 */
cardano_utxo_list_t* copyUtxosToNative(const UtxoList& utxos);

/**
 * @brief Copies a change address into an owned native value.
 * @param address The address to copy.
 * @return The owned address.
 */
cardano_address_t* copyAddressToNative(const Address& address);

} // namespace Cometa
