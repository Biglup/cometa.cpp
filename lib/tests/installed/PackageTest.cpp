/**
 * @file PackageTest.cpp
 *
 * @brief Verifies the installed package headers and static libraries.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <string>

/* IMPLEMENTATION ************************************************************/

int
main()
{
  const Cometa::AssetName name = Cometa::AssetName::fromCbor("46436f6d657461");

  const Cometa::ExUnits         budget = Cometa::ExUnits::fromCbor("820102");
  const Cometa::ProtocolVersion version(12, 0);
  const Cometa::SlotConfig      slots = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preview);

  const Cometa::UnitInterval       interval = Cometa::UnitInterval::fromDouble(0.75);
  const Cometa::Blake2bHash        hash     = Cometa::Blake2bHash::fromHex(std::string(64, '0'));
  const Cometa::Anchor             anchor("https://example.com", hash);
  const Cometa::GovernanceActionId action(hash, 1);

  if (interval.numerator() != 3 || anchor.hash() != hash || Cometa::GovernanceActionId::fromBech32(action.toBech32()) != action)
  {
    return 1;
  }

  const Cometa::BigInt integer = Cometa::BigInt(2).pow(128);

  if (integer.toString() != "340282366920938463463374607431768211456")
  {
    return 1;
  }

  const Cometa::Bech32Data decoded = Cometa::decodeBech32("a12uel5l");

  if (Cometa::encodeBase58({ 58 }) != "21" || decoded.prefix != "a" || !decoded.bytes.empty() || Cometa::encodeBech32(decoded.prefix, decoded.bytes) != "a12uel5l")
  {
    return 1;
  }

  Cometa::CborWriter writer;
  writer.writeBigInt(integer);
  Cometa::CborReader reader(writer.toBytes());

  if (reader.readBigInt() != integer || reader.bytesRemaining() != 0)
  {
    return 1;
  }

  Cometa::JsonWriter json;
  json.writeStartObject();
  json.writePropertyName("amount");
  json.writeBigInt(integer);
  json.writeEndObject();
  const Cometa::JsonValue parsed = Cometa::JsonValue::parse(json.toString());

  if (parsed.at("amount").toJson() != integer.toString())
  {
    return 1;
  }

  const Cometa::Bip32PrivateKey   root      = Cometa::Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0));
  const Cometa::Ed25519PrivateKey key       = root.derive({ Cometa::hardenIndex(1852), 0 }).toEd25519();
  const Cometa::Ed25519Signature  signature = key.sign({ 1, 2, 3 });

  if (!key.publicKey().verify(signature, { 1, 2, 3 }))
  {
    return 1;
  }

  const std::vector<std::uint8_t> entropy(16, 0);
  const std::vector<std::string>  words     = Cometa::entropyToMnemonic(entropy);
  const std::vector<std::uint8_t> encrypted = Cometa::encryptEmip3(entropy, "password");

  if (Cometa::mnemonicToEntropy(words) != entropy || Cometa::decryptEmip3(encrypted, "password") != entropy || Cometa::crc32({}) != 0)
  {
    return 1;
  }

  const Cometa::Credential        payment(Cometa::CredentialType::KeyHash, key.publicKey().hash());
  const Cometa::EnterpriseAddress paymentAddress(Cometa::NetworkId::Testnet, payment);
  const Cometa::Address           parsedAddress = Cometa::Address::fromString(paymentAddress.toString());

  if (parsedAddress.asEnterprise() != paymentAddress || parsedAddress.asBase().has_value())
  {
    return 1;
  }

  const Cometa::AssetId     token(key.publicKey().hash(), name);
  const Cometa::AssetIdList assets { Cometa::AssetId::lovelace(), token };

  if (Cometa::AssetId::fromHex(token.toHex()) != assets.back() || !assets.front().isLovelace())
  {
    return 1;
  }

  Cometa::MultiAsset tokens;
  tokens.set(token, 5);
  Cometa::Value paymentValue(2'000'000, tokens);
  paymentValue.addAsset(token, 2);
  const Cometa::Value parsedValue = Cometa::Value::fromCbor(paymentValue.toCbor());

  if (parsedValue != paymentValue || parsedValue.toAssetMap().at(token) != 7)
  {
    return 1;
  }

  const Cometa::CredentialSet  credentials { payment };
  const Cometa::Blake2bHashSet hashes { key.publicKey().hash() };

  if (Cometa::CredentialSet::fromCbor(credentials.toCbor()) != credentials || !hashes.contains(key.publicKey().hash()))
  {
    return 1;
  }

  Cometa::PlutusList fields;
  fields.pushBack(Cometa::PlutusData(42));
  const Cometa::Datum datum(Cometa::PlutusData(Cometa::ConstrPlutusData(0, fields)));

  if (Cometa::Datum::fromCbor(datum.toCbor()) != datum || datum.inlineData()->asConstructor().fields().at(0).asInteger() != Cometa::BigInt(42))
  {
    return 1;
  }

  const Cometa::ScriptPubkey       signatureRequirement(key.publicKey().hash());
  const Cometa::ScriptRequireGuard guard(payment);
  Cometa::ScriptInvalidBefore      starts(100);
  starts.setSlot(200);

  if (Cometa::ScriptPubkey::fromCbor(signatureRequirement.toCbor()) != signatureRequirement || Cometa::ScriptRequireGuard::fromCbor(guard.toCbor()) != guard || starts.slot() != 200)
  {
    return 1;
  }

  const Cometa::NativeScriptList requirements {
    Cometa::NativeScript(signatureRequirement), Cometa::NativeScript(starts)
  };
  const Cometa::NativeScript policy(Cometa::ScriptNOfK(1, requirements));

  if (Cometa::NativeScript::fromCbor(policy.toCbor()).hash() != policy.hash() || policy.asNOfK().required() != 1)
  {
    return 1;
  }

  const Cometa::PlutusV4Script plutus = Cometa::PlutusV4Script::fromHex("450100002499");
  Cometa::PlutusList           parameters;
  parameters.pushBack(Cometa::PlutusData(std::int64_t { 0 }));
  const Cometa::PlutusV4Script applied = plutus.applyParameters(parameters);

  if (applied.toHex() != "4b0100003249930101000001" || applied.languageVersion() != Cometa::PlutusLanguageVersion::V4)
  {
    return 1;
  }

  const Cometa::Script script(applied);

  if (script.language() != Cometa::ScriptLanguage::PlutusV4 || script.asPlutusV4() != applied || script.hash() != applied.hash())
  {
    return 1;
  }

  const Cometa::TransactionInput input(Cometa::Blake2bHash::fromHex(std::string(64, '1')), 5);
  Cometa::TransactionOutput      output(parsedAddress, Cometa::Value(2'000'000));
  output.setScriptRef(script);
  const Cometa::Utxo                  utxo(input, output);
  const Cometa::TransactionInputSet   inputs { input };
  const Cometa::TransactionOutputList outputs { output };
  Cometa::WithdrawalMap               withdrawals;
  withdrawals.set("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s", 10);

  if (Cometa::Utxo::fromCbor(utxo.toCbor()) != utxo || inputs.size() != 1 || outputs.size() != 1 || withdrawals.size() != 1)
  {
    return 1;
  }

  const Cometa::PlutusV4ScriptList auxiliaryScripts { Cometa::PlutusV4Script::fromHex("450100002499") };

  if (Cometa::PlutusV4ScriptList::fromCbor(auxiliaryScripts.toCbor()) != auxiliaryScripts)
  {
    return 1;
  }

  const Cometa::Metadatum metadata = Cometa::Metadatum::fromPlainJson(R"({"name":"Cometa","count":42})");

  if (Cometa::Metadatum::fromCbor(metadata.toCbor()) != metadata)
  {
    return 1;
  }

  Cometa::TransactionMetadata transactionMetadata;
  transactionMetadata.set(674, metadata);
  Cometa::AuxiliaryData auxiliary;
  auxiliary.setMetadata(transactionMetadata);
  auxiliary.setPlutusV4Scripts(auxiliaryScripts);

  if (Cometa::AuxiliaryData::fromCbor(auxiliary.toCbor()) != auxiliary || auxiliary.hash().toBytes().size() != 32U)
  {
    return 1;
  }

  const Cometa::Ipv4   poolAddress   = Cometa::Ipv4::fromString("192.0.2.1");
  const Cometa::Ipv6   poolAddressV6 = Cometa::Ipv6::fromString("0000:0000:0000:0000:0000:0000:0000:0001");
  const Cometa::BlsKey poolKey(std::vector<std::uint8_t>(96, 1), std::vector<std::uint8_t>(48, 2));
  Cometa::PoolMetadata poolMetadata("https://example.com/pool.json", Cometa::Blake2bHash::fromHex(std::string(64, '1')));
  poolMetadata.setUrl("abc");

  if (Cometa::Ipv4::fromCbor(poolAddress.toCbor()) != poolAddress || Cometa::Ipv6::fromCbor(poolAddressV6.toCbor()) != poolAddressV6 || Cometa::BlsKey::fromCbor(poolKey.toCbor()) != poolKey || Cometa::PoolMetadata::fromCbor(poolMetadata.toCbor()).url() != "abc")
  {
    return 1;
  }

  Cometa::SingleHostAddrRelay ipRelay(3001, poolAddress, poolAddressV6);
  ipRelay.setIpv4(std::nullopt);
  const Cometa::Relays poolRelays {
    Cometa::Relay(ipRelay),
    Cometa::Relay(Cometa::SingleHostNameRelay("example.com", 3001)),
    Cometa::Relay(Cometa::MultiHostNameRelay("example.com"))
  };

  if (Cometa::Relays::fromCbor(poolRelays.toCbor()) != poolRelays || poolRelays.at(1).type() != Cometa::RelayType::SingleHostName || Cometa::toString(Cometa::RelayType::SingleHostAddress).empty())
  {
    return 1;
  }

  const Cometa::PoolOwners poolOwners { Cometa::Blake2bHash::fromHex(std::string(56, '1')) };

  if (Cometa::PoolOwners::fromCbor(poolOwners.toCbor()) != poolOwners || poolOwners.size() != 1)
  {
    return 1;
  }

  const Cometa::Blake2bHash   poolOperator = Cometa::Blake2bHash::fromHex(std::string(56, '1'));
  const Cometa::RewardAddress poolReward(Cometa::NetworkId::Testnet, Cometa::Credential(Cometa::CredentialType::KeyHash, poolOperator));
  const Cometa::PoolParams    poolParams(poolOperator, hash, 10'000'000, 340'000, Cometa::UnitInterval(1, 20), poolReward, poolOwners, poolRelays);

  if (Cometa::PoolParams::fromCbor(poolParams.toCbor()) != poolParams)
  {
    return 1;
  }

  if (Cometa::toString(Cometa::CertificateType::PoolRegistration).empty())
  {
    return 1;
  }

  const Cometa::StakeRegistrationCert stakeCert(payment);

  if (Cometa::StakeRegistrationCert::fromCbor(stakeCert.toCbor()) != stakeCert)
  {
    return 1;
  }

  return name.toString() == "Cometa" && budget.cpuSteps() == 2 && version.major() == 12 &&
      slots.unixTimeFromSlot(0) == 1666656000ULL
    ? 0
    : 1;
}
