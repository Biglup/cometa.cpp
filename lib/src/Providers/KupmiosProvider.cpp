/**
 * @file KupmiosProvider.cpp
 *
 * @brief Kupmios provider operations.
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

#include <Cometa/Providers/KupmiosProvider.h>

#include "RemoteProvider.h"

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
KupmiosProvider::KupmiosProvider(NetworkMagic network, std::string kupoEndpoint, std::string ogmiosEndpoint, std::shared_ptr<HttpTransport> transport, std::string kupoToken, std::string ogmiosToken)
: m_provider(std::make_shared<RemoteProvider>(RemoteBackend::Kupmios, network, std::move(transport), std::move(kupoEndpoint), std::move(ogmiosEndpoint), std::move(kupoToken), std::move(ogmiosToken)))
{
}

std::string
KupmiosProvider::name() const
{
  return m_provider->name();
}

NetworkMagic
KupmiosProvider::networkMagic() const
{
  return m_provider->networkMagic();
}

ProtocolParameters
KupmiosProvider::getParameters() const
{
  return m_provider->getParameters();
}

UtxoList
KupmiosProvider::getUnspentOutputs(const Address& address) const
{
  return m_provider->getUnspentOutputs(address);
}

std::uint64_t
KupmiosProvider::getRewardsAvailable(const RewardAddress& address) const
{
  return m_provider->getRewardsAvailable(address);
}

UtxoList
KupmiosProvider::getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const
{
  return m_provider->getUnspentOutputsWithAsset(address, asset);
}

Utxo
KupmiosProvider::getUnspentOutputByNft(const AssetId& asset) const
{
  return m_provider->getUnspentOutputByNft(asset);
}

UtxoList
KupmiosProvider::resolveUnspentOutputs(const TransactionInputSet& inputs) const
{
  return m_provider->resolveUnspentOutputs(inputs);
}

PlutusData
KupmiosProvider::resolveDatum(const Blake2bHash& hash) const
{
  return m_provider->resolveDatum(hash);
}

bool
KupmiosProvider::confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const
{
  return m_provider->confirmTransaction(id, timeout);
}

Blake2bHash
KupmiosProvider::submitTransaction(const Transaction& transaction) const
{
  return m_provider->submitTransaction(transaction);
}

RedeemerList
KupmiosProvider::evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const
{
  return m_provider->evaluateTransaction(transaction, additionalUtxos);
}
} // namespace Cometa
