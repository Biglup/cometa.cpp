/**
 * @file KoiosProvider.cpp
 *
 * @brief Koios provider operations.
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

#include <Cometa/Providers/KoiosProvider.h>

#include "RemoteProvider.h"

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
KoiosProvider::KoiosProvider(NetworkMagic network, std::shared_ptr<HttpTransport> transport, std::string token, std::string endpoint)
: m_provider(std::make_shared<RemoteProvider>(RemoteBackend::Koios, network, std::move(transport), std::move(endpoint), "", std::move(token), ""))
{
}

std::string
KoiosProvider::name() const
{
  return m_provider->name();
}

NetworkMagic
KoiosProvider::networkMagic() const
{
  return m_provider->networkMagic();
}

ProtocolParameters
KoiosProvider::getParameters() const
{
  return m_provider->getParameters();
}

UtxoList
KoiosProvider::getUnspentOutputs(const Address& address) const
{
  return m_provider->getUnspentOutputs(address);
}

std::uint64_t
KoiosProvider::getRewardsAvailable(const RewardAddress& address) const
{
  return m_provider->getRewardsAvailable(address);
}

UtxoList
KoiosProvider::getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const
{
  return m_provider->getUnspentOutputsWithAsset(address, asset);
}

Utxo
KoiosProvider::getUnspentOutputByNft(const AssetId& asset) const
{
  return m_provider->getUnspentOutputByNft(asset);
}

UtxoList
KoiosProvider::resolveUnspentOutputs(const TransactionInputSet& inputs) const
{
  return m_provider->resolveUnspentOutputs(inputs);
}

PlutusData
KoiosProvider::resolveDatum(const Blake2bHash& hash) const
{
  return m_provider->resolveDatum(hash);
}

bool
KoiosProvider::confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const
{
  return m_provider->confirmTransaction(id, timeout);
}

Blake2bHash
KoiosProvider::submitTransaction(const Transaction& transaction) const
{
  return m_provider->submitTransaction(transaction);
}

RedeemerList
KoiosProvider::evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const
{
  return m_provider->evaluateTransaction(transaction, additionalUtxos);
}
} // namespace Cometa
