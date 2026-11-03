/**
 * @file ProviderAdapterTest.cpp
 *
 * @brief Tests native dispatch to C++ providers.
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

#include <Cometa/Cometa.h>
#include <Cometa/TransactionBuilder/ProviderTransactionEvaluator.h>

#include "../../src/CardanoInterop.h"
#include "../../src/Providers/ProviderAdapter.h"

#include <cardano/address/address.h>
#include <cardano/address/reward_address.h>
#include <cardano/assets/asset_id.h>
#include <cardano/common/utxo.h>
#include <cardano/common/utxo_list.h>
#include <cardano/crypto/blake2b_hash.h>
#include <cardano/plutus_data/plutus_data.h>
#include <cardano/protocol_params/protocol_parameters.h>
#include <cardano/providers/provider.h>
#include <cardano/transaction/transaction.h>
#include <cardano/transaction_body/transaction_input_set.h>
#include <cardano/witness_set/redeemer_list.h>

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Creates an output for provider callback round trips. @return The output. */
Utxo
sampleUtxo()
{
  const Blake2bHash id(std::vector<std::uint8_t>(32, 2));
  const Credential  credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address     address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  return Utxo(TransactionInput(id, 3), TransactionOutput(address, Value(2000000)));
}

/** @brief Supplies deterministic network responses to the native bridge. */
class TestProvider : public Provider
{
  public:

    /** @brief Selects whether UTxO queries fail. */
    bool failUtxos = false;

    /** @brief Gets the provider name. @return The name. */
    std::string
    name() const override
    {
      return "test-provider";
    }

    /** @brief Gets the test network. @return The network magic. */
    NetworkMagic
    networkMagic() const override
    {
      return NetworkMagic::Preprod;
    }

    /** @brief Gets test protocol parameters. @return The parameters. */
    ProtocolParameters
    getParameters() const override
    {
      ProtocolParameters parameters;
      parameters.setMinFeeA(44);
      return parameters;
    }

    /** @brief Gets test outputs. @param address The address. @return An empty list. */
    UtxoList
    getUnspentOutputs(const Address& address) const override
    {
      if (address.toBytes().empty() || failUtxos)
      {
        throw std::runtime_error("UTxO lookup failed");
      }

      return {};
    }

    /** @brief Gets test rewards. @param address The account. @return Available lovelace. */
    std::uint64_t
    getRewardsAvailable(const RewardAddress& address) const override
    {
      return address.toBytes().empty() ? 0 : 42;
    }

    /** @brief Gets a matching output. @param address The address. @param asset The asset. @return The output. */
    UtxoList
    getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const override
    {
      if (address.toBytes().empty() || asset.isLovelace())
      {
        throw std::invalid_argument("Expected a native token query");
      }

      return { sampleUtxo() };
    }

    /** @brief Gets an NFT output. @param asset The asset. @return The output. */
    Utxo
    getUnspentOutputByNft(const AssetId& asset) const override
    {
      if (asset.isLovelace())
      {
        throw std::invalid_argument("Expected an NFT query");
      }

      return sampleUtxo();
    }

    /** @brief Resolves an input. @param inputs The inputs. @return The output. */
    UtxoList
    resolveUnspentOutputs(const TransactionInputSet& inputs) const override
    {
      if (inputs.size() != 1)
      {
        throw std::invalid_argument("Expected one input");
      }

      return { sampleUtxo() };
    }

    /** @brief Resolves a datum. @param hash The hash. @return The datum. */
    PlutusData
    resolveDatum(const Blake2bHash& hash) const override
    {
      if (hash.toBytes().size() != 32)
      {
        throw std::invalid_argument("Expected a 32-byte datum hash");
      }

      return PlutusData(42);
    }

    /** @brief Confirms a test ID. @param id The ID. @param timeout The wait. @return True for the test wait. */
    bool
    confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const override
    {
      return id.toBytes().size() == 32 && timeout == std::chrono::milliseconds(100);
    }

