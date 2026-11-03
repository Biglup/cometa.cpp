/**
 * @file ImplicitCoin.h
 *
 * @brief Implicit transaction lovelace accounting.
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

#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/Transaction/SubTransaction.h>
#include <Cometa/Transaction/Transaction.h>

#include <cstdint>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/**
 * @brief Lovelace entering or leaving a transaction through rewards and deposits.
 *
 * Pool registrations are treated as new registrations because no chain registration state is queried.
 * Pool retirement refunds occur at an epoch boundary and are not included here.
 */
struct ImplicitCoin
{
    /** @brief Reward withdrawals available to the transaction, in lovelace. */
    std::uint64_t withdrawals = 0;

    /** @brief Certificate and governance deposits paid, in lovelace. */
    std::uint64_t deposits = 0;

    /** @brief Certificate deposits reclaimed, in lovelace. */
    std::uint64_t reclaimDeposits = 0;
};

/**
 * @brief Computes rewards, deposits, and deposit refunds.
 *
 * The result does not include UTxO values, fees, minting, or treasury donations.
 * @param transaction The transaction to inspect.
 * @param parameters The current protocol parameters for the target network.
 * @return The implicit lovelace components.
 * @throws Error The operation cannot be completed with the supplied values.
 */
ImplicitCoin computeImplicitCoin(const Transaction& transaction, const ProtocolParameters& parameters);

/**
 * @brief Computes rewards, deposits, and deposit refunds.
 *
 * The result does not include UTxO values, fees, minting, or treasury donations.
 * @param transaction The transaction to inspect.
 * @param parameters The current protocol parameters for the target network.
 * @return The implicit lovelace components.
 * @throws Error The operation cannot be completed with the supplied values.
 */
ImplicitCoin computeImplicitCoin(const SubTransaction& transaction, const ProtocolParameters& parameters);
} // namespace Cometa
