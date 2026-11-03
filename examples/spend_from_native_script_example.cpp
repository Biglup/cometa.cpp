/**
 * @file spend_from_native_script_example.cpp
 *
 * @brief Funds and spends a native script output on preprod.
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

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Native script from the matching cardano-c example. */
const char* ALWAYS_SUCCEEDS_NATIVE_SCRIPT = R"({"type":"all","scripts":[{"type":"before","slot":"1001655683199"}]})";
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::SecureKeyHandler   signer         = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider       = CometaExamples::preprodProvider();
    const Cometa::Address            paymentAddress = CometaExamples::paymentAddress(signer);

    const Cometa::ProtocolParameters parameters = provider.getParameters();

    const Cometa::NativeScript nativeScript = Cometa::NativeScript::fromJson(ALWAYS_SUCCEEDS_NATIVE_SCRIPT);

    const Cometa::Script     script(nativeScript);
    const Cometa::Credential scriptCredential(Cometa::CredentialType::ScriptHash, script.hash());
    const Cometa::Address    scriptAddress =
      Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, scriptCredential).toAddress();

    Cometa::TransactionBuilder funding(parameters, Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));
    funding.setUtxos(provider.getUnspentOutputs(paymentAddress))
      .setChangeAddress(paymentAddress)
      .setInvalidAfter(CometaExamples::expirySlot())
      .sendLovelace(scriptAddress, 2000000);

    Cometa::Transaction fundingTransaction = funding.build();

    CometaExamples::signAndSubmit(signer, provider, fundingTransaction);

    const Cometa::Utxo scriptOutput(
      Cometa::TransactionInput(fundingTransaction.id(), 0), fundingTransaction.body().outputs().at(0));

    Cometa::TransactionBuilder spending(parameters, Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));
    spending.setUtxos(provider.getUnspentOutputs(paymentAddress))
      .setChangeAddress(paymentAddress)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .sendLovelace(paymentAddress, 1000000)
      .addInput(scriptOutput);

    Cometa::Transaction spendingTransaction = spending.build();

    CometaExamples::signAndSubmit(signer, provider, spendingTransaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Spend from native script example failed: " << error.what() << '\n';
    return 1;
  }
}
