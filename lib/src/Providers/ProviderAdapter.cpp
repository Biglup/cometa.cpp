/**
 * @file ProviderAdapter.cpp
 *
 * @brief Native callbacks for C++ providers.
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

#include "ProviderAdapter.h"

#include "../CardanoInterop.h"

#include <cardano/address/address.h>
#include <cardano/address/reward_address.h>
#include <cardano/assets/asset_id.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/object.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/protocol_params/protocol_parameters.h>
#include <cardano/providers/provider.h>
#include <cardano/providers/provider_impl.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_body/transaction_input_set.h>
#include <cardano/witness_set/redeemer_list.h>

#include <chrono>
#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/** @brief Retains the C++ provider while native calls can reach it. */
struct ProviderContext : cardano_object_t
{
    /** @brief The retained provider. */
    std::shared_ptr<Provider> provider;

    /** @brief Stores the provider. @param value The provider to retain. */
    explicit ProviderContext(std::shared_ptr<Provider> value)
    : cardano_object_t { 1, &ProviderContext::deallocate, {} }
    , provider(std::move(value))
    {
    }

    /** @brief Releases the context. @param value The context to release. */
    static void
    deallocate(void* value)
    {
      delete static_cast<ProviderContext*>(static_cast<cardano_object_t*>(value));
    }
};

/**
 * @brief Gets the retained C++ provider.
 * @param implementation The native provider implementation.
 * @return The C++ provider.
 */
Provider&
providerOf(cardano_provider_impl_t* implementation)
{
  return *static_cast<ProviderContext*>(implementation->context)->provider;
}

/**
 * @brief Stores a provider error for the outer native call.
 * @param implementation The native provider implementation.
 * @param message The failure message.
 * @return A generic provider error.
 */
cardano_error_t
providerError(cardano_provider_impl_t* implementation, const char* message)
{
  std::snprintf(implementation->error_message, sizeof(implementation->error_message), "%s", message);
  return CARDANO_ERROR_GENERIC;
}

/**
 * @brief Copies a native address for a C++ provider.
 * @param value The native address.
 * @return The independent address.
 */
Address
copyAddress(const cardano_address_t* value)
{
  std::vector<std::uint8_t> bytes(cardano_address_get_bytes_size(value));
  checkCardanoError(cardano_address_to_bytes(value, bytes.data(), bytes.size()), "Provider address");
  return Address(bytes);
}

/**
 * @brief Copies a native reward address for a C++ provider.
 * @param value The native address.
 * @return The independent address.
 */
RewardAddress
copyRewardAddress(const cardano_reward_address_t* value)
{
  std::vector<std::uint8_t> bytes(cardano_reward_address_get_bytes_size(value));
  checkCardanoError(cardano_reward_address_to_bytes(value, bytes.data(), bytes.size()), "Provider rewardAddress");
  return RewardAddress(bytes);
}

/**
 * @brief Copies a native asset identifier.
 * @param value The native identifier.
 * @return The independent identifier.
 */
AssetId
copyAssetId(const cardano_asset_id_t* value)
{
  const std::uint8_t* bytes = cardano_asset_id_get_bytes(value);
  const std::size_t   size  = cardano_asset_id_get_bytes_size(value);
  if (size == 0)
  {
    return AssetId::lovelace();
  }

  return AssetId(std::vector<std::uint8_t>(bytes, bytes + size));
}

/**
 * @brief Copies a native hash.
 * @param value The native hash.
 * @return The independent hash.
 */
Blake2bHash
copyHash(const cardano_blake2b_hash_t* value)
{
  const std::uint8_t* bytes = cardano_blake2b_hash_get_data(value);
  const std::size_t   size  = cardano_blake2b_hash_get_bytes_size(value);
  return Blake2bHash(std::vector<std::uint8_t>(bytes, bytes + size));
}

/**
 * @brief Copies a native transaction.
 * @param value The native transaction.
 * @return The independent transaction.
 */
