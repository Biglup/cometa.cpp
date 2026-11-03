/**
 * @file mint_burn_native_script_example.cpp
 *
 * @brief Mints and burns tokens under a native script on preprod.
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

/** @brief CIP-25 metadata from the matching cardano-c example. */
const char* EXAMPLE_CIP_25_METADATA = R"({"b863bc7369f46136ac1048adb2fa7dae3af944c3bbb2be2f216a8d4f":{"BerryOnyx":{"color":"#0F0F0F","image":"ipfs://ipfs/QmS7w3Q5oVL9NE1gJnsMVPp6fcxia1e38cRT5pE5mmxawL","name":"Berry Onyx"},"BerryRaspberry":{"color":"#E30B5D","image":"ipfs://ipfs/QmXjegt568JqSUpAz9phxbXq5noWE3AeymZMUP43Ej2DRZ","name":"Berry Raspberry"}}})";
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

    const Cometa::ProtocolParameters parameters = provider.getParameters();

    const Cometa::NativeScript nativeScript = Cometa::NativeScript::fromJson(ALWAYS_SUCCEEDS_NATIVE_SCRIPT);

    const Cometa::Script     script(nativeScript);
    const Cometa::AssetId    onyx(script.hash(), Cometa::AssetName::fromString("BerryOnyx"));
    const Cometa::AssetId    raspberry(script.hash(), Cometa::AssetName::fromString("BerryRaspberry"));
    const Cometa::SlotConfig slots = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);

    Cometa::Value minted(2'000'000);
    minted.addAsset(onyx, 1);
    minted.addAsset(raspberry, 1);

    Cometa::TransactionBuilder mint(parameters, slots);

    mint.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .setMetadata(721, Cometa::Metadatum::fromPlainJson(EXAMPLE_CIP_25_METADATA))
      .mintToken(onyx, 1)
      .mintToken(raspberry, 1)
      .addScript(script)
      .sendValue(payment, minted);

    Cometa::Transaction mintTransaction = mint.build();

    CometaExamples::signAndSubmit(signer, provider, mintTransaction);

    Cometa::TransactionBuilder burn(parameters, slots);

    burn.setUtxos(provider.getUnspentOutputs(payment))
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .mintToken(onyx, -1)
      .addScript(script);

    Cometa::Transaction burnTransaction = burn.build();

    CometaExamples::signAndSubmit(signer, provider, burnTransaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Native mint and burn example failed: " << error.what() << '\n';
    return 1;
  }
}
