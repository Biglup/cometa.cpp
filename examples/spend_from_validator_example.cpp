/**
 * @file spend_from_validator_example.cpp
 *
 * @brief Funds and spends a Plutus V3 validator using its reference script.
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

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Reference script output used by the matching cardano-c example. */
const char* REFERENCE_SCRIPT_TX = "77c33469c6f21be375880f294da85fec13df50821f6c6591eab9eff723e68e66";
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
    const Cometa::ProtocolParameters parameters = provider.getParameters();
    const Cometa::SlotConfig         slots      = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);

    const Cometa::TransactionInputSet referenceInputs { Cometa::TransactionInput::fromHex(REFERENCE_SCRIPT_TX, 0) };
    const Cometa::UtxoList            resolved = provider.resolveUnspentOutputs(referenceInputs);

    if (resolved.size() != 1 || !resolved.at(0).output().scriptRef().has_value())
    {
      throw std::runtime_error("Reference script output is unavailable");
    }

    const Cometa::Utxo reference = resolved.at(0);

    const Cometa::Script     script = *reference.output().scriptRef();
    const Cometa::Credential scriptCredential(Cometa::CredentialType::ScriptHash, script.hash());
    const Cometa::Address    scriptAddress =
      Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, scriptCredential).toAddress();
    const Cometa::PlutusData voidData = Cometa::PlutusData::fromCbor("d87980");

    Cometa::TransactionBuilder funding(parameters, slots);
    funding.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .lockLovelace(scriptAddress, 2000000, Cometa::Datum(voidData));

    Cometa::Transaction fundingTransaction = funding.build();

    CometaExamples::signAndSubmit(signer, provider, fundingTransaction);

    const Cometa::Utxo scriptOutput(
      Cometa::TransactionInput(fundingTransaction.id(), 0), fundingTransaction.body().outputs().at(0));
    const Cometa::UtxoList paymentOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder spending(parameters, slots);
    spending.setUtxos(paymentOutputs)
      .setCollateralUtxos(paymentOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addReferenceInput(reference)
      .sendLovelace(payment, 1000000)
      .addInput(scriptOutput, voidData);

    Cometa::Transaction spendingTransaction = spending.build();

    CometaExamples::signAndSubmit(signer, provider, spendingTransaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Spend from validator example failed: " << error.what() << '\n';
    return 1;
  }
}