Transaction
copyTransaction(const cardano_transaction_t* value)
{
  return Transaction::fromCbor(encodeCbor(value, cardano_transaction_to_cbor));
}

/**
 * @brief Copies a native input set.
 * @param value The native inputs.
 * @return The independent input set.
 */
TransactionInputSet
copyInputs(const cardano_transaction_input_set_t* value)
{
  return TransactionInputSet::fromCbor(encodeCbor(value, cardano_transaction_input_set_to_cbor));
}

/**
 * @brief Copies native outputs into C++ values.
 * @param value The native output list, or null.
 * @return The independent outputs.
 */
UtxoList
copyUtxos(const cardano_utxo_list_t* value)
{
  UtxoList outputs;
  if (!value)
  {
    return outputs;
  }

  const std::size_t count = cardano_utxo_list_get_length(value);
  outputs.reserve(count);

  for (std::size_t index = 0; index < count; ++index)
  {
    cardano_utxo_t*                                   item   = nullptr;
    const cardano_error_t                             status = cardano_utxo_list_get(value, index, &item);
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> owner(item);
    checkCardanoError(status, "Provider UtxoList get");

    outputs.push_back(Utxo::fromCbor(encodeCbor(owner.get(), cardano_utxo_to_cbor)));
  }

  return outputs;
}

/**
 * @brief Copies C++ outputs into a native list.
 * @param outputs The C++ outputs.
 * @return The native list.
 */
cardano_utxo_list_t*
copyUtxos(const UtxoList& outputs)
{
  cardano_utxo_list_t*                                        value  = nullptr;
  const cardano_error_t                                       status = cardano_utxo_list_new(&value);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> owner(value);
  checkCardanoError(status, "Provider UtxoList");

  for (const Utxo& output: outputs)
  {
    const std::vector<std::uint8_t>                   bytes = output.toCbor();
    CardanoObject<cardano_utxo_t, cardano_utxo_unref> native(
      decodeCbor<cardano_utxo_t, cardano_utxo_unref>(bytes, cardano_utxo_from_cbor));

    checkCardanoError(cardano_utxo_list_add(owner.get(), native.get()), "Provider UtxoList add");
  }

  return owner.release();
}

/**
 * @brief Dispatches an address UTxO query.
 * @param implementation The native provider implementation.
 * @param address The address to query.
 * @param output Receives the outputs.
 * @return The native status.
 */
cardano_error_t
getUnspentOutputs(cardano_provider_impl_t* implementation, cardano_address_t* address, cardano_utxo_list_t** output)
{
  try
  {
    cardano_address_ref(address);
    CardanoObject<cardano_address_t, cardano_address_unref> owner(address);

    *output = copyUtxos(providerOf(implementation).getUnspentOutputs(copyAddress(owner.get())));
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider UTxO query failed");
  }
}

/**
 * @brief Dispatches a staking-reward query.
 * @param implementation The native provider implementation.
 * @param address The reward account to query.
 * @param output Receives the available lovelace.
 * @return The native status.
 */
cardano_error_t
getRewardsAvailable(cardano_provider_impl_t* implementation, cardano_reward_address_t* address, std::uint64_t* output)
{
  try
  {
    cardano_reward_address_ref(address);
    CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> owner(address);

    *output = providerOf(implementation).getRewardsAvailable(copyRewardAddress(owner.get()));
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider rewards query failed");
  }
}

/**
 * @brief Dispatches an asset-filtered UTxO query.
 * @param implementation The native provider implementation.
 * @param address The address to query.
 * @param asset The required asset.
 * @param output Receives the outputs.
 * @return The native status.
 */
cardano_error_t
getUnspentOutputsWithAsset(cardano_provider_impl_t* implementation, cardano_address_t* address, cardano_asset_id_t* asset, cardano_utxo_list_t** output)
{
  try
  {
    cardano_address_ref(address);
    CardanoObject<cardano_address_t, cardano_address_unref> addressOwner(address);
    cardano_asset_id_ref(asset);
    CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> assetOwner(asset);

    *output = copyUtxos(providerOf(implementation).getUnspentOutputsWithAsset(copyAddress(addressOwner.get()), copyAssetId(assetOwner.get())));
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider asset query failed");
  }
}

