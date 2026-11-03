/**
 * @file deferred_redeemer_withdraw_zero_example.cpp
 *
 * @brief Withdraws zero rewards with a redeemer based on the selected inputs.
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

#include <algorithm>
#include <cstdint>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Compiled Plutus V3 validator from the C example. */
const char* WITHDRAW_ZERO_VALIDATOR_V3 =
  "5901ad01010029800aba2aba1aba0aab9faab9eaab9dab9a488888896600264646530013005375400330090039804801"
  "2444b30013370e9002001c4cc8966002600260146ea800e264b30013010001899198008009bac30100022259800800c5"
  "6600266e1cc8cc004004dd6180918079baa0072259800800c52000899b8048008cc008008c05000501119198008009ba"
  "c3012300f375401044b30010018a4001133700900119801001180a000a022899199119914c00400a003480050021112c"
  "c00400a2946264b300133710004003159800acc004c02cc050dd519803002800c528c52820268cc00401260320070014"
  "01114a0809a29410131bad301700240546eb0c050c044dd500498008009bac3012300f3754010444b30010028a6103d8"
  "7a80008acc004c01c006266e9520003301330140024bd70466002007301500299b8048004005003201e404914a080622"
  "646600600660260046eb4c04400500f45900d18059baa0038b2012300c300937540086e1d20008acc004cdc3a400c007"
  "13233224a26eb4c034004c034c038004c024dd5002459007200e18041804800980400098019baa0088a4d13656400401";

/** @brief SMAUG stake pool identifier. */
const char* POOL_ID_HEX = "089a06986c7dbd50d411890a74ab9e60ba22d32bf6e59cb658491f2c";

/** @brief Creates the redeemer from the final canonical input positions. @param draft The balanced transaction. @param resolvedInputs Its selected inputs. @return The redeemer. */
Cometa::PlutusData
withdrawalRedeemer(const Cometa::Transaction& draft, const Cometa::UtxoList& resolvedInputs)
{
  std::vector<std::uint64_t> positions;
  positions.reserve(resolvedInputs.size());

  for (const Cometa::Utxo& utxo: resolvedInputs)
  {
    const std::optional<std::uint64_t> index = draft.findInputIndex(utxo.input());

    if (!index.has_value())
    {
      throw std::runtime_error("Resolved input is absent from the balanced transaction");
    }

    positions.push_back(*index);
  }

  std::sort(positions.begin(), positions.end());

  Cometa::PlutusList indices;

  for (std::uint64_t position: positions)
  {
    indices.pushBack(Cometa::PlutusData::fromUnsigned(position));
  }

  Cometa::PlutusList fields;
  fields.pushBack(Cometa::PlutusData(indices));
  return Cometa::PlutusData(Cometa::ConstrPlutusData(0, fields));
}
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
    const Cometa::SlotConfig         slots      = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);

    const Cometa::Script        script(Cometa::PlutusV3Script::fromHex(WITHDRAW_ZERO_VALIDATOR_V3));
    const Cometa::Credential    credential(Cometa::CredentialType::ScriptHash, script.hash());
    const Cometa::RewardAddress reward(Cometa::NetworkId::Testnet, credential);
    const Cometa::Blake2bHash   pool     = Cometa::Blake2bHash::fromHex(POOL_ID_HEX);
    const Cometa::PlutusData    voidData = Cometa::PlutusData::fromCbor("d87980");

    const Cometa::UtxoList registrationOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder registration(parameters, slots);
    registration.setUtxos(registrationOutputs)
      .setCollateralUtxos(registrationOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .registerRewardAddress(reward, voidData)
      .delegateStake(reward, pool, voidData);

    Cometa::Transaction registrationTransaction = registration.build();

    CometaExamples::signAndSubmit(signer, provider, registrationTransaction);

    const Cometa::UtxoList withdrawalOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder withdrawal(parameters, slots);
    withdrawal.setUtxos(withdrawalOutputs)
      .setCollateralUtxos(withdrawalOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .withdrawRewardsWithDeferredRedeemer(reward, 0, withdrawalRedeemer)
      .deregisterRewardAddress(reward, voidData);

    Cometa::Transaction withdrawalTransaction = withdrawal.build();

    CometaExamples::signAndSubmit(signer, provider, withdrawalTransaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Deferred zero-withdrawal example failed: " << error.what() << '\n';
    return 1;
  }
}
