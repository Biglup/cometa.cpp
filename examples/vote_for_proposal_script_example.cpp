/**
 * @file vote_for_proposal_script_example.cpp
 *
 * @brief Registers a script DRep, votes on a proposal, and deregisters.
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
#include <stdexcept>
#include <string>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Governance metadata from the matching cardano-c example. */
const char* ANCHOR_URL = "https://storage.googleapis.com/biglup/Angel_Castillo.jsonld";

/** @brief Hash of the governance metadata. */
const char* ANCHOR_HASH = "26ce09df4e6f64fe5cf248968ab78f4b8a0092580c234d78f68c079c0fce34f0";

/** @brief Deployment of the validator used by the matching cardano-c example. */
const char* REFERENCE_SCRIPT_TX = "77c33469c6f21be375880f294da85fec13df50821f6c6591eab9eff723e68e66";
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
    const Cometa::GovernanceActionId action(
      Cometa::Blake2bHash::fromHex(argv[1]), argc == 3 ? std::stoull(argv[2]) : 0);
    const Cometa::SecureKeyHandler   signer     = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider   = CometaExamples::preprodProvider();
    const Cometa::Address            payment    = CometaExamples::paymentAddress(signer);
    const Cometa::ProtocolParameters parameters = provider.getParameters();
    const Cometa::SlotConfig         slots      = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);

    const Cometa::TransactionInputSet referenceInputs { Cometa::TransactionInput::fromHex(REFERENCE_SCRIPT_TX, 0) };
    const Cometa::UtxoList            resolved = provider.resolveUnspentOutputs(referenceInputs);

    if (resolved.size() != 1 || !resolved.at(0).output().scriptRef().has_value())
    {
      throw std::runtime_error("Voting validator reference script is unavailable");
    }

    const Cometa::Script     script = *resolved.at(0).output().scriptRef();
    const Cometa::Credential credential(Cometa::CredentialType::ScriptHash, script.hash());
    const Cometa::DRep       drep(credential);
    const Cometa::Voter      voter(Cometa::VoterType::DRepScriptHash, credential);
    const Cometa::Anchor     anchor(ANCHOR_URL, Cometa::Blake2bHash::fromHex(ANCHOR_HASH));
    const Cometa::PlutusData redeemer = Cometa::PlutusData::fromCbor("d87980");

    const Cometa::UtxoList registrationOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder registration(parameters, slots);

    registration.setUtxos(registrationOutputs)
      .setCollateralUtxos(registrationOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .registerDRep(drep, anchor, redeemer);

    Cometa::Transaction registrationTransaction = registration.build();

    CometaExamples::signAndSubmit(signer, provider, registrationTransaction);

    const Cometa::UtxoList votingOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder voting(parameters, slots);

    voting.setUtxos(votingOutputs)
      .setCollateralUtxos(votingOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .vote(voter, action, Cometa::VotingProcedure(Cometa::Vote::Yes), redeemer);

    Cometa::Transaction votingTransaction = voting.build();

    CometaExamples::signAndSubmit(signer, provider, votingTransaction);

    const Cometa::UtxoList deregistrationOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder deregistration(parameters, slots);

    deregistration.setUtxos(deregistrationOutputs)
      .setCollateralUtxos(deregistrationOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .deregisterDRep(drep, redeemer);

    Cometa::Transaction deregistrationTransaction = deregistration.build();

    CometaExamples::signAndSubmit(signer, provider, deregistrationTransaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Script DRep voting example failed: " << error.what() << '\n';
    return 1;
  }
}
