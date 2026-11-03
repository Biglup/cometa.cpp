/**
 * @file CoinSelector.h
 *
 * @brief Coin selection strategy interface.
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

#include <Cometa/TransactionBuilder/CoinSelectionRequest.h>
#include <Cometa/TransactionBuilder/CoinSelectionResult.h>

#include <string>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Selects inputs and calculates change for a target value. */
class CoinSelector
{
  public:

    /** @brief Destroys the selector. */
    virtual ~CoinSelector() = default;

  public:

    /** @brief Gets the strategy name. @return The name. */
    virtual std::string name() const = 0;

    /**
     * @brief Selects outputs to cover a requested value.
     * @param request The outputs and value to cover.
     * @return The selected inputs, remaining inputs, and change.
     */
    virtual CoinSelectionResult select(const CoinSelectionRequest& request) const = 0;
};
} // namespace Cometa
