/**
 * @file deferred_redeemer_self_index_example.cpp
 *
 * @brief Spends a validator with a redeemer derived from its final input index.
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

#include <cstdint>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Compiled index-checking Plutus V3 validator from the C example. */
const char* SELF_INDEX_VALIDATOR_V3 = "58f501010029800aba2aba1aba0aab9faab9eaab9dab9a488888896600264646644b30013370e900118031baa0018994c004c02c00660166018003370e90002444b30013001300a375400d1323259800980880144c9660026008601a6ea8006266ebcc044c038dd5180898071baa0010068b2018329800800cdd6180898071baa301100b9bad3011300e375401280088896600200514c103d87a80008acc004c018006266e9520003301230130024bd70466002007301400299b8048004005003201c40451640386eb4c03c004c02cdd50034590090c01cdd5000c59005180400098041804800980400098019baa0088a4d13656400401";
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
    const Cometa::Script             script(Cometa::PlutusV3Script::fromHex(SELF_INDEX_VALIDATOR_V3));
    const Cometa::Credential         credential(Cometa::CredentialType::ScriptHash, script.hash());
    const Cometa::Address            scriptAddress =
      Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, credential).toAddress();
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
    const Cometa::UtxoList         paymentOutputs = provider.getUnspentOutputs(payment);
    const Cometa::TransactionInput scriptInput    = scriptOutput.input();

    Cometa::TransactionBuilder spending(parameters, slots);
    spending.setUtxos(paymentOutputs)
      .setCollateralUtxos(paymentOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .sendLovelace(payment, 1000000)
      .addInputWithDeferredRedeemer(
        scriptOutput,
        [scriptInput](const Cometa::Transaction& draft, const Cometa::UtxoList&)
        {
          const std::optional<std::uint64_t> index = draft.findInputIndex(scriptInput);

          if (!index.has_value())
          {
            throw std::runtime_error("Script input is absent from the balanced transaction");
          }

          Cometa::PlutusList fields;
          fields.pushBack(Cometa::PlutusData::fromUnsigned(*index));
          return Cometa::PlutusData(Cometa::ConstrPlutusData(0, fields));
        });

    Cometa::Transaction spendingTransaction = spending.build();
    CometaExamples::signAndSubmit(signer, provider, spendingTransaction);
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Deferred self-index example failed: " << error.what() << '\n';
    return 1;
  }
}
