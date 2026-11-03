/**
 * @file BlockfrostProvider.h
 *
 * @brief Blockfrost network provider.
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
/** @brief Reads Cardano network data and submits transactions through Blockfrost. */
class BlockfrostProvider final : public Provider
{
  public:

    /** @brief Creates a provider. @param network The Cardano network. @param projectId The Blockfrost project ID. @param transport The HTTP transport. */
    BlockfrostProvider(NetworkMagic network, std::string projectId, std::shared_ptr<HttpTransport> transport);

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

    /** @copydoc Provider::getUnspentOutputByNft */
    Utxo getUnspentOutputByNft(const AssetId& asset) const override;

    /** @copydoc Provider::resolveUnspentOutputs */
    UtxoList resolveUnspentOutputs(const TransactionInputSet& inputs) const override;

    /** @copydoc Provider::resolveDatum */
    PlutusData resolveDatum(const Blake2bHash& hash) const override;

    /** @copydoc Provider::confirmTransaction */
    bool confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const override;

    /** @copydoc Provider::submitTransaction */
    Blake2bHash submitTransaction(const Transaction& transaction) const override;

    /** @copydoc Provider::evaluateTransaction */
    RedeemerList evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const override;

  private:

    /** @brief Makes an absolute endpoint URL. @param path The API path. @return The URL. */
    std::string url(const std::string& path) const;

    /** @brief Executes an authenticated request. @param method The HTTP method. @param path The API path. @param body Request bytes. @param contentType The body media type. @return The response. */
    HttpResponse request(
      const std::string&               method,
      const std::string&               path,
      const std::vector<std::uint8_t>& body        = {},
      const std::string&               contentType = {}) const;

    /** @brief Parses a successful JSON response. @param response The response. @return The JSON value. */
    JsonValue parseJson(const HttpResponse& response) const;

    /** @brief Loads all pages of address outputs. @param path The initial path. @return The outputs. */
    UtxoList loadUtxos(const std::string& path) const;

  private:

    /** @brief Network magic. */
    NetworkMagic m_network;

    /** @brief Authentication token. */
    std::string m_projectId;

    /** @brief Request transport. */
    std::shared_ptr<HttpTransport> m_transport;
};
} // namespace Cometa
