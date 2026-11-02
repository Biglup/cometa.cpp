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

  const Cometa::StakeDeregistrationCert stakeDeregistration(payment);

  if (Cometa::StakeDeregistrationCert::fromCbor(stakeDeregistration.toCbor()) != stakeDeregistration)
  {
    return 1;
  }

  const Cometa::RegistrationCert   registration(payment, 2'000'000);
  const Cometa::UnregistrationCert unregistration(payment, 2'000'000);

  if (Cometa::RegistrationCert::fromCbor(registration.toCbor()) != registration || Cometa::UnregistrationCert::fromCbor(unregistration.toCbor()) != unregistration)
  {
    return 1;
  }

  const Cometa::StakeDelegationCert delegation(payment, poolOperator);

  if (Cometa::StakeDelegationCert::fromCbor(delegation.toCbor()) != delegation)
  {
    return 1;
  }

  const Cometa::PoolRegistrationCert poolRegistration(poolParams);

  if (Cometa::PoolRegistrationCert::fromCbor(poolRegistration.toCbor()) != poolRegistration)
  {
    return 1;
  }

  const Cometa::PoolRetirementCert poolRetirement(poolOperator, 1000);

  if (Cometa::PoolRetirementCert::fromCbor(poolRetirement.toCbor()) != poolRetirement)
  {
    return 1;
  }

  const Cometa::StakeRegistrationDelegationCert combinedRegistration(payment, poolOperator, 2'000'000);

  if (Cometa::StakeRegistrationDelegationCert::fromCbor(combinedRegistration.toCbor()) != combinedRegistration)
  {
    return 1;
  }

  const Cometa::GenesisKeyDelegationCert genesisDelegation(poolOperator, poolOperator, hash);

  if (Cometa::GenesisKeyDelegationCert::fromCbor(genesisDelegation.toCbor()) != genesisDelegation)
  {
    return 1;
  }

  const Cometa::MirToPotCert mir(Cometa::MirPotType::Reserve, 1'000'000);

  if (Cometa::MirToPotCert::fromCbor(mir.toCbor()) != mir)
  {
    return 1;
  }

  Cometa::MirToStakeCredentialsCert mirRewards(Cometa::MirPotType::Reserve);
  mirRewards.insert(payment, 1'000'000);

  if (Cometa::MirToStakeCredentialsCert::fromCbor(mirRewards.toCbor()) != mirRewards)
  {
    return 1;
  }

  const Cometa::MirCert mirVariant(mirRewards);

  if (Cometa::MirCert::fromCbor(mirVariant.toCbor()) != mirVariant)
  {
    return 1;
  }

  const Cometa::DRep representative(payment);

  if (Cometa::DRep::fromCbor(representative.toCbor()) != representative)
  {
    return 1;
  }

  const Cometa::VoteDelegationCert voteDelegation(payment, representative);

  if (Cometa::VoteDelegationCert::fromCbor(voteDelegation.toCbor()) != voteDelegation)
  {
    return 1;
  }

  const Cometa::StakeVoteDelegationCert combinedDelegation(payment, poolOperator, representative);

  if (Cometa::StakeVoteDelegationCert::fromCbor(combinedDelegation.toCbor()) != combinedDelegation)
  {
    return 1;
  }

  const Cometa::VoteRegistrationDelegationCert voteRegistration(payment, 2'000'000, representative);

  if (Cometa::VoteRegistrationDelegationCert::fromCbor(voteRegistration.toCbor()) != voteRegistration)
  {
    return 1;
  }

  const Cometa::StakeVoteRegistrationDelegationCert combinedRegistrationDelegation(payment, poolOperator, representative, 2'000'000);

  if (Cometa::StakeVoteRegistrationDelegationCert::fromCbor(combinedRegistrationDelegation.toCbor()) != combinedRegistrationDelegation)
  {
    return 1;
  }

  const Cometa::UnregisterDRepCert unregisterDRep(payment, 2'000'000);

  if (Cometa::UnregisterDRepCert::fromCbor(unregisterDRep.toCbor()) != unregisterDRep)
  {
    return 1;
  }

  const Cometa::RegisterDRepCert registerDRep(payment, 2'000'000);

  if (Cometa::RegisterDRepCert::fromCbor(registerDRep.toCbor()) != registerDRep)
  {
    return 1;
  }

  const Cometa::UpdateDRepCert updateDRep(payment);

  if (Cometa::UpdateDRepCert::fromCbor(updateDRep.toCbor()) != updateDRep)
  {
    return 1;
  }

  const Cometa::AuthCommitteeHotCert committeeAuthorization(payment, payment);

  if (Cometa::AuthCommitteeHotCert::fromCbor(committeeAuthorization.toCbor()) != committeeAuthorization)
  {
    return 1;
  }

  const Cometa::ResignCommitteeColdCert committeeResignation(payment);

  if (Cometa::ResignCommitteeColdCert::fromCbor(committeeResignation.toCbor()) != committeeResignation)
  {
    return 1;
  }

  const Cometa::Certificate genericCertificate(committeeAuthorization);

  if (Cometa::Certificate::fromCbor(genericCertificate.toCbor()) != genericCertificate)
  {
    return 1;
  }

  Cometa::CertificateSet certificateSet;
  certificateSet.add(genericCertificate);

  if (Cometa::CertificateSet::fromCbor(certificateSet.toCbor()).at(0) != genericCertificate)
  {
    return 1;
  }

  const Cometa::ExUnitPrices prices(Cometa::UnitInterval(1, 2), Cometa::UnitInterval(1, 3));

  if (Cometa::ExUnitPrices::fromCbor(prices.toCbor()) != prices)
  {
    return 1;
  }

  const Cometa::CostModel costModel(Cometa::PlutusLanguageVersion::V1, { 1, 2, 3 });

  if (Cometa::CostModel::fromCbor(costModel.toCbor()) != costModel)
  {
    return 1;
  }

  Cometa::CostModels costModels;
  costModels.insert(costModel);

  if (Cometa::CostModels::fromCbor(costModels.toCbor()) != costModels)
  {
    return 1;
  }

  const Cometa::DRepVotingThresholds dRepVotingThresholds(Cometa::UnitInterval(0, 1), Cometa::UnitInterval(1, 2), Cometa::UnitInterval(2, 3), Cometa::UnitInterval(3, 4), Cometa::UnitInterval(4, 5), Cometa::UnitInterval(5, 6), Cometa::UnitInterval(6, 7), Cometa::UnitInterval(7, 8), Cometa::UnitInterval(8, 9), Cometa::UnitInterval(9, 10));

  if (Cometa::DRepVotingThresholds::fromCbor(dRepVotingThresholds.toCbor()) != dRepVotingThresholds)
  {
    return 1;
  }

  const Cometa::PoolVotingThresholds poolVotingThresholds(Cometa::UnitInterval(0, 1), Cometa::UnitInterval(1, 2), Cometa::UnitInterval(2, 3), Cometa::UnitInterval(3, 4), Cometa::UnitInterval(4, 5));

  if (Cometa::PoolVotingThresholds::fromCbor(poolVotingThresholds.toCbor()) != poolVotingThresholds)
  {
    return 1;
  }

  Cometa::ProtocolParamUpdate parameterUpdate;
  parameterUpdate.setMinFeeA(std::uint64_t(44));

  if (Cometa::ProtocolParamUpdate::fromCbor(parameterUpdate.toCbor()) != parameterUpdate)
  {
    return 1;
  }

  Cometa::ProposedParamUpdates proposals;
  proposals.add(Cometa::Blake2bHash::fromHex(std::string(56, '0')), parameterUpdate);

  if (Cometa::ProposedParamUpdates::fromCbor(proposals.toCbor()) != proposals)
  {
    return 1;
  }

  const Cometa::Update epochUpdate(12, proposals);

  if (Cometa::Update::fromCbor(epochUpdate.toCbor()) != epochUpdate)
  {
    return 1;
  }

  Cometa::ProtocolParameters protocolParameters;
  protocolParameters.setMinFeeA(44);
  if (Cometa::ProtocolParameters(protocolParameters).minFeeA() != 44)
  {
    return 1;
  }

  if (Cometa::InfoAction::fromCbor(Cometa::InfoAction().toCbor()) != Cometa::InfoAction())
  {
    return 1;
  }

  if (Cometa::NoConfidenceAction::fromCbor(Cometa::NoConfidenceAction().toCbor()) != Cometa::NoConfidenceAction())
  {
    return 1;
  }

  const Cometa::HardForkInitiationAction hardFork(Cometa::ProtocolVersion(12, 0));
  if (Cometa::HardForkInitiationAction::fromCbor(hardFork.toCbor()) != hardFork)
  {
    return 1;
  }

  const Cometa::ParameterChangeAction parameterChange(parameterUpdate);
  if (Cometa::ParameterChangeAction::fromCbor(parameterChange.toCbor()) != parameterChange)
  {
    return 1;
  }

  const Cometa::Constitution constitution(Cometa::Anchor("https://a", Cometa::Blake2bHash::fromHex(std::string(64, '0'))));
  if (Cometa::Constitution::fromCbor(constitution.toCbor()) != constitution)
  {
    return 1;
  }

  const Cometa::NewConstitutionAction newConstitution(constitution);
  if (Cometa::NewConstitutionAction::fromCbor(newConstitution.toCbor()) != newConstitution)
  {
    return 1;
  }

  const Cometa::TreasuryWithdrawalsAction treasuryWithdrawals { Cometa::WithdrawalMap() };
  if (Cometa::TreasuryWithdrawalsAction::fromCbor(treasuryWithdrawals.toCbor()) != treasuryWithdrawals)
  {
    return 1;
  }

  Cometa::CommitteeMembersMap committeeMembers;
  const Cometa::Credential    committeeMember(Cometa::CredentialType::KeyHash, Cometa::Blake2bHash::fromHex(std::string(56, '0')));
  committeeMembers.insert(committeeMember, 200);
  if (Cometa::CommitteeMembersMap::fromCbor(committeeMembers.toCbor()) != committeeMembers || !committeeMembers.keys().contains(committeeMember))
  {
    return 1;
  }

  Cometa::Committee committee(Cometa::UnitInterval(1, 2));
  committee.addMember(committeeMember, 200);
  if (Cometa::Committee::fromCbor(committee.toCbor()) != committee || !committee.keys().contains(committeeMember))
  {
    return 1;
  }

  const Cometa::UpdateCommitteeAction committeeUpdate(Cometa::CredentialSet(), committeeMembers, Cometa::UnitInterval(1, 2));
  if (Cometa::UpdateCommitteeAction::fromCbor(committeeUpdate.toCbor()) != committeeUpdate)
  {
    return 1;
  }

  const Cometa::ProposalProcedure governanceProposal(500, Cometa::RewardAddress(Cometa::NetworkId::Testnet, committeeMember), Cometa::Anchor("https://a", Cometa::Blake2bHash::fromHex(std::string(64, '0'))), Cometa::InfoAction());
  if (Cometa::ProposalProcedure::fromCbor(governanceProposal.toCbor()) != governanceProposal)
  {
    return 1;
  }

  Cometa::ProposalProcedureSet governanceProposals;
  governanceProposals.add(governanceProposal);
  if (Cometa::ProposalProcedureSet::fromCbor(governanceProposals.toCbor()) != governanceProposals)
  {
    return 1;
  }

  const Cometa::Voter voter(Cometa::VoterType::DRepKeyHash, committeeMember);
  if (Cometa::Voter::fromCbor(voter.toCbor()) != voter || Cometa::toString(Cometa::Vote::Yes).empty())
  {
    return 1;
  }

  const Cometa::VotingProcedure vote(Cometa::Vote::Yes);
  if (Cometa::VotingProcedure::fromCbor(vote.toCbor()) != vote)
  {
    return 1;
  }

  Cometa::VotingProcedures votes;
  votes.insert(voter, Cometa::GovernanceActionId(Cometa::Blake2bHash::fromHex(std::string(64, '0')), 0), vote);
  if (Cometa::VotingProcedures::fromCbor(votes.toCbor()) != votes || votes.voters().size() != 1U)
  {
    return 1;
  }

  const Cometa::Redeemer redeemer(Cometa::RedeemerTag::Spend, 0, Cometa::PlutusData(1), Cometa::ExUnits(5, 7));
  if (Cometa::Redeemer::fromCbor(redeemer.toCbor()) != redeemer)
  {
    return 1;
  }

  Cometa::RedeemerList redeemers;
  redeemers.add(redeemer);
  if (Cometa::RedeemerList::fromCbor(redeemers.toCbor()) != redeemers)
  {
    return 1;
  }

  const Cometa::VkeyWitness keyWitness(Cometa::Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Cometa::Ed25519Signature(std::vector<std::uint8_t>(64, 2)));
  if (Cometa::VkeyWitness::fromCbor(keyWitness.toCbor()) != keyWitness)
  {
    return 1;
  }

  Cometa::VkeyWitnessSet keyWitnesses;
  keyWitnesses.add(keyWitness);
  if (Cometa::VkeyWitnessSet::fromCbor(keyWitnesses.toCbor()) != keyWitnesses)
  {
    return 1;
  }

  const Cometa::BootstrapWitness bootstrapWitness(Cometa::Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Cometa::Ed25519Signature(std::vector<std::uint8_t>(64, 2)), std::vector<std::uint8_t>(32, 3), { 0xa0 });
  if (Cometa::BootstrapWitness::fromCbor(bootstrapWitness.toCbor()) != bootstrapWitness)
  {
    return 1;
  }

  Cometa::BootstrapWitnessSet bootstrapWitnesses;
  bootstrapWitnesses.add(bootstrapWitness);
  if (Cometa::BootstrapWitnessSet::fromCbor(bootstrapWitnesses.toCbor()) != bootstrapWitnesses)
  {
    return 1;
  }

  Cometa::NativeScriptSet nativeScripts;
  nativeScripts.add(Cometa::NativeScript(Cometa::ScriptInvalidBefore(5)));
  if (Cometa::NativeScriptSet::fromCbor(nativeScripts.toCbor()) != nativeScripts)
  {
    return 1;
  }

  Cometa::PlutusDataSet datumValues;
  datumValues.add(Cometa::PlutusData(1));
  if (Cometa::PlutusDataSet::fromCbor(datumValues.toCbor()) != datumValues)
  {
    return 1;
  }

  Cometa::PlutusV1ScriptSet plutusV1Scripts;
  plutusV1Scripts.add(Cometa::PlutusV1Script(std::vector<std::uint8_t> { 0x01, 0x02 }));
  if (Cometa::PlutusV1ScriptSet::fromCbor(plutusV1Scripts.toCbor()) != plutusV1Scripts)
  {
    return 1;
  }

  Cometa::PlutusV2ScriptSet plutusV2Scripts;
  plutusV2Scripts.add(Cometa::PlutusV2Script(std::vector<std::uint8_t> { 0x01, 0x02 }));
  if (Cometa::PlutusV2ScriptSet::fromCbor(plutusV2Scripts.toCbor()) != plutusV2Scripts)
  {
    return 1;
  }

  Cometa::PlutusV3ScriptSet plutusV3Scripts;
  plutusV3Scripts.add(Cometa::PlutusV3Script(std::vector<std::uint8_t> { 0x01, 0x02 }));
  if (Cometa::PlutusV3ScriptSet::fromCbor(plutusV3Scripts.toCbor()) != plutusV3Scripts)
  {
    return 1;
  }

  const Cometa::SubTransactionBody subBody { Cometa::TransactionInputSet(), Cometa::TransactionOutputList() };
  if (Cometa::SubTransactionBody::fromCbor(subBody.toCbor()) != subBody)
  {
    return 1;
  }

  Cometa::RequiredGuardsMap requiredGuards;
  requiredGuards.insert(Cometa::Credential::fromCbor("8200581c00112233445566778899aabbccddeeff00112233445566778899aabb"));
  if (Cometa::RequiredGuardsMap::fromCbor(requiredGuards.toCbor()) != requiredGuards)
  {
    return 1;
  }

  Cometa::GuardSet guards;
  guards.add(Cometa::Credential::fromCbor("8200581c6199186adb51974690d7247d2646097d2c62763b16fb7ed3f9f55d39"));
  if (Cometa::GuardSet::fromCbor(guards.toCbor()) != guards)
  {
    return 1;
  }

  Cometa::DirectDepositMap directDeposits;
  directDeposits.insert(Cometa::RewardAddress::fromBech32("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s"), 1000);
  if (Cometa::DirectDepositMap::fromCbor(directDeposits.toCbor()) != directDeposits)
  {
    return 1;
  }

  Cometa::AccountBalanceIntervalsMap accountBalances;
  const Cometa::RewardAddress        mapAddress = Cometa::RewardAddress::fromBech32("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s");
  accountBalances.insert(mapAddress, Cometa::AccountBalanceInterval::exact(1000));
  if (Cometa::AccountBalanceIntervalsMap::fromCbor(accountBalances.toCbor()) != accountBalances)
  {
    return 1;
  }

  const Cometa::AccountBalanceInterval accountBalance = Cometa::AccountBalanceInterval::exact(1000);
  if (Cometa::AccountBalanceInterval::fromCbor(accountBalance.toCbor()) != accountBalance)
  {
    return 1;
  }

  Cometa::WitnessSet witnesses;
  witnesses.setNativeScripts(Cometa::NativeScriptSet());
  if (!witnesses.nativeScripts().has_value())
  {
    return 1;
  }

  Cometa::PlutusV4ScriptSet plutusV4Scripts;
  plutusV4Scripts.add(Cometa::PlutusV4Script(std::vector<std::uint8_t> { 0x01, 0x02 }));
  if (Cometa::PlutusV4ScriptSet::fromCbor(plutusV4Scripts.toCbor()) != plutusV4Scripts)
  {
    return 1;
  }

  return name.toString() == "Cometa" && budget.cpuSteps() == 2 && version.major() == 12 &&
      slots.unixTimeFromSlot(0) == 1666656000ULL
    ? 0
    : 1;
}
