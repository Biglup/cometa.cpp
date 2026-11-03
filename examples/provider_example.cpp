/**
 * @file provider_example.cpp
 *
 * @brief Reads a preprod address and its staking rewards.
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

#include "Support/NetworkExample.h"

#include <Cometa/Cometa.h>

#include <cstdint>
#include <exception>
#include <iostream>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Payment address from the matching cardano-c example. */
const char* PAYMENT_ADDRESS = "addr_test1qrwyxh7zvw8kdp9ar70klytasryj4ejz5j3n5sfw29j8nejzg53k4wq9vaswlnhthl6har9tygqc9037xepeu5s2v32qn6lwrx";

/** @brief Reward address from the matching cardano-c example. */
const char* REWARD_ADDRESS = "stake_test1uppy2gm2hqzkwc80em4mlat73j4jyqvzhclrvsu72g9xg4q2yweet";
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::BlockfrostProvider provider  = CometaExamples::preprodProvider();
    const Cometa::Address            payment   = Cometa::Address::fromString(PAYMENT_ADDRESS);
    const Cometa::RewardAddress      reward    = Cometa::RewardAddress::fromBech32(REWARD_ADDRESS);
    const Cometa::UtxoList           outputs   = provider.getUnspentOutputs(payment);
    const std::uint64_t              available = provider.getRewardsAvailable(reward);
    Cometa::Value                    balance(0);

    for (const Cometa::Utxo& output: outputs)
    {
      balance.addCoin(output.output().amount().coin());
    }

    std::cout << "Available lovelace: " << balance.coin() << '\n';
    std::cout << "Withdrawable rewards: " << available << '\n';
    std::cout << "Total lovelace: " << static_cast<std::uint64_t>(balance.coin()) + available << '\n';

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Provider example failed: " << error.what() << '\n';
    return 1;
  }
}