/**
 * @brief Dispatches an NFT UTxO query.
 * @param implementation The native provider implementation.
 * @param asset The NFT to find.
 * @param output Receives the output.
 * @return The native status.
 */
cardano_error_t
getUnspentOutputByNft(cardano_provider_impl_t* implementation, cardano_asset_id_t* asset, cardano_utxo_t** output)
{
  try
  {
    cardano_asset_id_ref(asset);
    CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> owner(asset);

    const Utxo result = providerOf(implementation).getUnspentOutputByNft(copyAssetId(owner.get()));
    *output           = decodeCbor<cardano_utxo_t, cardano_utxo_unref>(result.toCbor(), cardano_utxo_from_cbor);
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider NFT query failed");
  }
}

/**
 * @brief Dispatches input resolution.
 * @param implementation The native provider implementation.
 * @param inputs The transaction inputs.
 * @param output Receives the resolved outputs.
 * @return The native status.
 */
cardano_error_t
resolveUnspentOutputs(cardano_provider_impl_t* implementation, cardano_transaction_input_set_t* inputs, cardano_utxo_list_t** output)
{
  try
  {
    cardano_transaction_input_set_ref(inputs);
    CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> owner(inputs);

    *output = copyUtxos(providerOf(implementation).resolveUnspentOutputs(copyInputs(owner.get())));
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider input resolution failed");
  }
}

/**
 * @brief Dispatches datum resolution.
 * @param implementation The native provider implementation.
 * @param hash The datum hash.
 * @param output Receives the datum.
 * @return The native status.
 */
cardano_error_t
resolveDatum(cardano_provider_impl_t* implementation, cardano_blake2b_hash_t* hash, cardano_plutus_data_t** output)
{
  try
  {
    cardano_blake2b_hash_ref(hash);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(hash);

    const PlutusData datum = providerOf(implementation).resolveDatum(copyHash(owner.get()));
    *output                = decodeCbor<cardano_plutus_data_t, cardano_plutus_data_unref>(datum.toCbor(), cardano_plutus_data_from_cbor);
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider datum resolution failed");
  }
}

/**
 * @brief Dispatches transaction confirmation.
 * @param implementation The native provider implementation.
 * @param id The transaction ID.
 * @param timeoutMs The timeout in milliseconds.
 * @param confirmed Receives the confirmation status.
 * @return The native status.
 */
cardano_error_t
confirmTransaction(cardano_provider_impl_t* implementation, cardano_blake2b_hash_t* id, std::uint64_t timeoutMs, bool* confirmed)
{
  try
  {
    if (timeoutMs > static_cast<std::uint64_t>(std::numeric_limits<std::int64_t>::max()))
    {
      throw std::out_of_range("Confirmation timeout exceeds supported range");
    }

    cardano_blake2b_hash_ref(id);
    CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> owner(id);

    *confirmed = providerOf(implementation).confirmTransaction(copyHash(owner.get()), std::chrono::milliseconds(timeoutMs));
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider confirmation failed");
  }
}

/**
 * @brief Dispatches transaction submission.
 * @param implementation The native provider implementation.
 * @param transaction The signed transaction.
 * @param output Receives its ID.
 * @return The native status.
 */
cardano_error_t
submitTransaction(cardano_provider_impl_t* implementation, cardano_transaction_t* transaction, cardano_blake2b_hash_t** output)
{
  try
  {
    cardano_transaction_ref(transaction);
    CardanoObject<cardano_transaction_t, cardano_transaction_unref> owner(transaction);

    const Blake2bHash               id    = providerOf(implementation).submitTransaction(copyTransaction(owner.get()));
    const std::vector<std::uint8_t> bytes = id.toBytes();
    return cardano_blake2b_hash_from_bytes(bytes.data(), bytes.size(), output);
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider submission failed");
  }
}

