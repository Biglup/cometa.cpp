/**
 * @file RemoteProvider.h
 *
 * @brief Private remote provider implementation.
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
#include <map>
#include <memory>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Supported remote wire dialects. */
enum class RemoteBackend
{
  Hydra,
  Koios,
  Kupmios
};

/** @brief Private provider state shared by the public named backends. */
class RemoteProvider final : public Provider
{
  public:

    /** @brief Creates validated connection state.
     * @param backend Wire dialect.
     * @param network Chain identity.
     * @param transport HTTP implementation.
     * @param queryEndpoint Query endpoint.
     * @param rpcEndpoint Ogmios endpoint, or empty for Hydra and Koios.
     * @param queryToken Query credential.
     * @param rpcToken Ogmios credential.
     */
    RemoteProvider(RemoteBackend backend, NetworkMagic network, std::shared_ptr<HttpTransport> transport, std::string queryEndpoint, std::string rpcEndpoint, const std::string& queryToken, const std::string& rpcToken);

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

    /** @brief Sends one bounded request with no retries.
     * @param path Relative encoded path and query.
     * @param body Optional request bytes.
     * @param type Request media type.
     * @param rpc Select Ogmios connection.
     * @param timeout Remaining request budget.
     * @return The response, including non-success statuses.
     */
    HttpResponse request(const std::string& path, const std::optional<std::string>& body = std::nullopt, const std::string& type = "application/json", bool rpc = false, std::chrono::milliseconds timeout = std::chrono::milliseconds(30000)) const;

    /** @brief Executes a correlated stateless Ogmios method.
     * @param method Method name.
     * @param params Encoded parameters.
     * @return The result.
     */
    JsonValue rpc(const std::string& method, const std::string& params = "{}") const;

    /** @brief Loads all cursor or offset pages.
     * @param path Relative endpoint.
     * @param body Optional encoded POST body.
     * @return Response rows.
     */
    std::vector<JsonValue> pages(const std::string& path, const std::optional<std::string>& body = std::nullopt) const;

    /** @brief Decodes a ledger output and verifies script/datum hashes.
     * @param row Response row.
     * @return The decoded output.
     */
    Utxo output(const JsonValue& row) const;

    /** @brief Loads current outputs and rejects duplicates.
     * @param path Relative endpoint.
     * @param body Optional encoded POST body.
     * @return Current outputs.
     */
    UtxoList outputs(const std::string& path, const std::optional<std::string>& body = std::nullopt) const;

    /** @brief Checks indexed inclusion within the remaining request budget.
     * @param id Expected transaction ID.
     * @param timeout Remaining budget.
     * @return True if indexed in a block.
     */
    bool confirmed(const Blake2bHash& id, std::chrono::milliseconds timeout) const;

  private:

    /** @brief Wire dialect. */
    RemoteBackend m_backend;

    /** @brief Explicit chain identity. */
    NetworkMagic m_network;

    /** @brief Caller-owned HTTP stack. */
    std::shared_ptr<HttpTransport> m_transport;

    /** @brief Query base URL, possibly containing query credentials. */
    std::string m_queryEndpoint;

    /** @brief Ogmios base URL. */
    std::string m_rpcEndpoint;

    /** @brief Query authentication headers. */
    std::vector<std::pair<std::string, std::string>> m_queryHeaders;

    /** @brief Ogmios authentication headers. */
    std::vector<std::pair<std::string, std::string>> m_rpcHeaders;
};
} // namespace Cometa
