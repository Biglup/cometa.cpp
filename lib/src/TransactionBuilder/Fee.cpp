/**
 * @file Fee.cpp
 *
 * @brief Fee calculations and serialized size measurements.
 *
 * @author angel.castillo
 * @date   Nov 02, 2026
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

#include <Cometa/TransactionBuilder/Fee.h>

#include "../CardanoInterop.h"
#include "TransactionConversions.h"

#include <cardano/common/ex_units.h>
#include <cardano/common/unit_interval.h>
#include <cardano/protocol_params/ex_unit_prices.h>
#include <cardano/scripts/script.h>
#include <cardano/transaction_builder/fee.h>

#include <limits>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::uint64_t
computeTransactionFee(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeResolvedInputs(copyUtxosToNative(resolvedInputs));

  std::uint64_t result {};
  checkCardanoError(cardano_compute_transaction_fee(nativeTransaction.get(), nativeResolvedInputs.get(), parameters.checked(), &result), "computeTransactionFee");

  return result;
}

std::uint64_t
computeMinAdaRequired(const TransactionOutput& output, std::uint64_t coinsPerUtxoByte)
{
  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> nativeOutput(
    decodeCbor<cardano_transaction_output_t, cardano_transaction_output_unref>(output.toCbor(), cardano_transaction_output_from_cbor));

  std::uint64_t result {};
  checkCardanoError(cardano_compute_min_ada_required(nativeOutput.get(), coinsPerUtxoByte, &result), "computeMinAdaRequired");

  return result;
}

std::uint64_t
computeMinScriptFee(const Transaction& transaction, const ExUnitPrices& prices, const UtxoList& resolvedInputs, const UnitInterval& coinsPerRefScriptByte)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));
  CardanoObject<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref> nativePrices(
    decodeCbor<cardano_ex_unit_prices_t, cardano_ex_unit_prices_unref>(prices.toCbor(), cardano_ex_unit_prices_from_cbor));
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>         nativeResolvedInputs(copyUtxosToNative(resolvedInputs));
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> nativeCoinsPerRefScriptByte(
    decodeCbor<cardano_unit_interval_t, cardano_unit_interval_unref>(coinsPerRefScriptByte.toCbor(), cardano_unit_interval_from_cbor));

  std::uint64_t result {};
  checkCardanoError(cardano_compute_min_script_fee(nativeTransaction.get(), nativePrices.get(), nativeResolvedInputs.get(), nativeCoinsPerRefScriptByte.get(), &result), "computeMinScriptFee");

  return result;
}

std::uint64_t
computeMinFeeWithoutScripts(const Transaction& transaction, std::uint64_t constant, std::uint64_t coefficient)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));

  std::uint64_t result {};
  checkCardanoError(cardano_compute_min_fee_without_scripts(nativeTransaction.get(), constant, coefficient, &result), "computeMinFeeWithoutScripts");

  return result;
}

std::uint64_t
computeScriptRefFee(const UtxoList& resolvedInputs, const UnitInterval& coinsPerRefScriptByte)
{
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>         nativeResolvedInputs(copyUtxosToNative(resolvedInputs));
  CardanoObject<cardano_unit_interval_t, cardano_unit_interval_unref> nativeCoinsPerRefScriptByte(
    decodeCbor<cardano_unit_interval_t, cardano_unit_interval_unref>(coinsPerRefScriptByte.toCbor(), cardano_unit_interval_from_cbor));

  std::uint64_t result {};
  checkCardanoError(cardano_compute_script_ref_fee(nativeResolvedInputs.get(), nativeCoinsPerRefScriptByte.get(), &result), "computeScriptRefFee");

  return result;
}

std::uint64_t
computeScriptRefFee(const UtxoList& resolvedInputs, const ProtocolParameters& parameters)
{
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> nativeResolvedInputs(copyUtxosToNative(resolvedInputs));

  std::uint64_t result {};
  checkCardanoError(cardano_compute_script_ref_fee_with_params(nativeResolvedInputs.get(), parameters.checked(), &result), "computeScriptRefFee");

  return result;
}

std::size_t
serializedCoinSize(std::uint64_t lovelace)
{
  std::size_t result {};
  checkCardanoError(cardano_get_serialized_coin_size(lovelace, &result), "serializedCoinSize");

  return result;
}

std::size_t
serializedOutputSize(const TransactionOutput& output)
{
  CardanoObject<cardano_transaction_output_t, cardano_transaction_output_unref> nativeOutput(
    decodeCbor<cardano_transaction_output_t, cardano_transaction_output_unref>(output.toCbor(), cardano_transaction_output_from_cbor));

  std::size_t result {};
  checkCardanoError(cardano_get_serialized_output_size(nativeOutput.get(), &result), "serializedOutputSize");

  return result;
}

std::size_t
serializedScriptSize(const Script& script)
{
  CardanoObject<cardano_script_t, cardano_script_unref> nativeScript(
    decodeCbor<cardano_script_t, cardano_script_unref>(script.toCbor(), cardano_script_from_cbor));

  std::size_t result {};
  checkCardanoError(cardano_get_serialized_script_size(nativeScript.get(), &result), "serializedScriptSize");

  return result;
}

std::size_t
serializedTransactionSize(const Transaction& transaction)
{
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));

  std::size_t result {};
  checkCardanoError(cardano_get_serialized_transaction_size(nativeTransaction.get(), &result), "serializedTransactionSize");

  return result;
}

ExUnits
totalExecutionUnits(const RedeemerList& redeemers)
{
  std::uint64_t memory = 0;
  std::uint64_t steps  = 0;

  for (const Redeemer& redeemer: redeemers)
  {
    const ExUnits units = redeemer.exUnits();

    if (units.memory() > std::numeric_limits<std::uint64_t>::max() - memory || units.cpuSteps() > std::numeric_limits<std::uint64_t>::max() - steps)
    {
      throw Error(ErrorCode::IntegerOverflow, "totalExecutionUnits");
    }

    memory += units.memory();
    steps  += units.cpuSteps();
  }

  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> nativeRedeemers(
    decodeCbor<cardano_redeemer_list_t, cardano_redeemer_list_unref>(redeemers.toCbor(), cardano_redeemer_list_from_cbor));

  cardano_ex_units_t*                                       result = nullptr;
  const cardano_error_t                                     status = cardano_get_total_ex_units_in_redeemers(nativeRedeemers.get(), &result);
  CardanoObject<cardano_ex_units_t, cardano_ex_units_unref> owner(result);

  checkCardanoError(status, "totalExecutionUnits");

  return ExUnits::fromCbor(encodeCbor(owner.get(), cardano_ex_units_to_cbor));
}
} // namespace Cometa
