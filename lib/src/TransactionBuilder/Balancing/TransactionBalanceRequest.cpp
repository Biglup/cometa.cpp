/**
 * @file TransactionBalanceRequest.cpp
 *
 * @brief Standalone balancing request values.
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

#include <Cometa/TransactionBuilder/Balancing/TransactionBalanceRequest.h>

#include <utility>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
TransactionBalanceRequest::TransactionBalanceRequest(
  Transaction        transaction,
  ProtocolParameters parameters,
  Address            changeAddress,
  SlotConfig         slots)
: transaction(std::move(transaction))
, parameters(std::move(parameters))
, changeAddress(std::move(changeAddress))
, slots(slots)
{
}
} // namespace Cometa
