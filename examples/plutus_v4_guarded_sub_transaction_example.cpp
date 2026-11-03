/**
 * @file plutus_v4_guarded_sub_transaction_example.cpp
 *
 * @brief Builds a Plutus V4 spend with a guarded sub-transaction offline.
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

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

/* CONSTANTS *****************************************************************/

namespace
{
/** @brief Always-succeeds Plutus V4 validator from the C example. */
const char* ALWAYS_SUCCEEDS_VALIDATOR_V4 = "450100002499";

/** @brief Always-succeeds Plutus V4 guard from the C example. */
const char* ALWAYS_SUCCEEDS_GUARD_V4 = "450101002499";

/** @brief Maximum count accepted for an externally supplied cost model. */
constexpr std::size_t MAX_COST_MODEL_SIZE = 512;

/** @brief Parses the network cost model. @param text Comma or space separated integers. @return The ordered costs. */
std::vector<std::int64_t>
parseCostModel(const std::string& text)
{
  std::vector<std::int64_t> costs;
  const char*               cursor = text.c_str();

  while (*cursor != '\0')
  {
    if (*cursor == '-' || (*cursor >= '0' && *cursor <= '9'))
    {
      char* end             = nullptr;
      errno                 = 0;
      const long long value = std::strtoll(cursor, &end, 10);

      if (errno != 0 || end == cursor || costs.size() == MAX_COST_MODEL_SIZE)
      {
        throw std::invalid_argument("PLUTUS_V4_COST_MODEL is invalid");
      }

      costs.push_back(static_cast<std::int64_t>(value));
      cursor = end;
    }
    else
    {
      ++cursor;
    }
  }

  if (costs.empty())
  {
    throw std::invalid_argument("PLUTUS_V4_COST_MODEL is empty");
  }

  return costs;
}

/** @brief Creates the offline Dijkstra parameters. @param costs Ordered Plutus V4 costs. @return Protocol parameters. */
Cometa::ProtocolParameters
parameters(const std::vector<std::int64_t>& costs)
{
  Cometa::CostModels models;
  models.insert(Cometa::CostModel(Cometa::PlutusLanguageVersion::V4, costs));

  Cometa::ProtocolParameters result;
  result.setMinFeeA(44);
  result.setMinFeeB(155381);
  result.setAdaPerUtxoByte(4310);
  result.setExecutionCosts(Cometa::ExUnitPrices(
    Cometa::UnitInterval::fromDouble(0.0577), Cometa::UnitInterval::fromDouble(0.0000721)));
  result.setRefScriptCostPerByte(Cometa::UnitInterval(15, 1));
  result.setMaxTxExUnits(Cometa::ExUnits(14000000, 10000000000));
  result.setCollateralPercentage(150);
  result.setMaxCollateralInputs(3);
  result.setProtocolVersion(Cometa::ProtocolVersion(12, 0));
  result.setCostModels(models);
  return result;
}

/** @brief Signs a transaction identifier. @param key Signing key. @param id Transaction identifier. @return Key witnesses. */
Cometa::VkeyWitnessSet
signId(const Cometa::Ed25519PrivateKey& key, const Cometa::Blake2bHash& id)
{
  Cometa::VkeyWitnessSet witnesses;
  witnesses.add(Cometa::VkeyWitness(key.publicKey(), key.sign(id.toBytes())));
  return witnesses;
}

/** @brief Prints all execution budgets. @param level Transaction level. @param witnesses Script witnesses. */
void
printRedeemers(const char* level, const Cometa::WitnessSet& witnesses)
{
  const std::optional<Cometa::RedeemerList> redeemers = witnesses.redeemers();

  if (!redeemers.has_value())
  {
    return;
  }

  for (std::size_t i = 0; i < redeemers->size(); ++i)
  {
    const Cometa::Redeemer redeemer = redeemers->at(i);
    const Cometa::ExUnits  units    = redeemer.exUnits();

    std::cout << level << " redeemer (tag " << static_cast<int>(redeemer.tag()) << ", index "
              << redeemer.index() << "): " << units.memory() << " memory, " << units.cpuSteps()
              << " cpu steps\n";
  }
}

/** @brief Prints a transaction and its identifier. @param name Label. @param transaction The transaction. */
void
printTransaction(const char* name, const Cometa::Transaction& transaction)
{
  Cometa::CborWriter writer;
  writer.writeEncoded(transaction.toCbor());

  std::cout << name << ": " << transaction.id().toHex() << '\n';
  std::cout << writer.toHex() << '\n';
}
} // namespace

/* MAIN **********************************************************************/

