/**
 * @file Provider.h
 *
 * @brief Blockchain data and transaction submission interface.
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
#include <Cometa/Address/RewardAddress.h>
#include <Cometa/Assets/AssetId.h>
#include <Cometa/Common/NetworkMagic.h>
#include <Cometa/Common/UtxoList.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/PlutusData/PlutusData.h>
#include <Cometa/ProtocolParams/ProtocolParameters.h>
#include <Cometa/Transaction/Transaction.h>
#include <Cometa/TransactionBody/TransactionInputSet.h>
#include <Cometa/WitnessSet/RedeemerList.h>

#include <chrono>
#include <cstdint>
#include <string>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Supplies network data and submits transactions. */
class Provider
{
  public:

    /** @brief Destroys the provider. */
    virtual ~Provider() = default;

  public:

    /** @brief Gets the provider name. @return The name. */
    virtual std::string name() const = 0;

    /** @brief Gets the connected network. @return The network magic. */
    virtual NetworkMagic networkMagic() const = 0;

    /** @brief Gets current protocol parameters. @return The parameters. */
    virtual ProtocolParameters getParameters() const = 0;

    /** @brief Gets all unspent outputs at an address.
     * @param address The address to query.
     * @return The unspent outputs.
     */
    virtual UtxoList getUnspentOutputs(const Address& address) const = 0;

    /** @brief Gets available staking rewards.
     * @param address The reward account to query.
     * @return The available lovelace.
     */
    virtual std::uint64_t getRewardsAvailable(const RewardAddress& address) const = 0;

    /** @brief Gets outputs containing an asset at an address.
     * @param address The address to query.
     * @param asset The asset to find.
     * @return The matching outputs.
     */
    virtual UtxoList getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const = 0;

    /** @brief Gets an output containing an NFT.
     * @param asset The NFT to find.
     * @return The matching output.
     */
    virtual Utxo getUnspentOutputByNft(const AssetId& asset) const = 0;

    /** @brief Resolves transaction inputs to outputs.
     * @param inputs The inputs to resolve.
     * @return The resolved outputs.
     */
    virtual UtxoList resolveUnspentOutputs(const TransactionInputSet& inputs) const = 0;

    /** @brief Resolves a datum hash.
     * @param hash The datum hash.
     * @return The datum.
     */
    virtual PlutusData resolveDatum(const Blake2bHash& hash) const = 0;

    /** @brief Waits for a transaction to be confirmed.
     * @param id The transaction ID.
     * @param timeout The maximum wait.
     * @return True when confirmed within the timeout.
     */
    virtual bool confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const = 0;

    /** @brief Submits a signed transaction.
     * @param transaction The transaction to submit.
     * @return The submitted transaction ID.
     */
    virtual Blake2bHash submitTransaction(const Transaction& transaction) const = 0;

    /** @brief Evaluates script execution budgets.
     * @param transaction The transaction to evaluate.
     * @param additionalUtxos Additional resolved outputs.
     * @return Redeemers with execution budgets.
     */
    virtual RedeemerList evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const = 0;
};
} // namespace Cometa
