/**
 * @file RandomImproveCoinSelector.h
 *
 * @brief Selects UTxOs with the random-improve strategy.
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
#include <Cometa/TransactionBuilder/SelectionStrategy.h>

#include <cstdint>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Selects UTxOs with the random-improve strategy.
 *
 * The strategy first covers required amounts, then improves the selection toward its target. A
 * supplied seed makes selection reproducible for the same request.
 */
class RandomImproveCoinSelector final : public CoinSelector
{
  public:

    /** @brief Creates a selector seeded from a secure random source. */
    RandomImproveCoinSelector();

    /**
     * @brief Creates a reproducible selector.
     * @param seed The deterministic random seed.
     */
    explicit RandomImproveCoinSelector(std::uint64_t seed);

    /**
     * @brief Creates a reproducible selector with a target strategy.
     * @param seed The deterministic random seed.
     * @param strategy The target amount strategy.
     */
    RandomImproveCoinSelector(std::uint64_t seed, SelectionStrategy strategy);

  public:

    /**
     * @brief Gets the strategy name.
     * @return The name.
     */
    std::string name() const override;

    /**
     * @brief Selects outputs using random-improve.
     * @param request The outputs and value to cover.
     * @return The selected inputs, remaining inputs, and change.
     */
    CoinSelectionResult select(const CoinSelectionRequest& request) const override;

  private:

    /** @brief Seed used for deterministic selection. */
    std::uint64_t m_seed;

    /** @brief Whether selection uses the explicit seed. */
    bool m_hasSeed;

    /** @brief Target amount strategy. */
    SelectionStrategy m_strategy;
};
} // namespace Cometa
