/**
 * @file HydraProvider.cpp
 *
 * @brief Hydra provider operations.
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

#include <Cometa/Providers/HydraProvider.h>

#include "RemoteProvider.h"

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
HydraProvider::HydraProvider(NetworkMagic network, std::string endpoint, std::shared_ptr<HttpTransport> transport, std::string token)
: m_provider(std::make_shared<RemoteProvider>(RemoteBackend::Hydra, network, std::move(transport), std::move(endpoint), "", std::move(token), ""))
{
}

std::string
HydraProvider::name() const
{
  return m_provider->name();
}

NetworkMagic
HydraProvider::networkMagic() const
{
  return m_provider->networkMagic();
}

ProtocolParameters
HydraProvider::getParameters() const
{
  return m_provider->getParameters();
}

UtxoList
HydraProvider::getUnspentOutputs(const Address& address) const
{
  return m_provider->getUnspentOutputs(address);
}

std::uint64_t
HydraProvider::getRewardsAvailable(const RewardAddress& address) const
{
  return m_provider->getRewardsAvailable(address);
}

UtxoList
HydraProvider::getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const
{
  return m_provider->getUnspentOutputsWithAsset(address, asset);
}

Utxo
HydraProvider::getUnspentOutputByNft(const AssetId& asset) const
{
  return m_provider->getUnspentOutputByNft(asset);
}

UtxoList
HydraProvider::resolveUnspentOutputs(const TransactionInputSet& inputs) const
{
  return m_provider->resolveUnspentOutputs(inputs);
}

PlutusData
HydraProvider::resolveDatum(const Blake2bHash& hash) const
{
  return m_provider->resolveDatum(hash);
}

bool
HydraProvider::confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const
{
  return m_provider->confirmTransaction(id, timeout);
}

Blake2bHash
HydraProvider::submitTransaction(const Transaction& transaction) const
{
  return m_provider->submitTransaction(transaction);
}

RedeemerList
HydraProvider::evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const
{
  return m_provider->evaluateTransaction(transaction, additionalUtxos);
}
} // namespace Cometa
