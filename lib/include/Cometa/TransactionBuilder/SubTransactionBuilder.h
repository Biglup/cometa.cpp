/**
 * @file SubTransactionBuilder.h
 *
 * @brief Dijkstra sub-transaction construction.
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
#include <Cometa/Common/Credential.h>
#include <Cometa/Common/NetworkId.h>
#include <Cometa/Common/Utxo.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/SlotConfig.h>
#include <Cometa/Transaction/SubTransaction.h>
#include <Cometa/TransactionBody/TransactionOutput.h>
#include <Cometa/TransactionBody/Value.h>

#include <cstdint>
#include <optional>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_sub_tx_builder_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Builds one Dijkstra sub-transaction from independently owned values. */
class SubTransactionBuilder
{
  public:

    /** @brief Creates a builder. @param parameters Current protocol parameters. @param slots The network slot timeline. */
    SubTransactionBuilder(const ProtocolParameters& parameters, SlotConfig slots);

    /** @brief Moves a builder. @param other The source builder. */
    SubTransactionBuilder(SubTransactionBuilder&& other) noexcept;

    /** @brief Releases builder resources. */
    ~SubTransactionBuilder();

  public:

    /** @brief Sets the transaction network ID. @param network The network ID. @return This builder. */
    SubTransactionBuilder& setNetworkId(NetworkId network);

    /** @brief Sets the earliest valid slot. @param slot The slot. @return This builder. */
    SubTransactionBuilder& setInvalidBefore(std::uint64_t slot);

    /** @brief Sets the first invalid slot. @param slot The slot. @return This builder. */
    SubTransactionBuilder& setInvalidAfter(std::uint64_t slot);

    /**
     * @brief Adds a spendable input.
     * @param utxo The output to spend.
     * @param redeemer The script redeemer, when needed.
     * @param datum The datum, when it is not in the output.
     * @return This builder.
     */
    SubTransactionBuilder& addInput(
      const Utxo&                      utxo,
      const std::optional<PlutusData>& redeemer = std::nullopt,
      const std::optional<PlutusData>& datum    = std::nullopt);

    /** @brief Adds an output. @param output The output to add. @return This builder. */
    SubTransactionBuilder& addOutput(const TransactionOutput& output);

    /** @brief Adds a payment in lovelace. @param address The recipient. @param lovelace The amount. @return This builder. */
    SubTransactionBuilder& sendLovelace(const Address& address, std::uint64_t lovelace);

    /** @brief Adds a multi-asset payment. @param address The recipient. @param value The amount. @return This builder. */
    SubTransactionBuilder& sendValue(const Address& address, const Value& value);

    /** @brief Adds a guard. @param guard The required credential. @return This builder. */
    SubTransactionBuilder& addGuard(const Credential& guard);

    /**
     * @brief Requires a guard in the top-level transaction.
     * @param guard The required credential.
     * @param datum Datum for a script guard, when needed.
     * @return This builder.
     */
    SubTransactionBuilder& requireTopLevelGuard(
      const Credential&                guard,
      const std::optional<PlutusData>& datum = std::nullopt);

    /** @brief Builds one sub-transaction. @return The sub-transaction. */
    SubTransaction build();

  public:

    /** @brief Moves another builder into this instance. @param other The source. @return This builder. */
    SubTransactionBuilder& operator=(SubTransactionBuilder&& other) noexcept;

    /** @brief A builder cannot be copied. */
    SubTransactionBuilder(const SubTransactionBuilder&) = delete;

    /** @brief A builder cannot be copy assigned. */
    SubTransactionBuilder& operator=(const SubTransactionBuilder&) = delete;

  private:

    /** @brief Gets the native builder or throws after a move. @return The native builder. */
    cardano_sub_tx_builder_t* checked() const;

  private:

    /** @brief Owned native builder. */
    cardano_sub_tx_builder_t* m_value;
};
} // namespace Cometa