    /** @brief Returns the submitted ID. @param transaction The transaction. @return Its ID. */
    Blake2bHash
    submitTransaction(const Transaction& transaction) const override
    {
      return transaction.id();
    }

    /** @brief Evaluates a transaction. @param transaction The transaction. @param additionalUtxos Extra inputs. @return No redeemers. */
    RedeemerList
    evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const override
    {
      if (transaction.body().fee() != 10 || !additionalUtxos.empty())
      {
        throw std::invalid_argument("Unexpected evaluation request");
      }

      return RedeemerList();
    }
};
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(ProviderAdapter, DispatchesParametersUtxosAndRewards)
{
  // Arrange
  const std::shared_ptr<TestProvider>                       provider = std::make_shared<TestProvider>();
  CardanoObject<cardano_provider_t, cardano_provider_unref> native(createProviderAdapter(provider));
  const Credential                                          credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address                                             address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  const RewardAddress                                       reward(NetworkId::Testnet, credential);

  cardano_address_t*              addressValue = nullptr;
  const std::vector<std::uint8_t> addressBytes = address.toBytes();
  ASSERT_EQ(cardano_address_from_bytes(addressBytes.data(), addressBytes.size(), &addressValue), CARDANO_SUCCESS);
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(addressValue);

  cardano_reward_address_t*       rewardValue = nullptr;
  const std::vector<std::uint8_t> rewardBytes = reward.toBytes();
  ASSERT_EQ(cardano_reward_address_from_bytes(rewardBytes.data(), rewardBytes.size(), &rewardValue), CARDANO_SUCCESS);
  CardanoObject<cardano_reward_address_t, cardano_reward_address_unref> nativeReward(rewardValue);

  // Act
  cardano_protocol_parameters_t*                                                  parameters      = nullptr;
  cardano_utxo_list_t*                                                            outputs         = nullptr;
  std::uint64_t                                                                   available       = 0;
  const cardano_error_t                                                           parameterStatus = cardano_provider_get_parameters(native.get(), &parameters);
  const cardano_error_t                                                           outputStatus    = cardano_provider_get_unspent_outputs(native.get(), nativeAddress.get(), &outputs);
  const cardano_error_t                                                           rewardStatus    = cardano_provider_get_rewards_available(native.get(), nativeReward.get(), &available);
  CardanoObject<cardano_protocol_parameters_t, cardano_protocol_parameters_unref> parameterOwner(parameters);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>                     outputOwner(outputs);

  // Assert
  EXPECT_STREQ(cardano_provider_get_name(native.get()), "test-provider");
  EXPECT_EQ(cardano_provider_get_network_magic(native.get()), CARDANO_NETWORK_MAGIC_PREPROD);
  EXPECT_EQ(parameterStatus, CARDANO_SUCCESS);
  EXPECT_EQ(cardano_protocol_parameters_get_min_fee_a(parameterOwner.get()), 44U);
  EXPECT_EQ(outputStatus, CARDANO_SUCCESS);
  EXPECT_EQ(cardano_utxo_list_get_length(outputOwner.get()), 0U);
  EXPECT_EQ(rewardStatus, CARDANO_SUCCESS);
  EXPECT_EQ(available, 42U);
}

TEST(ProviderAdapter, KeepsProviderAliveAndReportsCallbackErrors)
{
  // Arrange
  std::shared_ptr<TestProvider> provider = std::make_shared<TestProvider>();
  provider->failUtxos                    = true;
  const std::weak_ptr<TestProvider>                         observer(provider);
  CardanoObject<cardano_provider_t, cardano_provider_unref> native(createProviderAdapter(provider));
  provider.reset();

  const Credential                credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address                   address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  const std::vector<std::uint8_t> bytes   = address.toBytes();
  cardano_address_t*              value   = nullptr;
  ASSERT_EQ(cardano_address_from_bytes(bytes.data(), bytes.size(), &value), CARDANO_SUCCESS);
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(value);

  // Act
  cardano_utxo_list_t*  outputs = nullptr;
  const cardano_error_t status  = cardano_provider_get_unspent_outputs(native.get(), nativeAddress.get(), &outputs);
  const std::string     message = cardano_provider_get_last_error(native.get());
  native.reset();

  // Assert
  EXPECT_EQ(status, CARDANO_ERROR_GENERIC);
  EXPECT_EQ(message, "UTxO lookup failed");
  EXPECT_EQ(outputs, nullptr);
  EXPECT_TRUE(observer.expired());
}

