/**
 * @file deferred_redeemer_batch_example.cpp
 *
 * @brief Spends two script outputs with input and payout indices computed after balancing.
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
/** @brief Compiled input/output-pairing Plutus V3 validator from the C example. */
const char* ORDER_PAIRING_VALIDATOR_V3 =
  "59022c01010029800aba2aba1aba0aab9faab9eaab9dab9a488888896600264653001300800198041804800cdc3a4005"
  "30080024888966002600460106ea800e2653001300d00198069807000cdc3a40009112cc004c004c030dd500444c8c8c"
  "c8966002602a007132323232598009804980a1baa001899192cc004c02cc058dd5000c56600266ebcc068c05cdd5180d"
  "180b9baa00300d899b894820225e8c9660026022602e6ea80062900044dd6980d980c1baa001405864b3001301130173"
  "75400314c103d87a8000899198008009bab301c3019375400444b30010018a6103d87a8000899192cc004cdc8a450000"
  "18acc004cdc7a44100001898059980f180e00125eb82298103d87a80004069133004004302000340686eb8c068004c07"
  "400501b202c32330010013756600660306ea8c06cc060dd5001112cc004006298103d87a8000899192cc004cdc8a4500"
  "0018acc004cdc7a44100001898051980e980d80125eb82298103d87a80004065133004004301f00340646eb8c064004c"
  "07000501a4528202a8b202a330033758603260346034602c6ea8038dd69800980b1baa01123019301a0018b202633001"
  "3758602e60286ea8030dd6980b980a1baa00f3001001222598008014530103d87a80008acc004c024006260086602e60"
  "3000497ae08cc00400e6032005337009000800a006404c80b0dd2a40011640486eb4c048004dd6980900118090009806"
  "9baa0088b201618049baa0038b200e180400098019baa0088a4d1365640041";

/** @brief Funds one validator output. @param signer The signer. @param provider The network provider. @param parameters Protocol parameters. @param slots The network timeline. @param payment The funding address. @param scriptAddress The validator address. @param datum The inline datum. @return The funded output. */
Cometa::Utxo
fundScript(
  const Cometa::SecureKeyHandler&   signer,
  const Cometa::BlockfrostProvider& provider,
  const Cometa::ProtocolParameters& parameters,
  const Cometa::SlotConfig&         slots,
  const Cometa::Address&            payment,
  const Cometa::Address&            scriptAddress,
  const Cometa::PlutusData&         datum)
{
  Cometa::TransactionBuilder builder(parameters, slots);

  builder.setUtxos(provider.getUnspentOutputs(payment))
    .setChangeAddress(payment)
    .setInvalidAfter(CometaExamples::expirySlot())
    .lockLovelace(scriptAddress, 2'000'000, Cometa::Datum(datum));

  Cometa::Transaction transaction = builder.build();
  CometaExamples::signAndSubmit(signer, provider, transaction);
  return Cometa::Utxo(Cometa::TransactionInput(transaction.id(), 0), transaction.body().outputs().at(0));
}

/** @brief Creates the redeemer for one input and its payout. @param input The script input. @param payoutAddress The payout destination. @param payoutLovelace The minimum payout. @param draft The balanced transaction. @return The redeemer. */
Cometa::PlutusData
pairingRedeemer(
  const Cometa::TransactionInput& input,
  const Cometa::Address&          payoutAddress,
  std::uint64_t                   payoutLovelace,
  const Cometa::Transaction&      draft)
{
  const std::optional<std::uint64_t> inputIndex  = draft.findInputIndex(input);
  const std::optional<std::uint64_t> outputIndex = draft.findOutputIndex(payoutAddress, payoutLovelace);

  if (!inputIndex.has_value() || !outputIndex.has_value())
  {
    throw std::runtime_error("Script input or payout is absent from the balanced transaction");
  }

  Cometa::PlutusList fields;
  fields.pushBack(Cometa::PlutusData::fromUnsigned(*inputIndex));
  fields.pushBack(Cometa::PlutusData::fromUnsigned(*outputIndex));
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

    const Cometa::Script     script(Cometa::PlutusV3Script::fromHex(ORDER_PAIRING_VALIDATOR_V3));
    const Cometa::Credential credential(Cometa::CredentialType::ScriptHash, script.hash());
    const Cometa::Address    scriptAddress =
      Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, credential).toAddress();
    const Cometa::PlutusData datum = Cometa::PlutusData::fromCbor("d87980");

    const Cometa::Utxo     inputA         = fundScript(signer, provider, parameters, slots, payment, scriptAddress, datum);
    const Cometa::Utxo     inputB         = fundScript(signer, provider, parameters, slots, payment, scriptAddress, datum);
    const Cometa::UtxoList paymentOutputs = provider.getUnspentOutputs(payment);

    Cometa::TransactionBuilder spending(parameters, slots);

    spending.setUtxos(paymentOutputs)
      .setCollateralUtxos(paymentOutputs)
      .setCollateralChangeAddress(payment)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .addScript(script)
      .sendLovelace(payment, 1'200'000)
      .sendLovelace(payment, 1'500'000)
      .addInputWithDeferredRedeemer(
        inputA,
        [input = inputA.input(), payment](const Cometa::Transaction& draft, const Cometa::UtxoList&)
        {
          return pairingRedeemer(input, payment, 1'200'000, draft);
        })
      .addInputWithDeferredRedeemer(
        inputB,
        [input = inputB.input(), payment](const Cometa::Transaction& draft, const Cometa::UtxoList&)
        {
          return pairingRedeemer(input, payment, 1'500'000, draft);
        });

    Cometa::Transaction transaction = spending.build();

    CometaExamples::signAndSubmit(signer, provider, transaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Deferred batch example failed: " << error.what() << '\n';
    return 1;
  }
}
