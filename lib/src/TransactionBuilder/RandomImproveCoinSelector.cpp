/**
 * @file RandomImproveCoinSelector.cpp
 *
 * @brief Random-improve coin selection strategy.
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

#include <Cometa/TransactionBuilder/RandomImproveCoinSelector.h>

#include "../CardanoInterop.h"
#include "CoinSelectorAdapter.h"

#include <cardano/transaction_builder/coin_selection/coin_selector.h>
#include <cardano/transaction_builder/coin_selection/random_improve_coin_selector.h>

#include <stdexcept>

/* IMPLEMENTATION ************************************************************/

namespace Cometa
{
RandomImproveCoinSelector::RandomImproveCoinSelector()
: m_seed(0)
, m_hasSeed(false)
, m_strategy(SelectionStrategy::Optimal)
{
}

RandomImproveCoinSelector::RandomImproveCoinSelector(std::uint64_t seed)
: m_seed(seed)
, m_hasSeed(true)
, m_strategy(SelectionStrategy::Optimal)
{
}

RandomImproveCoinSelector::RandomImproveCoinSelector(std::uint64_t seed, SelectionStrategy strategy)
: m_seed(seed)
, m_hasSeed(true)
, m_strategy(strategy)
{
  if (strategy != SelectionStrategy::Minimal && strategy != SelectionStrategy::Optimal)
  {
    throw std::invalid_argument("Unknown coin selection strategy");
  }
}

std::string
RandomImproveCoinSelector::name() const
{
  return "random-improve";
}

CoinSelectionResult
RandomImproveCoinSelector::select(const CoinSelectionRequest& request) const
{
  cardano_coin_selector_t* value  = nullptr;
  cardano_error_t          status = CARDANO_SUCCESS;

  if (!m_hasSeed)
  {
    status = cardano_random_improve_coin_selector_new(&value);
  }
  else if (m_strategy == SelectionStrategy::Minimal)
  {
    status = cardano_random_improve_coin_selector_new_with_options(
      m_seed, CARDANO_SELECTION_STRATEGY_MINIMAL, &value);
  }
  else
  {
    status = cardano_random_improve_coin_selector_new_with_seed(m_seed, &value);
  }

  CardanoObject<cardano_coin_selector_t, cardano_coin_selector_unref> selector(value);
  checkCardanoError(status, "RandomImproveCoinSelector");

  return selectWithNative(selector.get(), request);
}
} // namespace Cometa
