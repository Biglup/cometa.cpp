/**
 * @file reward_account_pubkey_example.cpp
 *
 * @brief Registers, delegates, withdraws, and unregisters a stake key.
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
#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include <exception>
#include <iostream>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Reward address from the matching cardano-c example. */
const char* REWARD_ADDRESS = "stake_test1uppy2gm2hqzkwc80em4mlat73j4jyqvzhclrvsu72g9xg4q2yweet";

/** @brief Pool from the matching cardano-c example. */
const char* POOL_ID = "pool1pzdqdxrv0k74p4q33y98f2u7vzaz95et7mjeedjcfy0jcgk754f";
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::SecureKeyHandler   signer   = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider = CometaExamples::preprodProvider();
    const Cometa::Address            payment  = CometaExamples::paymentAddress(signer);
    const Cometa::RewardAddress      reward   = Cometa::RewardAddress::fromBech32(REWARD_ADDRESS);
    const Cometa::Blake2bHash        pool(Cometa::decodeBech32(POOL_ID).bytes);

    const Cometa::ProtocolParameters parameters = provider.getParameters();
    const Cometa::SlotConfig         slots      = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);
    Cometa::DerivationPath           stakePath;
    stakePath.role = Cometa::DerivationRole::Staking;
    const std::vector<Cometa::DerivationPath> signingPaths { Cometa::DerivationPath {}, stakePath };

    Cometa::TransactionBuilder registration(parameters, slots);
    registration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .registerRewardAddress(reward)
      .delegateStake(reward, pool)
      .delegateVotingPower(reward, Cometa::DRep(Cometa::DRepType::Abstain));

    Cometa::Transaction registrationTransaction = registration.build();

    CometaExamples::signAndSubmit(signer, provider, registrationTransaction, signingPaths);

    Cometa::TransactionBuilder deregistration(parameters, slots);
    deregistration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .withdrawRewards(reward, 0)
      .deregisterRewardAddress(reward);

    Cometa::Transaction deregistrationTransaction = deregistration.build();

    CometaExamples::signAndSubmit(signer, provider, deregistrationTransaction, signingPaths);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Reward account pubkey example failed: " << error.what() << '\n';
    return 1;
  }
}