int
main(int argc, char** argv)
{
  try
  {
    const char* privateKeyHex = std::getenv("PAYMENT_PRIVATE_KEY");
    const char* costModelText = std::getenv("PLUTUS_V4_COST_MODEL");

    if (argc != 3 || privateKeyHex == nullptr || costModelText == nullptr)
    {
      std::cerr << "Usage: " << argv[0] << " <funding transaction id>#<index> <funding lovelace>\n"
                << "Set PAYMENT_PRIVATE_KEY and PLUTUS_V4_COST_MODEL in the environment.\n";
      return 1;
    }

    const std::string reference(argv[1]);
    const std::size_t separator = reference.find('#');

    if (separator != 64 || separator == reference.size() - 1)
    {
      throw std::invalid_argument("Funding UTxO must be <transaction id>#<index>");
    }

    const std::uint64_t index    = std::stoull(reference.substr(separator + 1));
    const std::uint64_t lovelace = std::stoull(argv[2]);

    const Cometa::Ed25519PrivateKey key = Cometa::Ed25519PrivateKey::fromNormalHex(privateKeyHex);
    const Cometa::Credential        paymentCredential(Cometa::CredentialType::KeyHash, key.publicKey().hash());
    const Cometa::Address           paymentAddress =
      Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, paymentCredential).toAddress();
    const Cometa::ProtocolParameters params = parameters(parseCostModel(costModelText));
    const Cometa::SlotConfig         slots  = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preprod);
    const Cometa::Utxo               fundingUtxo(
      Cometa::TransactionInput::fromHex(reference.substr(0, separator), index),
      Cometa::TransactionOutput(paymentAddress, Cometa::Value(static_cast<std::int64_t>(lovelace))));

    const Cometa::Script     validator(Cometa::PlutusV4Script::fromHex(ALWAYS_SUCCEEDS_VALIDATOR_V4));
    const Cometa::Script     guardScript(Cometa::PlutusV4Script::fromHex(ALWAYS_SUCCEEDS_GUARD_V4));
    const Cometa::Credential guard(Cometa::CredentialType::ScriptHash, guardScript.hash());
    const Cometa::Credential scriptCredential(Cometa::CredentialType::ScriptHash, validator.hash());
    const Cometa::Address    scriptAddress =
      Cometa::EnterpriseAddress(Cometa::NetworkId::Testnet, scriptCredential).toAddress();
    const Cometa::PlutusData voidData = Cometa::PlutusData::fromCbor("d87980");

    std::cout << "Payment address: " << paymentAddress.toString() << '\n';
    std::cout << "Validator address: " << scriptAddress.toString() << '\n';

    Cometa::TransactionBuilder lock(params, slots);
    lock.setUtxos(Cometa::UtxoList { fundingUtxo })
      .setChangeAddress(paymentAddress)
      .lockLovelace(scriptAddress, 5000000, Cometa::Datum(voidData))
      .sendLovelace(paymentAddress, 5000000);

    Cometa::Transaction lockTransaction = lock.build();
    lockTransaction.applyVkeyWitnesses(signId(key, lockTransaction.id()));

    const Cometa::TransactionOutputList outputs = lockTransaction.body().outputs();
    const Cometa::Utxo                  scriptUtxo(Cometa::TransactionInput(lockTransaction.id(), 0), outputs.at(0));
    const Cometa::Utxo                  subUtxo(Cometa::TransactionInput(lockTransaction.id(), 1), outputs.at(1));
    Cometa::UtxoList                    changeUtxos;

    for (std::size_t i = 2; i < outputs.size(); ++i)
    {
      changeUtxos.push_back(Cometa::Utxo(Cometa::TransactionInput(lockTransaction.id(), i), outputs.at(i)));
    }

    Cometa::SubTransactionBuilder sub(params, slots);
    sub.addInput(subUtxo)
      .sendLovelace(paymentAddress, 5000000)
      .addScript(guardScript)
      .addGuard(guard, voidData);

    Cometa::SubTransaction subTransaction = sub.build();
    subTransaction.applyVkeyWitnesses(signId(key, subTransaction.id()));

    Cometa::TransactionBuilder spend(params, slots);
    spend.setUtxos(changeUtxos)
      .setCollateralUtxos(changeUtxos)
      .setCollateralChangeAddress(paymentAddress)
      .setChangeAddress(paymentAddress)
      .addScript(validator)
      .addInput(scriptUtxo, voidData)
      .addSubTransaction(subTransaction, Cometa::UtxoList { subUtxo });

    Cometa::Transaction spendTransaction = spend.build();
    spendTransaction.applyVkeyWitnesses(signId(key, spendTransaction.id()));

    printRedeemers("Top level", spendTransaction.witnessSet());
    printRedeemers("Sub transaction", subTransaction.witnessSet());
    printTransaction("Lock transaction id", lockTransaction);
    printTransaction("Spend transaction id", spendTransaction);
    return 0;
  }
  catch (const std::exception& error)
  {
    std::cerr << "Plutus V4 guarded sub-transaction example failed: " << error.what() << '\n';
    return 1;
  }
}
