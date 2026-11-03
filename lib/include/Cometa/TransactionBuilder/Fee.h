/**
 * @file Fee.h
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Common/UtxoList.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/Scripts/Script.h>
#include <Cometa/Transaction/Transaction.h>

#include <cstddef>
#include <cstdint>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/**
 * @brief Computes the minimum fee for a transaction or batch.
 *
 * Uses the supplied reference-script pricing parameters. Inputs are not changed.
 * @param transaction The complete transaction, including any witness sizes to price.
 * @param resolvedInputs Resolved spend and reference inputs; include every priced occurrence, including those in sub-transactions.
 * @param parameters The current protocol parameters for the target network.
 * @return The fee in lovelace.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::uint64_t computeTransactionFee(const Transaction& transaction, const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

/**
 * @brief Computes an output's minimum lovelace requirement.
 * @param output The proposed output.
 * @param coinsPerUtxoByte The protocol cost in lovelace per accounted UTxO byte.
 * @return The minimum lovelace amount.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::uint64_t computeMinAdaRequired(const TransactionOutput& output, std::uint64_t coinsPerUtxoByte);

/**
 * @brief Computes execution and reference-script fees.
 *
 * Uses Conway reference-script pricing tiers; use computeTransactionFee() for protocol-specific pricing.
 * @param transaction The complete transaction, including any witness sizes to price.
 * @param prices Lovelace prices for execution memory and steps.
 * @param resolvedInputs Resolved spend and reference inputs; include every priced occurrence, including those in sub-transactions.
 * @param coinsPerRefScriptByte The reference-script fee rate in lovelace per byte.
 * @return The script fee in lovelace.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::uint64_t computeMinScriptFee(const Transaction& transaction, const ExUnitPrices& prices, const UtxoList& resolvedInputs, const UnitInterval& coinsPerRefScriptByte);

/**
 * @brief Computes the size-dependent fee without script charges.
 *
 * The ledger fee size excludes the transaction validity flag.
 * @param transaction The complete transaction, including any witness sizes to price.
 * @param constant The constant fee term in lovelace.
 * @param coefficient The fee per serialized transaction byte.
 * @return The fee in lovelace.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::uint64_t computeMinFeeWithoutScripts(const Transaction& transaction, std::uint64_t constant, std::uint64_t coefficient);

/**
 * @brief Computes reference-script fees using Conway pricing tiers.
 * @param resolvedInputs Resolved spend and reference inputs; include every priced occurrence, including those in sub-transactions.
 * @param coinsPerRefScriptByte The reference-script fee rate in lovelace per byte.
 * @return The reference-script fee in lovelace.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::uint64_t computeScriptRefFee(const UtxoList& resolvedInputs, const UnitInterval& coinsPerRefScriptByte);

/**
 * @brief Computes reference-script fees using network pricing parameters.
 *
 * Unset stride and multiplier parameters use Conway defaults.
 * @param resolvedInputs Resolved spend and reference inputs; include every priced occurrence, including those in sub-transactions.
 * @param parameters The current protocol parameters for the target network.
 * @return The reference-script fee in lovelace.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::uint64_t computeScriptRefFee(const UtxoList& resolvedInputs, const ProtocolParameters& parameters);

/**
 * @brief Computes the CBOR size of a lovelace amount.
 * @param lovelace The amount to encode.
 * @return The encoded size in bytes.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::size_t serializedCoinSize(std::uint64_t lovelace);

/**
 * @brief Computes an output's serialized size.
 * @param output The output to measure.
 * @return The size in bytes.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::size_t serializedOutputSize(const TransactionOutput& output);

/**
 * @brief Computes the ledger script size used for reference-script pricing.
 *
 * The measure depends on the script language; it is not necessarily Script::toCbor().size().
 * @param script The script to measure.
 * @return The script size in bytes.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::size_t serializedScriptSize(const Script& script);

/**
 * @brief Computes a complete transaction's serialized size.
 * @param transaction The complete transaction, including any witness sizes to price.
 * @return The size in bytes, including its current witnesses.
 * @throws Error The fee or size cannot be computed from the supplied values.
 */
std::size_t serializedTransactionSize(const Transaction& transaction);

/**
 * @brief Sums the memory and execution-step budgets of redeemers.
 * @param redeemers The redeemers whose budgets are added.
 * @return The total execution budget.
 * @throws Error A component of the sum exceeds the 64-bit unsigned range.
 */
ExUnits totalExecutionUnits(const RedeemerList& redeemers);
} // namespace Cometa
