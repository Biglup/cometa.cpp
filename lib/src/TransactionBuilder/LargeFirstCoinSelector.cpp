/**
 * @file LargeFirstCoinSelector.cpp
 *
 * @brief Largest-first coin selection strategy.
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

#include <Cometa/TransactionBuilder/LargeFirstCoinSelector.h>

#include "../CardanoInterop.h"
#include "CoinSelectorAdapter.h"

#include <cardano/transaction_builder/coin_selection/coin_selector.h>
#include <cardano/transaction_builder/coin_selection/large_first_coin_selector.h>

/* IMPLEMENTATION ************************************************************/

namespace Cometa
{
std::string
LargeFirstCoinSelector::name() const
{
  return "large-first";
}

CoinSelectionResult
LargeFirstCoinSelector::select(const CoinSelectionRequest& request) const
{
  cardano_coin_selector_t*                                            value  = nullptr;
  const cardano_error_t                                               status = cardano_large_first_coin_selector_new(&value);
  CardanoObject<cardano_coin_selector_t, cardano_coin_selector_unref> selector(value);
  checkCardanoError(status, "LargeFirstCoinSelector");

  return selectWithNative(selector.get(), request);
}
} // namespace Cometa
