/**
 * @file CoinSelectorAdapter.h
 *
 * @brief Native callback bridge for C++ coin selectors.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/TransactionBuilder/CoinSelector.h>

#include <memory>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_coin_selector_t;

/* FUNCTION DECLARATION ******************************************************/

namespace Cometa
{
/**
 * @brief Creates a native selector that forwards to a C++ implementation.
 * @param selector The implementation to retain.
 * @return An owned native selector.
 */
cardano_coin_selector_t* createCoinSelectorAdapter(std::shared_ptr<CoinSelector> selector);
} // namespace Cometa
