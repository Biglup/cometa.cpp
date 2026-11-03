/**
 * @file TransactionBuilder.h
 *
 * @brief Balanced transaction construction.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Address/Address.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/TransactionBody/TransactionOutput.h>
#include <Cometa/TransactionBody/Value.h>
#include <Cometa/TransactionBuilder/CoinSelector.h>
#include <Cometa/TransactionBuilder/TransactionEvaluator.h>

#include <cstdint>
#include <memory>
#include <optional>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_tx_builder_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Builds balanced Cardano transactions from C++ values. */
class TransactionBuilder
{
  public:

    /**
     * @brief Creates a builder for a network and protocol version.
     * @param parameters Current protocol parameters.
     * @param slots The network slot timeline.
     */
    TransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots);

    /** @brief Moves a builder. @param other The source builder. */
    TransactionBuilder(TransactionBuilder&& other) noexcept;

    /** @brief Releases builder resources. */
    ~TransactionBuilder();

  public:

    /** @brief Sets the coin selection strategy. @param selector The strategy to retain. @return This builder. */
    TransactionBuilder& setCoinSelector(std::shared_ptr<CoinSelector> selector);

    /** @brief Sets the script evaluator. @param evaluator The evaluator to retain. @return This builder. */
    TransactionBuilder& setTransactionEvaluator(std::shared_ptr<TransactionEvaluator> evaluator);

    /** @brief Sets the transaction network ID. @param network The network ID. @return This builder. */
    TransactionBuilder& setNetworkId(NetworkId network);

    /** @brief Sets the address for change outputs. @param address The change address. @return This builder. */
    TransactionBuilder& setChangeAddress(const Address& address);

    /** @brief Sets spendable outputs for coin selection. @param utxos The outputs. @return This builder. */
    TransactionBuilder& setUtxos(const UtxoList& utxos);

    /** @brief Sets collateral outputs for script execution. @param utxos The outputs. @return This builder. */
    TransactionBuilder& setCollateralUtxos(const UtxoList& utxos);

    /** @brief Sets the earliest valid slot. @param slot The slot. @return This builder. */
    TransactionBuilder& setInvalidBefore(std::uint64_t slot);

    /** @brief Sets the first invalid slot. @param slot The slot. @return This builder. */
    TransactionBuilder& setInvalidAfter(std::uint64_t slot);

    /** @brief Sets the minimum fee. @param lovelace The minimum lovelace. @return This builder. */
    TransactionBuilder& setMinimumFee(std::uint64_t lovelace);

    /** @brief Adds a payment in lovelace. @param address The recipient. @param lovelace The amount. @return This builder. */
    TransactionBuilder& sendLovelace(const Address& address, std::uint64_t lovelace);

    /** @brief Adds a multi-asset payment. @param address The recipient. @param value The amount. @return This builder. */
    TransactionBuilder& sendValue(const Address& address, const Value& value);

    /**
     * @brief Adds a spendable input.
     * @param utxo The output to spend.
     * @param redeemer The script redeemer, when needed.
     * @param datum The datum, when it is not in the output.
     * @return This builder.
     */
    TransactionBuilder& addInput(
      const Utxo&                      utxo,
      const std::optional<PlutusData>& redeemer = std::nullopt,
      const std::optional<PlutusData>& datum    = std::nullopt);

    /** @brief Adds an output. @param output The output to add. @return This builder. */
    TransactionBuilder& addOutput(const TransactionOutput& output);

    /** @brief Balances and builds the transaction. @return The transaction. */
    Transaction build();

  public:

    /** @brief Moves another builder into this instance. @param other The source. @return This builder. */
    TransactionBuilder& operator=(TransactionBuilder&& other) noexcept;

    /** @brief A builder cannot be copied. */
    TransactionBuilder(const TransactionBuilder&) = delete;

    /** @brief A builder cannot be copy assigned. */
    TransactionBuilder& operator=(const TransactionBuilder&) = delete;

  private:

    /** @brief Gets the native builder or throws after a move. @return The native builder. */
    cardano_tx_builder_t* checked() const;

  private:

    /** @brief Owned native builder. */
    cardano_tx_builder_t* m_value;

    /** @brief Retained selection strategy, when configured. */
    std::shared_ptr<CoinSelector> m_coinSelector;

    /** @brief Retained execution evaluator, when configured. */
    std::shared_ptr<TransactionEvaluator> m_evaluator;
};
} // namespace Cometa
