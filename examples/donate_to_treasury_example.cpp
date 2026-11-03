/**
 * @file donate_to_treasury_example.cpp
 *
 * @brief Donates lovelace to the treasury on preprod.
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

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::SecureKeyHandler   signer     = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider   = CometaExamples::preprodProvider();
    const Cometa::Address            sender     = CometaExamples::paymentAddress(signer);
    const Cometa::UtxoList           utxos      = provider.getUnspentOutputs(sender);
    const Cometa::ProtocolParameters parameters = provider.getParameters();

    Cometa::TransactionBuilder builder(parameters, Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));
    builder.setUtxos(utxos)
      .setChangeAddress(sender)
      .setInvalidAfter(CometaExamples::expirySlot())
      .setDonation(2000000);

    Cometa::Transaction transaction = builder.build();
    CometaExamples::signAndSubmit(signer, provider, transaction);
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Treasury donation example failed: " << error.what() << '\n';
    return 1;
  }
}
