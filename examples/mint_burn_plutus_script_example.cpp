/**
 * @file mint_burn_plutus_script_example.cpp
 *
 * @brief Mints and burns tokens under a reference Plutus V3 script.
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

/** @brief CIP-25 metadata from the matching cardano-c example. */
const char* EXAMPLE_CIP_25_METADATA = R"({"b863bc7369f46136ac1048adb2fa7dae3af944c3bbb2be2f216a8d4f":{"PlutusBerryOnyx":{"color":"#0F0F0F","image":"ipfs://ipfs/QmS7w3Q5oVL9NE1gJnsMVPp6fcxia1e38cRT5pE5mmxawL","name":"Plutus Berry Onyx"},"PlutusBerryRaspberry":{"color":"#E30B5D","image":"ipfs://ipfs/QmXjegt568JqSUpAz9phxbXq5noWE3AeymZMUP43Ej2DRZ","name":"Plutus Berry Raspberry"}}})";
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
      throw std::runtime_error("Minting reference script is unavailable");
    }

    const Cometa::Utxo reference = resolved.at(0);

    const Cometa::Script     script = *reference.output().scriptRef();
    const Cometa::AssetId    onyx(script.hash(), Cometa::AssetName::fromString("PlutusBerryOnyx"));
    const Cometa::AssetId    raspberry(script.hash(), Cometa::AssetName::fromString("PlutusBerryRaspberry"));
    const Cometa::PlutusData redeemer = Cometa::PlutusData::fromCbor("d87980");

    Cometa::Value minted(2'000'000);
    minted.addAsset(onyx, 1);
    minted.addAsset(raspberry, 1);

    const Cometa::UtxoList mintOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder mint(parameters, slots);

    mint.setUtxos(mintOutputs)
      .setCollateralUtxos(mintOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .setMetadata(721, Cometa::Metadatum::fromPlainJson(EXAMPLE_CIP_25_METADATA))
      .mintToken(onyx, 1, redeemer)
      .mintToken(raspberry, 1, redeemer)
      .addReferenceInput(reference)
      .sendValue(payment, minted);

    Cometa::Transaction mintTransaction = mint.build();

    CometaExamples::signAndSubmit(signer, provider, mintTransaction);

    const Cometa::UtxoList burnOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder burn(parameters, slots);

    burn.setUtxos(burnOutputs)
      .setCollateralUtxos(burnOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .mintToken(onyx, -1, redeemer)
      .addReferenceInput(reference);

    Cometa::Transaction burnTransaction = burn.build();

    CometaExamples::signAndSubmit(signer, provider, burnTransaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Plutus mint and burn example failed: " << error.what() << '\n';
    return 1;
  }
}
