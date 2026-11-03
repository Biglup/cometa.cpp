/**
 * @file drep_pubkey_example.cpp
 *
 * @brief Registers, delegates to, and deregisters a key DRep on preprod.
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
/** @brief Governance metadata from the matching cardano-c example. */
const char* ANCHOR_URL = "https://storage.googleapis.com/biglup/Angel_Castillo.jsonld";

/** @brief Governance metadata hash from the matching cardano-c example. */
const char* ANCHOR_HASH = "26ce09df4e6f64fe5cf248968ab78f4b8a0092580c234d78f68c079c0fce34f0";

/** @brief Reward account from the matching cardano-c example. */
const char* REWARD_ADDRESS = "stake_test1uppy2gm2hqzkwc80em4mlat73j4jyqvzhclrvsu72g9xg4q2yweet";
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::SecureKeyHandler   signer     = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider   = CometaExamples::preprodProvider();
    const Cometa::Address            payment    = CometaExamples::paymentAddress(signer);
    const Cometa::RewardAddress      reward     = Cometa::RewardAddress::fromBech32(REWARD_ADDRESS);
    const Cometa::ProtocolParameters parameters = provider.getParameters();
    const Cometa::SlotConfig         slots      = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);
    const Cometa::Bip32PublicKey     account    = signer.accountPublicKey(Cometa::AccountDerivationPath {});
    const Cometa::Credential         credential(
      Cometa::CredentialType::KeyHash, account.derive({ 3, 0 }).toEd25519().hash());
    const Cometa::DRep   drep(credential);
    const Cometa::Anchor anchor(ANCHOR_URL, Cometa::Blake2bHash::fromHex(ANCHOR_HASH));

    Cometa::DerivationPath stakePath;
    stakePath.role = Cometa::DerivationRole::Staking;
    Cometa::DerivationPath drepPath;
    drepPath.role = Cometa::DerivationRole::DRep;
    const std::vector<Cometa::DerivationPath> stakeSigners { Cometa::DerivationPath {}, stakePath };
    const std::vector<Cometa::DerivationPath> drepSigners { Cometa::DerivationPath {}, drepPath };

    Cometa::TransactionBuilder registration(parameters, slots);
    registration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .registerDRep(drep, anchor);
    Cometa::Transaction registrationTransaction = registration.build();
    CometaExamples::signAndSubmit(signer, provider, registrationTransaction, drepSigners);

    Cometa::TransactionBuilder delegation(parameters, slots);
    delegation.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .registerRewardAddress(reward)
      .delegateVotingPower(reward, drep);
    Cometa::Transaction delegationTransaction = delegation.build();
    CometaExamples::signAndSubmit(signer, provider, delegationTransaction, stakeSigners);

    Cometa::TransactionBuilder rewardDeregistration(parameters, slots);
    rewardDeregistration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .withdrawRewards(reward, 0)
      .deregisterRewardAddress(reward);
    Cometa::Transaction rewardDeregistrationTransaction = rewardDeregistration.build();
    CometaExamples::signAndSubmit(signer, provider, rewardDeregistrationTransaction, stakeSigners);

    Cometa::TransactionBuilder drepDeregistration(parameters, slots);
    drepDeregistration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .deregisterDRep(drep);
    Cometa::Transaction drepDeregistrationTransaction = drepDeregistration.build();
    CometaExamples::signAndSubmit(signer, provider, drepDeregistrationTransaction, drepSigners);
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "DRep pubkey example failed: " << error.what() << '\n';
    return 1;
  }
}