TEST(ProviderAdapter, DispatchesAssetAndNftQueries)
{
  // Arrange
  const std::shared_ptr<TestProvider>                       provider = std::make_shared<TestProvider>();
  CardanoObject<cardano_provider_t, cardano_provider_unref> native(createProviderAdapter(provider));
  const Address                                             address = sampleUtxo().output().address();
  const AssetId                                             asset(std::vector<std::uint8_t>(29, 1));

  const std::vector<std::uint8_t> addressBytes = address.toBytes();
  cardano_address_t*              addressValue = nullptr;
  ASSERT_EQ(cardano_address_from_bytes(addressBytes.data(), addressBytes.size(), &addressValue), CARDANO_SUCCESS);
  CardanoObject<cardano_address_t, cardano_address_unref> nativeAddress(addressValue);

  const std::vector<std::uint8_t> assetBytes = asset.toBytes();
  cardano_asset_id_t*             assetValue = nullptr;
  ASSERT_EQ(cardano_asset_id_from_bytes(assetBytes.data(), assetBytes.size(), &assetValue), CARDANO_SUCCESS);
  CardanoObject<cardano_asset_id_t, cardano_asset_id_unref> nativeAsset(assetValue);

  // Act
  cardano_utxo_list_t*                                        outputs    = nullptr;
  cardano_utxo_t*                                             nft        = nullptr;
  const cardano_error_t                                       listStatus = cardano_provider_get_unspent_outputs_with_asset(native.get(), nativeAddress.get(), nativeAsset.get(), &outputs);
  const cardano_error_t                                       nftStatus  = cardano_provider_get_unspent_output_by_nft(native.get(), nativeAsset.get(), &nft);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref> outputOwner(outputs);
  CardanoObject<cardano_utxo_t, cardano_utxo_unref>           nftOwner(nft);

  // Assert
  ASSERT_EQ(listStatus, CARDANO_SUCCESS);
  ASSERT_EQ(nftStatus, CARDANO_SUCCESS);
  EXPECT_EQ(cardano_utxo_list_get_length(outputOwner.get()), 1U);
  EXPECT_EQ(Utxo::fromCbor(encodeCbor(nftOwner.get(), cardano_utxo_to_cbor)), sampleUtxo());
}

TEST(ProviderAdapter, ResolvesInputsAndDatums)
{
  // Arrange
  const std::shared_ptr<TestProvider>                       provider = std::make_shared<TestProvider>();
  CardanoObject<cardano_provider_t, cardano_provider_unref> native(createProviderAdapter(provider));
  TransactionInputSet                                       inputs;
  inputs.insert(sampleUtxo().input());

  CardanoObject<cardano_transaction_input_set_t, cardano_transaction_input_set_unref> nativeInputs(
    decodeCbor<cardano_transaction_input_set_t, cardano_transaction_input_set_unref>(inputs.toCbor(), cardano_transaction_input_set_from_cbor));
  const Blake2bHash               hash(std::vector<std::uint8_t>(32, 3));
  const std::vector<std::uint8_t> hashBytes = hash.toBytes();
  cardano_blake2b_hash_t*         hashValue = nullptr;
  ASSERT_EQ(cardano_blake2b_hash_from_bytes(hashBytes.data(), hashBytes.size(), &hashValue), CARDANO_SUCCESS);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> nativeHash(hashValue);

  // Act
  cardano_utxo_list_t*                                            outputs      = nullptr;
  cardano_plutus_data_t*                                          datum        = nullptr;
  const cardano_error_t                                           outputStatus = cardano_provider_resolve_unspent_outputs(native.get(), nativeInputs.get(), &outputs);
  const cardano_error_t                                           datumStatus  = cardano_provider_resolve_datum(native.get(), nativeHash.get(), &datum);
  CardanoObject<cardano_utxo_list_t, cardano_utxo_list_unref>     outputOwner(outputs);
  CardanoObject<cardano_plutus_data_t, cardano_plutus_data_unref> datumOwner(datum);

  // Assert
  ASSERT_EQ(outputStatus, CARDANO_SUCCESS);
  ASSERT_EQ(datumStatus, CARDANO_SUCCESS);
  EXPECT_EQ(cardano_utxo_list_get_length(outputOwner.get()), 1U);
  EXPECT_EQ(PlutusData::fromCbor(encodeCbor(datumOwner.get(), cardano_plutus_data_to_cbor)), PlutusData(42));
}

