/**
 * @file sub_transaction_batch_example.cpp
 *
 * @brief Settles two signed trade intents in a guarded batch.
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
#include <Cometa/TransactionBuilder/SubTransactionBuilder.h>
#include <Cometa/TransactionBuilder/TransactionBuilder.h>

#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Token from the matching cardano-c example. */
const char* TOKEN_ASSET_ID = "1ec85dcee27f2d90ec1f9a1e4ce74a667dc9be8b184463223f9c960154534c41";

/** @brief Buyer signing seed. */
const char* BUYER_PRIVATE_KEY = "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60";

/** @brief Seller signing seed. */
const char* SELLER_PRIVATE_KEY = "4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb";

/** @brief Batcher signing seed. */
const char* BATCHER_PRIVATE_KEY = "c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7";

/** @brief Returns the example passphrase. @return The passphrase bytes. */
std::vector<std::uint8_t>
passphrase()
{
  return { 'p', 'a', 's', 's', 'w', 'o', 'r', 'd' };
}

/** @brief Creates an amount with the traded token. @param lovelace The coin amount. @param tokens The token amount. @return The value. */
Cometa::Value
makeValue(std::int64_t lovelace, std::int64_t tokens)
{
  Cometa::Value value(lovelace);

  if (tokens != 0)
  {
    value.addAsset(Cometa::AssetId::fromHex(TOKEN_ASSET_ID), tokens);
  }

  return value;
}

/** @brief One trading party and its opening output. */
struct Party
{
    /** @brief Encrypted signing key. */
    Cometa::SecureKeyHandler signer;

    /** @brief Key credential. */
    Cometa::Credential credential;

    /** @brief Payment address. */
    Cometa::Address address;

    /** @brief Opening output. */
    Cometa::Utxo utxo;

    /** @brief Opening outputs for builder resolution. */
    Cometa::UtxoList utxos;

    /** @brief Creates a party. @param key The private signing seed. @param txId The opening transaction ID. @param lovelace Opening coin. @param tokens Opening tokens. */
    Party(const char* key, const char* txId, std::int64_t lovelace, std::int64_t tokens)
    : signer(Cometa::SecureKeyHandler::softwareEd25519(
        Cometa::Ed25519PrivateKey::fromNormalHex(key),
        passphrase(),
        passphrase))
    , credential(Cometa::CredentialType::KeyHash, signer.publicKey().hash())
    , address(Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, credential).toAddress())
    , utxo(Cometa::TransactionInput::fromHex(txId, 0), Cometa::TransactionOutput(address, makeValue(lovelace, tokens)))
    , utxos { utxo }
    {
    }
};

/** @brief Creates the offline protocol parameters. @return The parameters. */
Cometa::ProtocolParameters
parameters()
{
  Cometa::ProtocolParameters result;
  result.setMinFeeA(44);
  result.setMinFeeB(155381);
  result.setAdaPerUtxoByte(4310);
  result.setExecutionCosts(Cometa::ExUnitPrices(
    Cometa::UnitInterval::fromDouble(0.0577), Cometa::UnitInterval::fromDouble(0.0000721)));
  result.setRefScriptCostPerByte(Cometa::UnitInterval(15, 1));
  return result;
}

/** @brief Builds a signed trade intent. @param params Protocol parameters. @param party The signer. @param batcher The trusted guard. @param lovelace Desired coin output. @param tokens Desired token output. @return The signed intent. */
Cometa::SubTransaction
buildIntent(
  const Cometa::ProtocolParameters& params,
  const Party&                      party,
  const Party&                      batcher,
  std::int64_t                      lovelace,
  std::int64_t                      tokens)
{
  Cometa::SubTransactionBuilder builder(params, Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));

  builder.addInput(party.utxo)
    .sendValue(party.address, makeValue(lovelace, tokens))
    .requireTopLevelGuard(batcher.credential);

  Cometa::SubTransaction intent = builder.build();
  intent.applyVkeyWitnesses(party.signer.signSubTransaction(intent));
  return intent;
}
} // namespace

/* MAIN **********************************************************************/

int
main()
{
  try
  {
    const Cometa::ProtocolParameters params = parameters();

    const Party buyer(
      BUYER_PRIVATE_KEY,
      "0000000000000000000000000000000000000000000000000000000000000001",
      100'000'000,
      0);
    const Party seller(
      SELLER_PRIVATE_KEY,
      "0000000000000000000000000000000000000000000000000000000000000002",
      10'000'000,
      5000);
    const Party batcher(
      BATCHER_PRIVATE_KEY,
      "0000000000000000000000000000000000000000000000000000000000000003",
      20'000'000,
      0);

    const Cometa::SubTransaction buyerIntent  = buildIntent(params, buyer, batcher, 50'000'000, 1000);
    const Cometa::SubTransaction sellerIntent = buildIntent(params, seller, batcher, 60'000'000, 4000);

    Cometa::TransactionBuilder builder(params, Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod));

    builder.setUtxos(batcher.utxos)
      .setChangeAddress(batcher.address)
      .addSubTransaction(buyerIntent, buyer.utxos)
      .addSubTransaction(sellerIntent, seller.utxos)
      .addGuard(batcher.credential);

    Cometa::Transaction transaction = builder.build();
    transaction.applyVkeyWitnesses(batcher.signer.signTransaction(transaction));

    std::cout << "Buyer intent: " << buyerIntent.id().toHex() << '\n';
    std::cout << "Seller intent: " << sellerIntent.id().toHex() << '\n';
    std::cout << "Batch transaction: " << transaction.id().toHex() << '\n';
    std::cout << "Batch fee: " << transaction.body().fee() << " lovelace\n";

    Cometa::CborWriter writer;
    writer.writeEncoded(transaction.toCbor());

    std::cout << "Transaction CBOR: " << writer.toHex() << '\n';

    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Sub-transaction batch example failed: " << error.what() << '\n';
    return 1;
  }
}
