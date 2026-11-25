/**
 * @file HydraProvider.h
 *
 * @brief A Provider implementation using the Hydra API for one Cardano network.
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

#include <Cometa/Json/JsonValue.h>
#include <Cometa/Providers/HttpTransport.h>
#include <Cometa/Providers/Provider.h>

#include <memory>
#include <string>
#include <vector>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Hydra blockchain queries, evaluation and explicit submission.
 *
 * Quantities and prices are decoded exactly. HTTP failures are distinct from
 * empty results. Independent queries do not constitute an atomic snapshot.
 * Submission is never retried automatically. Confirmation observes inclusion,
 * not L1 settlement. Confirmation polls only the latest snapshot transaction
 * list; older transactions absent from it cannot be rediscovered. The injected
 * transport must honor request deadlines. Staking rewards and remote evaluation
 * are unsupported; configure a local evaluator for Head script transactions.
 */
class HydraProvider final : public Provider
{
  public:

    /**
     * @brief Creates an HTTP provider for an existing Conway Hydra Head.
     * @param network Explicit Cardano network identity.
     * @param token Optional bearer credential, never included in diagnostics.
     * @param transport HTTP transport shared by this provider.
     * @param endpoint Absolute Hydra HTTP(S) endpoint.
     * @throws std::invalid_argument Configuration is invalid.
     */
    HydraProvider(NetworkMagic network, std::string endpoint, std::shared_ptr<HttpTransport> transport, std::string token = {});

  public:

    /** @copydoc Provider::name */
    std::string name() const override;

    /** @copydoc Provider::networkMagic */
    NetworkMagic networkMagic() const override;

    /** @copydoc Provider::getParameters */
    ProtocolParameters getParameters() const override;

    /** @copydoc Provider::getUnspentOutputs */
    UtxoList getUnspentOutputs(const Address& address) const override;

    /** @copydoc Provider::getRewardsAvailable */
    std::uint64_t getRewardsAvailable(const RewardAddress& address) const override;

    /** @copydoc Provider::getUnspentOutputsWithAsset */
    UtxoList getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const override;

    /**
     * @copydoc Provider::getUnspentOutputByNft
     * @throws std::invalid_argument The asset is lovelace.
     * @throws std::runtime_error The asset is not held as one NFT or its output cannot be found.
     */
    Utxo getUnspentOutputByNft(const AssetId& asset) const override;

    /** @copydoc Provider::resolveUnspentOutputs */
    UtxoList resolveUnspentOutputs(const TransactionInputSet& inputs) const override;

    /** @copydoc Provider::resolveDatum */
    PlutusData resolveDatum(const Blake2bHash& hash) const override;

    /**
     * @copydoc Provider::confirmTransaction
     * @throws std::invalid_argument The timeout is negative.
     */
    bool confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const override;

    /** @copydoc Provider::submitTransaction */
    Blake2bHash submitTransaction(const Transaction& transaction) const override;

    /** @copydoc Provider::evaluateTransaction */
    RedeemerList evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const override;

  private:

    /** @brief Shared connection configuration and provider operations. */
    std::shared_ptr<Provider> m_provider;
};
} // namespace Cometa
