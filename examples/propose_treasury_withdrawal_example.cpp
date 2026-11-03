/**
 * @file propose_treasury_withdrawal_example.cpp
 *
 * @brief Proposes a treasury withdrawal using a constitution reference script.
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
/** @brief Governance anchor from the matching cardano-c example. */
const char* METADATA_URL = "https://raw.githubusercontent.com/IntersectMBO/governance-actions/refs/heads/main/mainnet/2024-11-19-infohf/metadata.jsonld";

/** @brief Hash of the governance anchor. */
const char* METADATA_HASH = "93106d082a93e94df5aff74f678438bae3a647dac63465fbfcde6a3058f41a1e";

/** @brief Refund and withdrawal account. */
const char* STAKE_ADDRESS = "stake_test1uppy2gm2hqzkwc80em4mlat73j4jyqvzhclrvsu72g9xg4q2yweet";

/** @brief Constitution guardrails script hash. */
const char* CONSTITUTION_SCRIPT_HASH = "fa24fb305126805cf2164c161d852a0e7330cf988f1fe558cf7d4a64";

/** @brief Reference output from the matching cardano-c example. */
const char* REFERENCE_SCRIPT_TX = "9aabbac24d1e39cb3e677981c84998a4210bae8d56b0f60908eedb9f59efffc8";
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::SecureKeyHandler   signer         = CometaExamples::sampleSigner();
    const Cometa::BlockfrostProvider provider       = CometaExamples::preprodProvider();
    const Cometa::Address            payment        = CometaExamples::paymentAddress(signer);
    const Cometa::RewardAddress      reward         = Cometa::RewardAddress::fromBech32(STAKE_ADDRESS);
    const Cometa::ProtocolParameters parameters     = provider.getParameters();
    const Cometa::SlotConfig         slots          = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);
    const Cometa::UtxoList           paymentOutputs = provider.getUnspentOutputs(payment);

    const Cometa::TransactionInputSet referenceInputs { Cometa::TransactionInput::fromHex(REFERENCE_SCRIPT_TX, 0) };
    const Cometa::UtxoList            resolved = provider.resolveUnspentOutputs(referenceInputs);

    if (resolved.size() != 1 || !resolved.at(0).output().scriptRef().has_value())
    {
      throw std::runtime_error("Constitution reference script is unavailable");
    }

    const Cometa::Utxo        reference = resolved.at(0);
    const Cometa::Blake2bHash policy    = Cometa::Blake2bHash::fromHex(CONSTITUTION_SCRIPT_HASH);

    if (reference.output().scriptRef()->hash() != policy)
    {
      throw std::runtime_error("Constitution reference script hash mismatch");
    }

    Cometa::WithdrawalMap withdrawals;
    withdrawals.set(reward, 1000000000000ULL);

    Cometa::TransactionBuilder builder(parameters, slots);
    builder.setUtxos(paymentOutputs)
      .setChangeAddress(payment)
      .setInvalidAfter(CometaExamples::expirySlot())
      .setCollateralUtxos(paymentOutputs)
      .setCollateralChangeAddress(payment)
      .addReferenceInput(reference)
      .proposeTreasuryWithdrawals(
        reward,
        Cometa::Anchor(METADATA_URL, Cometa::Blake2bHash::fromHex(METADATA_HASH)),
        withdrawals,
        policy);

    Cometa::Transaction transaction = builder.build();

    CometaExamples::signAndSubmit(signer, provider, transaction);

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Treasury withdrawal proposal example failed: " << error.what() << '\n';
    return 1;
  }
}