TEST(ProviderAdapter, DispatchesConfirmationSubmissionAndEvaluation)
{
  // Arrange
  const std::shared_ptr<TestProvider>                             provider = std::make_shared<TestProvider>();
  CardanoObject<cardano_provider_t, cardano_provider_unref>       native(createProviderAdapter(provider));
  const Transaction                                               transaction(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> nativeTransaction(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(transaction.toCbor(), cardano_transaction_from_cbor));

  const std::vector<std::uint8_t> idBytes = transaction.id().toBytes();
  cardano_blake2b_hash_t*         idValue = nullptr;
  ASSERT_EQ(cardano_blake2b_hash_from_bytes(idBytes.data(), idBytes.size(), &idValue), CARDANO_SUCCESS);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref> nativeId(idValue);

  // Act
  bool                                                                confirmed      = false;
  cardano_blake2b_hash_t*                                             submittedId    = nullptr;
  cardano_redeemer_list_t*                                            redeemers      = nullptr;
  const cardano_error_t                                               confirmStatus  = cardano_provider_confirm_transaction(native.get(), nativeId.get(), 100, &confirmed);
  const cardano_error_t                                               submitStatus   = cardano_provider_submit_transaction(native.get(), nativeTransaction.get(), &submittedId);
  const cardano_error_t                                               evaluateStatus = cardano_provider_evaluate_transaction(native.get(), nativeTransaction.get(), nullptr, &redeemers);
  CardanoObject<cardano_blake2b_hash_t, cardano_blake2b_hash_unref>   submittedOwner(submittedId);
  CardanoObject<cardano_redeemer_list_t, cardano_redeemer_list_unref> redeemerOwner(redeemers);

  // Assert
  ASSERT_EQ(confirmStatus, CARDANO_SUCCESS);
  ASSERT_EQ(submitStatus, CARDANO_SUCCESS);
  ASSERT_EQ(evaluateStatus, CARDANO_SUCCESS);
  EXPECT_TRUE(confirmed);
  EXPECT_EQ(Blake2bHash(std::vector<std::uint8_t>(cardano_blake2b_hash_get_data(submittedOwner.get()), cardano_blake2b_hash_get_data(submittedOwner.get()) + cardano_blake2b_hash_get_bytes_size(submittedOwner.get()))), transaction.id());
  EXPECT_EQ(cardano_redeemer_list_get_length(redeemerOwner.get()), 0U);
}

TEST(ProviderAdapter, EvaluatesThroughProviderStrategy)
{
  // Arrange
  const std::shared_ptr<TestProvider> provider = std::make_shared<TestProvider>();
  const ProviderTransactionEvaluator  evaluator(provider);
  const Transaction                   transaction(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());

  // Act
  const RedeemerList result = evaluator.evaluate(transaction, {});

  // Assert
  EXPECT_EQ(evaluator.name(), "test-provider");
  EXPECT_EQ(result.size(), 0U);
}
