/**
 * @file vote_for_proposal_pubkey_example.cpp
 *
 * @brief Registers a key DRep, votes on a proposal, and deregisters.
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
#include <string>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Governance metadata from the matching cardano-c example. */
const char* ANCHOR_URL = "https://storage.googleapis.com/biglup/Angel_Castillo.jsonld";

/** @brief Hash of the governance metadata. */
const char* ANCHOR_HASH = "26ce09df4e6f64fe5cf248968ab78f4b8a0092580c234d78f68c079c0fce34f0";
} // namespace

/* MAIN **********************************************************************/

int
main(int argc, char** argv)
{
  if (argc < 2 || argc > 3)
  {
    std::cerr << "Usage: " << argv[0] << " <proposal transaction id> [proposal index]\n";
    return 1;
  }

  try
  {
    const Cometa::Blake2bHash        proposalId    = Cometa::Blake2bHash::fromHex(argv[1]);
    const std::uint64_t              proposalIndex = argc == 3 ? std::stoull(argv[2]) : 0;
    const Cometa::GovernanceActionId action(proposalId, proposalIndex);
    const Cometa::SecureKeyHandler   signer   = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider = CometaExamples::preprodProvider();
    const Cometa::Address            payment  = CometaExamples::paymentAddress(signer);

    const Cometa::ProtocolParameters parameters = provider.getParameters();
    const Cometa::SlotConfig         slots      = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);

    const Cometa::Bip32PublicKey account = signer.accountPublicKey(Cometa::AccountDerivationPath {});
    const Cometa::Credential     credential(
      Cometa::CredentialType::KeyHash, account.derive({ 3, 0 }).toEd25519().hash());
    const Cometa::DRep     drep(credential);
    const Cometa::Voter    voter(Cometa::VoterType::DRepKeyHash, credential);
    const Cometa::Anchor   anchor(ANCHOR_URL, Cometa::Blake2bHash::fromHex(ANCHOR_HASH));
    Cometa::DerivationPath drepPath;
    drepPath.role = Cometa::DerivationRole::DRep;
    const std::vector<Cometa::DerivationPath> signingPaths { Cometa::DerivationPath {}, drepPath };

    Cometa::TransactionBuilder registration(parameters, slots);
    registration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .registerDRep(drep, anchor);

    Cometa::Transaction registrationTransaction = registration.build();

    CometaExamples::signAndSubmit(signer, provider, registrationTransaction, signingPaths);

    Cometa::TransactionBuilder voting(parameters, slots);
    voting.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .vote(voter, action, Cometa::VotingProcedure(Cometa::Vote::Yes));

    Cometa::Transaction votingTransaction = voting.build();

    CometaExamples::signAndSubmit(signer, provider, votingTransaction, signingPaths);

    Cometa::TransactionBuilder deregistration(parameters, slots);
    deregistration.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .deregisterDRep(drep);

    Cometa::Transaction deregistrationTransaction = deregistration.build();

    CometaExamples::signAndSubmit(signer, provider, deregistrationTransaction, signingPaths);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Key DRep voting example failed: " << error.what() << '\n';
    return 1;
  }
}