/**
 * @brief Dispatches transaction evaluation.
 * @param implementation The native provider implementation.
 * @param transaction The transaction to evaluate.
 * @param additionalUtxos Additional resolved outputs.
 * @param output Receives redeemers with budgets.
 * @return The native status.
 */
cardano_error_t
evaluateTransaction(cardano_provider_impl_t* implementation, cardano_transaction_t* transaction, cardano_utxo_list_t* additionalUtxos, cardano_redeemer_list_t** output)
{
  try
  {
    cardano_transaction_ref(transaction);
    CardanoObject<cardano_transaction_t, cardano_transaction_unref> transactionOwner(transaction);

    const RedeemerList redeemers = providerOf(implementation).evaluateTransaction(copyTransaction(transactionOwner.get()), copyUtxos(additionalUtxos));
    *output                      = decodeCbor<cardano_redeemer_list_t, cardano_redeemer_list_unref>(redeemers.toCbor(), cardano_redeemer_list_from_cbor);
    return CARDANO_SUCCESS;
  }
  catch (const std::exception& error)
  {
    return providerError(implementation, error.what());
  }
  catch (...)
  {
    return providerError(implementation, "Provider evaluation failed");
  }
}
} // namespace

/** @brief Connects protocol-parameter copying to the C++ provider. */
class ProviderAdapter
{
  public:

    /**
     * @brief Dispatches a protocol-parameter query.
     * @param implementation The native provider implementation.
     * @param output Receives independent parameters.
     * @return The native status.
     */
    static cardano_error_t
    getParameters(cardano_provider_impl_t* implementation, cardano_protocol_parameters_t** output)
    {
      try
      {
        const ProtocolParameters parameters = providerOf(implementation).getParameters();
        return cardano_protocol_parameters_deep_clone(parameters.checked(), output);
      }
      catch (const std::exception& error)
      {
        return providerError(implementation, error.what());
      }
      catch (...)
      {
        return providerError(implementation, "Provider parameters query failed");
      }
    }
};

/* FUNCTIONS IMPLEMENTATION **************************************************/

cardano_provider_t*
createProviderAdapter(std::shared_ptr<Provider> provider)
{
  if (!provider)
  {
    throw std::invalid_argument("Provider is required");
  }

  const std::string name = provider->name();
  if (name.empty() || name.size() >= 256 || name.find('\0') != std::string::npos)
  {
    throw std::invalid_argument("Provider name must contain 1 to 255 characters without NUL");
  }

  const NetworkMagic      network = provider->networkMagic();
  ProviderContext*        context = new ProviderContext(std::move(provider));
  cardano_provider_impl_t implementation {};

  std::memcpy(implementation.name, name.data(), name.size());
  implementation.network_magic                  = static_cast<cardano_network_magic_t>(network);
  implementation.context                        = context;
  implementation.get_parameters                 = &ProviderAdapter::getParameters;
  implementation.get_unspent_outputs            = &getUnspentOutputs;
  implementation.get_rewards_balance            = &getRewardsAvailable;
  implementation.get_unspent_outputs_with_asset = &getUnspentOutputsWithAsset;
  implementation.get_unspent_output_by_nft      = &getUnspentOutputByNft;
  implementation.resolve_unspent_outputs        = &resolveUnspentOutputs;
  implementation.resolve_datum                  = &resolveDatum;
  implementation.await_transaction_confirmation = &confirmTransaction;
  implementation.post_transaction_to_chain      = &submitTransaction;
  implementation.evaluate_transaction           = &evaluateTransaction;

  cardano_provider_t*   value  = nullptr;
  const cardano_error_t status = cardano_provider_new(implementation, &value);
  if (status != CARDANO_SUCCESS)
  {
    delete context;
    checkCardanoError(status, "Provider adapter");
  }

  return value;
}
} // namespace Cometa
