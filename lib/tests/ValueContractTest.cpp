/**
 * @file ValueContractTest.cpp
 *
 * @brief Unit tests for value semantics, serialization, and allocation failures.
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

#include <src/allocators.h>

#include <gtest/gtest.h>

#include <cstdlib>
#include <limits>
#include <type_traits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
const std::string HASH_HEX(56, '0');

Blake2bHash
keyHash()
{
  return Blake2bHash::fromHex(HASH_HEX);
}

std::size_t failureIndex    = 0;
std::size_t allocationIndex = 0;

void*
failingMalloc(std::size_t size)
{
  return allocationIndex++ == failureIndex ? nullptr : std::malloc(size);
}

void*
failingRealloc(void* pointer, std::size_t size)
{
  return allocationIndex++ == failureIndex ? nullptr : std::realloc(pointer, size);
}

class AllocationFailure
{
  public:

    explicit AllocationFailure(std::size_t index, bool trackRealloc = false)
    {
      allocationIndex = 0;
      failureIndex    = index;
      cardano_set_allocators(failingMalloc, trackRealloc ? failingRealloc : std::realloc, std::free);
    }

    ~AllocationFailure()
    {
      cardano_set_allocators(std::malloc, std::realloc, std::free);
    }

    AllocationFailure(const AllocationFailure&)            = delete;
    AllocationFailure& operator=(const AllocationFailure&) = delete;
};
template<class T>
T sampleValue();

template<>
Blake2bHash
sampleValue<Blake2bHash>()
{
  return keyHash();
}

template<>
AssetName
sampleValue<AssetName>()
{
  return AssetName::fromString("Cometa");
}

template<>
Credential
sampleValue<Credential>()
{
  return Credential(CredentialType::KeyHash, keyHash());
}

template<class T>
class ValueContract : public testing::Test
{
};

template<>
ExUnits
sampleValue<ExUnits>()
{
  return ExUnits(100, 200);
}

template<>
ProtocolVersion
sampleValue<ProtocolVersion>()
{
  return ProtocolVersion(12, 0);
}

template<>
UnitInterval
sampleValue<UnitInterval>()
{
  return UnitInterval(3, 4);
}

template<>
Anchor
sampleValue<Anchor>()
{
  return Anchor("https://example.com", Blake2bHash::fromHex(std::string(64, '0')));
}

template<>
GovernanceActionId
sampleValue<GovernanceActionId>()
{
  return GovernanceActionId(Blake2bHash::fromHex(std::string(64, '0')), 1);
}

template<>
BigInt
sampleValue<BigInt>()
{
  return BigInt::fromString("340282366920938463463374607431768211456");
}

template<>
PlutusData
sampleValue<PlutusData>()
{
  return PlutusData::fromCbor("d87982a1010243aabbcc");
}

template<>
PlutusList
sampleValue<PlutusList>()
{
  return PlutusList::fromCbor("82a1010243aabbcc");
}

template<>
PlutusMap
sampleValue<PlutusMap>()
{
  return PlutusMap::fromCbor("a2010243aabbcc8103");
}

template<>
ConstrPlutusData
sampleValue<ConstrPlutusData>()
{
  return ConstrPlutusData::fromCbor("d87982a1010243aabbcc");
}

template<>
Datum
sampleValue<Datum>()
{
  return Datum(PlutusData::fromCbor("d87982a1010243aabbcc"));
}

template<>
ScriptPubkey
sampleValue<ScriptPubkey>()
{
  return ScriptPubkey(Blake2bHash::fromHex(std::string(56, '1')));
}

template<>
ScriptInvalidBefore
sampleValue<ScriptInvalidBefore>()
{
  return ScriptInvalidBefore(42);
}

template<>
ScriptInvalidAfter
sampleValue<ScriptInvalidAfter>()
{
  return ScriptInvalidAfter(42);
}

template<>
ScriptRequireGuard
sampleValue<ScriptRequireGuard>()
{
  return ScriptRequireGuard(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
}

template<>
NativeScript
sampleValue<NativeScript>()
{
  return NativeScript(ScriptInvalidBefore(5));
}

template<>
NativeScriptList
sampleValue<NativeScriptList>()
{
  return NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) };
}

template<>
ScriptAll
sampleValue<ScriptAll>()
{
  return ScriptAll(NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) });
}

template<>
ScriptAny
sampleValue<ScriptAny>()
{
  return ScriptAny(NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) });
}

template<>
ScriptNOfK
sampleValue<ScriptNOfK>()
{
  return ScriptNOfK(1, NativeScriptList { NativeScript(ScriptInvalidBefore(5)), NativeScript(ScriptInvalidAfter(7)) });
}

template<>
PlutusV1Script
sampleValue<PlutusV1Script>()
{
  return PlutusV1Script::fromHex("450100002499");
}

template<>
PlutusV2Script
sampleValue<PlutusV2Script>()
{
  return PlutusV2Script::fromHex("450100002499");
}

template<>
PlutusV3Script
sampleValue<PlutusV3Script>()
{
  return PlutusV3Script::fromHex("450100002499");
}

template<>
PlutusV4Script
sampleValue<PlutusV4Script>()
{
  return PlutusV4Script::fromHex("450100002499");
}

template<>
Script
sampleValue<Script>()
{
  return Script(sampleValue<NativeScript>());
}

template<>
TransactionMetadata
sampleValue<TransactionMetadata>()
{
  TransactionMetadata metadata;
  metadata.set(1, Metadatum("value"));

  return metadata;
}

template<>
AuxiliaryData
sampleValue<AuxiliaryData>()
{
  AuxiliaryData value;
  value.setMetadata(sampleValue<TransactionMetadata>());
  value.setNativeScripts(NativeScriptList { NativeScript(ScriptInvalidBefore(5)) });
  value.setPlutusV1Scripts(PlutusV1ScriptList { PlutusV1Script::fromHex("450100002499") });
  value.setPlutusV2Scripts(PlutusV2ScriptList { PlutusV2Script::fromHex("450100002499") });
  value.setPlutusV3Scripts(PlutusV3ScriptList { PlutusV3Script::fromHex("450100002499") });
  value.setPlutusV4Scripts(PlutusV4ScriptList { PlutusV4Script::fromHex("450100002499") });

  return value;
}

template<>
Metadatum
sampleValue<Metadatum>()
{
  return Metadatum::fromPlainJson(R"({"key":[1,"text"]})");
}

template<>
MetadatumList
sampleValue<MetadatumList>()
{
  return MetadatumList { sampleValue<Metadatum>() };
}

template<>
MetadatumMap
sampleValue<MetadatumMap>()
{
  MetadatumMap map;
  map.insert(Metadatum("key"), sampleValue<Metadatum>());

  return map;
}

template<>
PlutusV1ScriptList
sampleValue<PlutusV1ScriptList>()
{
  return PlutusV1ScriptList { PlutusV1Script::fromHex("450100002499") };
}

template<>
PlutusV2ScriptList
sampleValue<PlutusV2ScriptList>()
{
  return PlutusV2ScriptList { PlutusV2Script::fromHex("450100002499") };
}

template<>
PlutusV3ScriptList
sampleValue<PlutusV3ScriptList>()
{
  return PlutusV3ScriptList { PlutusV3Script::fromHex("450100002499") };
}

template<>
PlutusV4ScriptList
sampleValue<PlutusV4ScriptList>()
{
  return PlutusV4ScriptList { PlutusV4Script::fromHex("450100002499") };
}

template<>
TransactionInput
sampleValue<TransactionInput>()
{
  return TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
}

template<>
TransactionOutput
sampleValue<TransactionOutput>()
{
  TransactionOutput value(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  value.setDatum(Datum(PlutusData(std::int64_t { 42 })));
  value.setScriptRef(Script(PlutusV4Script::fromHex("450100002499")));

  return value;
}

template<>
Utxo
sampleValue<Utxo>()
{
  return Utxo(sampleValue<TransactionInput>(), sampleValue<TransactionOutput>());
}

template<>
TransactionInputSet
sampleValue<TransactionInputSet>()
{
  return TransactionInputSet { TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5) };
}

template<>
TransactionOutputList
sampleValue<TransactionOutputList>()
{
  return TransactionOutputList { sampleValue<TransactionOutput>() };
}

template<>
WithdrawalMap
sampleValue<WithdrawalMap>()
{
  WithdrawalMap value;
  value.set("stake_test1uqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxtxu2s", std::numeric_limits<std::uint64_t>::max());

  return value;
}

template<>
Ipv4
sampleValue<Ipv4>()
{
  return Ipv4::fromString("127.0.0.1");
}

template<>
Ipv6
sampleValue<Ipv6>()
{
  return Ipv6::fromString("0000:0000:0000:0000:0000:0000:0000:0001");
}

template<>
BlsKey
sampleValue<BlsKey>()
{
  return BlsKey(std::vector<std::uint8_t>(96, 1), std::vector<std::uint8_t>(48, 2));
}

template<>
PoolMetadata
sampleValue<PoolMetadata>()
{
  return PoolMetadata("https://example.com", Blake2bHash::fromHex(std::string(64, '1')));
}

template<>
SingleHostAddrRelay
sampleValue<SingleHostAddrRelay>()
{
  return SingleHostAddrRelay(3001, sampleValue<Ipv4>(), sampleValue<Ipv6>());
}

template<>
SingleHostNameRelay
sampleValue<SingleHostNameRelay>()
{
  return SingleHostNameRelay("example.com", 3001);
}

template<>
MultiHostNameRelay
sampleValue<MultiHostNameRelay>()
{
  return MultiHostNameRelay("example.com");
}

template<>
Relay
sampleValue<Relay>()
{
  return Relay(sampleValue<SingleHostAddrRelay>());
}

template<>
Relays
sampleValue<Relays>()
{
  return Relays { sampleValue<Relay>(), Relay(sampleValue<SingleHostNameRelay>()), Relay(sampleValue<MultiHostNameRelay>()) };
}

template<>
PoolOwners
sampleValue<PoolOwners>()
{
  return PoolOwners { Blake2bHash::fromHex(std::string(56, '1')) };
}

template<>
PoolParams
sampleValue<PoolParams>()
{
  const Blake2bHash   operatorHash = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash   vrfHash      = Blake2bHash::fromHex(std::string(64, '2'));
  const Credential    credential(CredentialType::KeyHash, operatorHash);
  const RewardAddress reward(NetworkId::Testnet, credential);

  return PoolParams(operatorHash, vrfHash, 10'000'000, 340'000, UnitInterval(1, 20), reward, PoolOwners { operatorHash }, sampleValue<Relays>());
}

template<>
StakeRegistrationCert
sampleValue<StakeRegistrationCert>()
{
  return StakeRegistrationCert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
}

template<>
StakeDeregistrationCert
sampleValue<StakeDeregistrationCert>()
{
  return StakeDeregistrationCert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
}

template<>
RegistrationCert
sampleValue<RegistrationCert>()
{
  return RegistrationCert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))), 2'000'000);
}

template<>
UnregistrationCert
sampleValue<UnregistrationCert>()
{
  return UnregistrationCert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))), 2'000'000);
}

template<>
StakeDelegationCert
sampleValue<StakeDelegationCert>()
{
  return StakeDelegationCert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))), Blake2bHash::fromHex(std::string(56, '2')));
}

template<>
PoolRegistrationCert
sampleValue<PoolRegistrationCert>()
{
  return PoolRegistrationCert(sampleValue<PoolParams>());
}

template<>
PoolRetirementCert
sampleValue<PoolRetirementCert>()
{
  return PoolRetirementCert(Blake2bHash::fromHex(std::string(56, '1')), 1000);
}

template<>
StakeRegistrationDelegationCert
sampleValue<StakeRegistrationDelegationCert>()
{
  return StakeRegistrationDelegationCert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))), Blake2bHash::fromHex(std::string(56, '2')), 2'000'000);
}

template<>
GenesisKeyDelegationCert
sampleValue<GenesisKeyDelegationCert>()
{
  return GenesisKeyDelegationCert(Blake2bHash::fromHex(std::string(56, '1')), Blake2bHash::fromHex(std::string(56, '2')), Blake2bHash::fromHex(std::string(64, '3')));
}

template<>
MirToPotCert
sampleValue<MirToPotCert>()
{
  return MirToPotCert(MirPotType::Reserve, 1'000'000);
}

template<>
MirToStakeCredentialsCert
sampleValue<MirToStakeCredentialsCert>()
{
  MirToStakeCredentialsCert cert(MirPotType::Reserve);
  cert.insert(Credential(CredentialType::KeyHash, keyHash()), 1'000'000);
  return cert;
}

template<>
MirCert
sampleValue<MirCert>()
{
  return MirCert(MirToPotCert(MirPotType::Reserve, 1'000'000));
}

template<>
DRep
sampleValue<DRep>()
{
  return DRep(Credential(CredentialType::KeyHash, keyHash()));
}

template<>
VoteDelegationCert
sampleValue<VoteDelegationCert>()
{
  const Credential credential(CredentialType::KeyHash, keyHash());
  return VoteDelegationCert(credential, DRep(credential));
}

template<>
StakeVoteDelegationCert
sampleValue<StakeVoteDelegationCert>()
{
  const Credential credential(CredentialType::KeyHash, keyHash());
  return StakeVoteDelegationCert(credential, keyHash(), DRep(credential));
}

template<>
VoteRegistrationDelegationCert
sampleValue<VoteRegistrationDelegationCert>()
{
  const Credential credential(CredentialType::KeyHash, keyHash());
  return VoteRegistrationDelegationCert(credential, 2'000'000, DRep(credential));
}

template<>
StakeVoteRegistrationDelegationCert
sampleValue<StakeVoteRegistrationDelegationCert>()
{
  const Credential credential(CredentialType::KeyHash, keyHash());
  return StakeVoteRegistrationDelegationCert(credential, keyHash(), DRep(credential), 2'000'000);
}

template<>
UnregisterDRepCert
sampleValue<UnregisterDRepCert>()
{
  return UnregisterDRepCert(Credential(CredentialType::KeyHash, keyHash()), 2'000'000);
}

template<>
RegisterDRepCert
sampleValue<RegisterDRepCert>()
{
  return RegisterDRepCert(Credential(CredentialType::KeyHash, keyHash()), 2'000'000);
}

template<>
UpdateDRepCert
sampleValue<UpdateDRepCert>()
{
  return UpdateDRepCert(Credential(CredentialType::KeyHash, keyHash()));
}

template<>
AuthCommitteeHotCert
sampleValue<AuthCommitteeHotCert>()
{
  const Credential credential(CredentialType::KeyHash, keyHash());
  return AuthCommitteeHotCert(credential, credential);
}

template<>
ResignCommitteeColdCert
sampleValue<ResignCommitteeColdCert>()
{
  return ResignCommitteeColdCert(Credential(CredentialType::KeyHash, keyHash()));
}

template<>
Certificate
sampleValue<Certificate>()
{
  return Certificate(StakeRegistrationCert(Credential(CredentialType::KeyHash, keyHash())));
}

template<>
CertificateSet
sampleValue<CertificateSet>()
{
  CertificateSet set;
  set.add(sampleValue<Certificate>());
  return set;
}

template<>
ExUnitPrices
sampleValue<ExUnitPrices>()
{
  return ExUnitPrices(UnitInterval(1, 2), UnitInterval(1, 3));
}

template<>
CostModel
sampleValue<CostModel>()
{
  return CostModel(PlutusLanguageVersion::V1, { 1, 2, 3 });
}

template<>
CostModels
sampleValue<CostModels>()
{
  CostModels models;
  models.insert(sampleValue<CostModel>());
  return models;
}

template<>
DRepVotingThresholds
sampleValue<DRepVotingThresholds>()
{
  return DRepVotingThresholds(UnitInterval(0, 1), UnitInterval(1, 2), UnitInterval(2, 3), UnitInterval(3, 4), UnitInterval(4, 5), UnitInterval(5, 6), UnitInterval(6, 7), UnitInterval(7, 8), UnitInterval(8, 9), UnitInterval(9, 10));
}

template<>
PoolVotingThresholds
sampleValue<PoolVotingThresholds>()
{
  return PoolVotingThresholds(UnitInterval(0, 1), UnitInterval(1, 2), UnitInterval(2, 3), UnitInterval(3, 4), UnitInterval(4, 5));
}

template<>
ProtocolParamUpdate
sampleValue<ProtocolParamUpdate>()
{
  ProtocolParamUpdate update;
  update.setMinFeeA(std::uint64_t(44));
  update.setPoolPledgeInfluence(UnitInterval(1, 2));
  return update;
}

template<>
ProposedParamUpdates
sampleValue<ProposedParamUpdates>()
{
  ProposedParamUpdates proposals;
  proposals.add(Blake2bHash::fromHex(std::string(56, '0')), sampleValue<ProtocolParamUpdate>());
  return proposals;
}

template<>
Update
sampleValue<Update>()
{
  return Update(12, sampleValue<ProposedParamUpdates>());
}

template<>
InfoAction
sampleValue<InfoAction>()
{
  return InfoAction();
}

template<>
NoConfidenceAction
sampleValue<NoConfidenceAction>()
{
  return NoConfidenceAction();
}

template<>
HardForkInitiationAction
sampleValue<HardForkInitiationAction>()
{
  return HardForkInitiationAction(ProtocolVersion(12, 0));
}

template<>
ParameterChangeAction
sampleValue<ParameterChangeAction>()
{
  ProtocolParamUpdate update;
  update.setMinFeeA(std::uint64_t(44));
  return ParameterChangeAction(update);
}

template<>
Constitution
sampleValue<Constitution>()
{
  return Constitution(Anchor("https://a", Blake2bHash::fromHex(std::string(64, '0'))));
}

template<>
NewConstitutionAction
sampleValue<NewConstitutionAction>()
{
  return NewConstitutionAction(sampleValue<Constitution>());
}

template<>
TreasuryWithdrawalsAction
sampleValue<TreasuryWithdrawalsAction>()
{
  return TreasuryWithdrawalsAction(WithdrawalMap());
}

template<>
CommitteeMembersMap
sampleValue<CommitteeMembersMap>()
{
  CommitteeMembersMap map;
  map.insert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0'))), 200);
  return map;
}

template<>
Committee
sampleValue<Committee>()
{
  Committee committee(UnitInterval(1, 2));
  committee.addMember(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0'))), 200);
  return committee;
}

template<>
UpdateCommitteeAction
sampleValue<UpdateCommitteeAction>()
{
  return UpdateCommitteeAction(CredentialSet(), CommitteeMembersMap(), UnitInterval(1, 2));
}

template<>
ProposalProcedure
sampleValue<ProposalProcedure>()
{
  const Credential credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  return ProposalProcedure(500, RewardAddress(NetworkId::Testnet, credential), Anchor("https://a", Blake2bHash::fromHex(std::string(64, '0'))), InfoAction());
}

template<>
ProposalProcedureSet
sampleValue<ProposalProcedureSet>()
{
  ProposalProcedureSet set;
  set.add(sampleValue<ProposalProcedure>());
  return set;
}

template<>
Voter
sampleValue<Voter>()
{
  return Voter(VoterType::DRepKeyHash, Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0'))));
}

template<>
VotingProcedure
sampleValue<VotingProcedure>()
{
  return VotingProcedure(Vote::Yes, Anchor("https://a", Blake2bHash::fromHex(std::string(64, '0'))));
}

template<>
VotingProcedures
sampleValue<VotingProcedures>()
{
  VotingProcedures result;
  result.insert(sampleValue<Voter>(), GovernanceActionId(Blake2bHash::fromHex(std::string(64, '1')), 0), sampleValue<VotingProcedure>());
  return result;
}

template<>
Redeemer
sampleValue<Redeemer>()
{
  return Redeemer(RedeemerTag::Spend, 0, PlutusData(1), ExUnits(5, 7));
}

using ValueTypes = testing::Types<Redeemer, VotingProcedures, VotingProcedure, Voter, ProposalProcedureSet, ProposalProcedure, UpdateCommitteeAction, Committee, CommitteeMembersMap, TreasuryWithdrawalsAction, NewConstitutionAction, Constitution, ParameterChangeAction, HardForkInitiationAction, NoConfidenceAction, InfoAction, Update, ProposedParamUpdates, ProtocolParamUpdate, DRepVotingThresholds, PoolVotingThresholds, CostModels, CostModel, ExUnitPrices, CertificateSet, Certificate, ResignCommitteeColdCert, AuthCommitteeHotCert, UpdateDRepCert, RegisterDRepCert, UnregisterDRepCert, StakeVoteRegistrationDelegationCert, VoteRegistrationDelegationCert, StakeVoteDelegationCert, VoteDelegationCert, DRep, MirCert, MirToStakeCredentialsCert, MirToPotCert, GenesisKeyDelegationCert, StakeRegistrationDelegationCert, PoolRetirementCert, PoolRegistrationCert, StakeDelegationCert, RegistrationCert, UnregistrationCert, StakeDeregistrationCert, StakeRegistrationCert, PoolParams, PoolOwners, SingleHostAddrRelay, SingleHostNameRelay, MultiHostNameRelay, Relay, Relays, Ipv4, Ipv6, BlsKey, PoolMetadata, TransactionMetadata, AuxiliaryData, Metadatum, MetadatumList, MetadatumMap, PlutusV1ScriptList, PlutusV2ScriptList, PlutusV3ScriptList, PlutusV4ScriptList, TransactionInput, TransactionOutput, Utxo, TransactionInputSet, TransactionOutputList, WithdrawalMap, Script, PlutusV1Script, PlutusV2Script, PlutusV3Script, PlutusV4Script, NativeScript, NativeScriptList, ScriptAll, ScriptAny, ScriptNOfK, ScriptPubkey, ScriptInvalidBefore, ScriptInvalidAfter, ScriptRequireGuard, PlutusData, PlutusList, PlutusMap, ConstrPlutusData, Datum, Blake2bHash, AssetName, Credential, ExUnits, ProtocolVersion, UnitInterval, Anchor, GovernanceActionId>;
TYPED_TEST_SUITE(ValueContract, ValueTypes);
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(Credential, FailedCopyAssignmentPreservesDestination)
{
  // Arrange
  const Credential source(CredentialType::KeyHash, keyHash());
  Credential       destination(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential before          = destination;
  std::size_t      allocationCount = 0;
  {
    AllocationFailure count(std::numeric_limits<std::size_t>::max());
    const Credential  copy = source;
    allocationCount        = allocationIndex;
  }
  ASSERT_GT(allocationCount, 0);

  // Act & Assert
  for (std::size_t i = 0; i < allocationCount; ++i)
  {
    {
      AllocationFailure fail(i);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }
    EXPECT_EQ(destination, before);
    EXPECT_EQ(source.type(), CredentialType::KeyHash);
  }
}

TYPED_TEST(ValueContract, SupportsCopyAndMove)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();

  // Act
  const TypeParam copied = source;
  const TypeParam moved  = std::move(source);

  // Assert
  EXPECT_EQ(copied, moved);
  EXPECT_THROW(source.toCbor(), std::logic_error);
  static_assert(std::is_nothrow_move_constructible_v<TypeParam>);
  static_assert(std::is_nothrow_move_assignable_v<TypeParam>);
}

TYPED_TEST(ValueContract, SupportsCopySelfAssignment)
{
  // Arrange
  TypeParam        source = sampleValue<TypeParam>();
  const TypeParam  before = source;
  const TypeParam* alias  = &source;

  // Act
  source = *alias;

  // Assert
  EXPECT_EQ(source, before);
}

TYPED_TEST(ValueContract, SupportsMoveSelfAssignment)
{
  // Arrange
  TypeParam       source = sampleValue<TypeParam>();
  const TypeParam before = source;
  TypeParam*      alias  = &source;

  // Act
  source = std::move(*alias);

  // Assert
  EXPECT_EQ(source, before);
}

TYPED_TEST(ValueContract, SupportsAssignmentToMovedFromValues)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();
  TypeParam moved  = std::move(source);

  // Act
  source = moved;
  moved  = std::move(source);

  // Assert
  EXPECT_EQ(moved, sampleValue<TypeParam>());
  EXPECT_THROW(source.toCbor(), std::logic_error);
}

TYPED_TEST(ValueContract, SupportsSwapWithMovedFromValues)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();
  TypeParam moved  = std::move(source);

  // Act
  swap(source, moved);

  // Assert
  EXPECT_EQ(source, sampleValue<TypeParam>());
  EXPECT_THROW(moved.toCbor(), std::logic_error);
}

TYPED_TEST(ValueContract, ReleasesPartialCopiesOnEveryAllocationFailure)
{
  // Arrange
  const TypeParam source = sampleValue<TypeParam>();
  std::size_t     count  = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   copy(source);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW({ const TypeParam copy(source); }, std::bad_alloc);
  }
  EXPECT_EQ(source, sampleValue<TypeParam>());
}

TYPED_TEST(ValueContract, ReleasesParserStateOnAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> cbor  = sampleValue<TypeParam>().toCbor();
  std::size_t                     count = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   parsed = TypeParam::fromCbor(cbor);
    count                    = allocationIndex;
  }

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    try
    {
      const TypeParam parsed = TypeParam::fromCbor(cbor);
      // Optional native caches can fail without invalidating the parsed value.
      EXPECT_EQ(parsed.toCbor(), cbor);
    }
    catch (const std::bad_alloc&)
    {
    }
    catch (const Error& error)
    {
      // Some native parser paths translate allocation errors into CBOR errors.
      EXPECT_NE(error.code(), makeErrorCode(ErrorCode::Success));
    }
  }
}

TYPED_TEST(ValueContract, ReleasesStateWhenConstructionFails)
{
  // Arrange
  AllocationFailure fail(0);

  // Act & Assert
  EXPECT_THROW(sampleValue<TypeParam>(), std::bad_alloc);
}

TYPED_TEST(ValueContract, FailedCopyAssignmentPreservesDestination)
{
  // Arrange
  const TypeParam source      = sampleValue<TypeParam>();
  TypeParam       destination = sampleValue<TypeParam>();
  const TypeParam before      = destination;
  std::size_t     count       = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    destination = source;
    count       = allocationIndex;
  }

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    {
      AllocationFailure fail(i);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }
    EXPECT_EQ(destination, before);
  }
}

TEST(ProtocolParameters, ReleasesPartialCopiesOnAllocationFailure)
{
  // Arrange
  ProtocolParameters parameters;
  parameters.setMinFeeA(44);
  parameters.setMaxPledgeLeverage(UnitInterval(1, 2));
  std::size_t count = 0;
  {
    AllocationFailure        tracker(std::numeric_limits<std::size_t>::max());
    const ProtocolParameters copy(parameters);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW({ const ProtocolParameters copy(parameters); }, std::bad_alloc);
  }
  EXPECT_EQ(parameters.minFeeA(), 44);
  EXPECT_EQ(parameters.maxPledgeLeverage(), UnitInterval(1, 2));
}

TEST(Anchor, FailedMutationPreservesValue)
{
  // Arrange
  const Anchor before = sampleValue<Anchor>();
  std::size_t  count  = 0;
  {
    Anchor            value = before;
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    value.setUrl("https://changed.example");
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    Anchor value = before;
    {
      AllocationFailure fail(i);
      EXPECT_THROW(value.setUrl("https://changed.example"), std::bad_alloc);
    }
    EXPECT_EQ(value, before);
    EXPECT_EQ(value.toCbor(), before.toCbor());
  }
}

TEST(GovernanceActionId, FailedMutationPreservesValue)
{
  // Arrange
  const GovernanceActionId before = sampleValue<GovernanceActionId>();
  std::size_t              count  = 0;
  {
    GovernanceActionId value = before;
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max());
    value.setIndex(2);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    GovernanceActionId value = before;
    {
      AllocationFailure fail(i);
      EXPECT_THROW(value.setIndex(2), std::bad_alloc);
    }
    EXPECT_EQ(value, before);
    EXPECT_EQ(value.toCbor(), before.toCbor());
  }
}

TEST(UnitInterval, FloatingPointConversionReleasesStateOnFailure)
{
  // Arrange
  AllocationFailure fail(0);

  // Act & Assert
  EXPECT_THROW(UnitInterval::fromDouble(0.75), std::bad_alloc);
}

TEST(BigInt, FailedArithmeticPreservesOperands)
{
  // Arrange
  BigInt       value  = sampleValue<BigInt>();
  const BigInt before = value;

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(value *= value, std::bad_alloc);
  }
  EXPECT_EQ(value, before);
}

TEST(BigInt, FailedDivisionReleasesPartialResults)
{
  // Arrange
  const BigInt value = sampleValue<BigInt>();
  const BigInt divisor(7);

  // Act & Assert
  for (std::size_t i = 0; i < 2; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW(value.divRem(divisor), std::bad_alloc);
  }
}

TEST(BigInt, FailedCopyPreservesDestination)
{
  // Arrange
  const BigInt source = sampleValue<BigInt>();
  BigInt       destination(-7);

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(destination = source, std::bad_alloc);
  }
  EXPECT_EQ(destination, BigInt(-7));
  EXPECT_EQ(source, sampleValue<BigInt>());
}

TEST(BigInt, ConstructorsReleaseStateOnFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 1, 2, 3 };

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt(1), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromUnsigned(1), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromString("12345678901234567890"), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromBytes(bytes), std::bad_alloc);
  }
}

TEST(Base58, ReportsAllocationFailures)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 58 };

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(encodeBase58(bytes), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(decodeBase58("21"), std::bad_alloc);
  }
}

TEST(Bech32, ReleasesStateOnEveryEncodingAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 1, 2, 3 };
  std::size_t                     count = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const std::string encoded = encodeBech32("test", bytes);
    EXPECT_FALSE(encoded.empty());
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    bool              failed = false;

    try
    {
      encodeBech32("test", bytes);
    }
    catch (const std::bad_alloc&)
    {
      failed = true;
    }
    catch (const Error&)
    {
      failed = true;
    }

    EXPECT_TRUE(failed);
  }
}

TEST(Bech32, ReleasesStateOnEveryDecodingAllocationFailure)
{
  // Arrange
  const std::string encoded = encodeBech32("test", { 1, 2, 3 });
  std::size_t       count   = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const Bech32Data  decoded = decodeBech32(encoded);
    EXPECT_EQ(decoded.prefix, "test");
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    bool              failed = false;

    try
    {
      decodeBech32(encoded);
    }
    catch (const std::bad_alloc&)
    {
      failed = true;
    }
    catch (const Error&)
    {
      failed = true;
    }

    EXPECT_TRUE(failed);
  }
}

TEST(CborReader, FailedCopiesReleasePartialStateAndPreserveDestination)
{
  // Arrange
  CborReader source("82810102");
  ASSERT_EQ(source.readStartArray(), 2U);
  ASSERT_EQ(source.readStartArray(), 1U);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const CborReader  copy(source);

    EXPECT_GT(copy.bytesRemaining(), 0U);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    CborReader destination("03");

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.readUInt(), 3U);
  }
}

TEST(CborWriter, FailedCopiesReleasePartialStateAndPreserveDestination)
{
  // Arrange
  CborWriter source;
  source.writeStartArray(2);
  source.writeUInt(1);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const CborWriter  copy(source);

    EXPECT_GT(copy.toBytes().size(), 0U);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    CborWriter destination;
    destination.writeUInt(3);

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toHex(), "03");
  }
}

TEST(CborReader, ConstructorReleasesEveryPartialAllocation)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 0x81, 0x01 };
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const CborReader  reader(bytes);

    EXPECT_EQ(reader.bytesRemaining(), bytes.size());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW(CborReader reader(bytes), std::bad_alloc);
  }
}

TEST(CborWriter, FailedResetPreservesEncodedBytes)
{
  // Arrange
  CborWriter writer;
  writer.writeUInt(42);

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(writer.reset(), std::bad_alloc);
  }

  EXPECT_EQ(writer.toHex(), "182a");
}

TEST(JsonValue, FailedCopiesReleaseStateAndPreserveDestination)
{
  // Arrange
  const JsonValue source      = JsonValue::parse("{\"a\":[1,2,3],\"b\":{\"x\":1.2300}}");
  std::size_t     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const JsonValue   copy(source);

    EXPECT_GT(copy.size(), 0);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    JsonValue destination = JsonValue::parse("42");

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.asInt(), 42);
  }
}

TEST(JsonWriter, FailedCopiesReleaseStateAndPreserveDestination)
{
  // Arrange
  JsonWriter source;
  source.writeStartObject();
  source.writePropertyName("a");
  source.writeStartArray();
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const JsonWriter  copy(source);

    EXPECT_GT(static_cast<int>(copy.context()), 0);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    JsonWriter destination;
    destination.writeUInt(42);

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), "42");
  }
}

TEST(JsonValue, ParsingReportsEveryAllocationFailure)
{
  // Arrange
  const std::string json        = "{\"a\":[1,2,\"text\",true,null,{}]}";
  std::size_t       allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const JsonValue   value = JsonValue::parse(json);

    EXPECT_EQ(value.size(), 1U);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW(JsonValue::parse(json), std::bad_alloc);
  }
}

TEST(JsonValue, FailedChildCopiesLeaveParentReadable)
{
  // Arrange
  const JsonValue object = JsonValue::parse("{\"a\":[1,2]}");
  const JsonValue array  = object.at("a");

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(object.at("a"), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(object.valueAt(0), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(array.at(0), std::bad_alloc);
  }
  EXPECT_EQ(object.toJson(), "{\"a\":[1,2]}");
}

TEST(JsonWriter, FailedResetPreservesDocumentAndFormatting)
{
  // Arrange
  JsonWriter writer(JsonFormat::Pretty);
  writer.writeStartArray();
  writer.writeUInt(42);
  writer.writeEndArray();
  const std::string before = writer.toString();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(writer.reset(), std::bad_alloc);
  }
  EXPECT_EQ(writer.toString(), before);
}

TEST(JsonWriter, ReportsBigIntegerFormattingAllocationFailure)
{
  // Arrange
  JsonWriter   writer;
  const BigInt value(42);

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(writer.writeBigIntAsString(value), std::bad_alloc);
  }
  EXPECT_THROW(writer.toString(), std::bad_alloc);
  writer.reset();
  writer.writeBigIntAsString(value);
  EXPECT_EQ(writer.toString(), "\"42\"");
}

TEST(Ed25519Signature, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Ed25519Signature source         = Ed25519Signature(std::vector<std::uint8_t>(64, 1));
  Ed25519Signature       destination    = Ed25519Signature(std::vector<std::uint8_t>(64, 2));
  const std::string      sourceHex      = source.toHex();
  const std::string      destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Ed25519Signature, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(64, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519Signature(bytes), std::bad_alloc);
}

TEST(Ed25519PublicKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Ed25519PublicKey source         = Ed25519PublicKey(std::vector<std::uint8_t>(32, 1));
  Ed25519PublicKey       destination    = Ed25519PublicKey(std::vector<std::uint8_t>(32, 2));
  const std::string      sourceHex      = source.toHex();
  const std::string      destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Ed25519PublicKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(32, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519PublicKey(bytes), std::bad_alloc);
}

TEST(Ed25519PrivateKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Ed25519PrivateKey source         = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 1));
  Ed25519PrivateKey       destination    = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 2));
  const std::string       sourceHex      = source.toHex();
  const std::string       destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Ed25519PrivateKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(32, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519PrivateKey::fromNormalBytes(bytes), std::bad_alloc);
}

TEST(Bip32PublicKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Bip32PublicKey source         = Bip32PublicKey(std::vector<std::uint8_t>(64, 1));
  Bip32PublicKey       destination    = Bip32PublicKey(std::vector<std::uint8_t>(64, 2));
  const std::string    sourceHex      = source.toHex();
  const std::string    destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Bip32PublicKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(64, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Bip32PublicKey(bytes), std::bad_alloc);
}

TEST(Bip32PrivateKey, FailedCopyPreservesBothKeys)
{
  // Arrange
  const Bip32PrivateKey source         = Bip32PrivateKey(std::vector<std::uint8_t>(96, 1));
  Bip32PrivateKey       destination    = Bip32PrivateKey(std::vector<std::uint8_t>(96, 2));
  const std::string     sourceHex      = source.toHex();
  const std::string     destinationHex = destination.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(destination = source, std::bad_alloc);
  }

  EXPECT_EQ(source.toHex(), sourceHex);
  EXPECT_EQ(destination.toHex(), destinationHex);
}

TEST(Bip32PrivateKey, ConstructorReportsAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(96, 1);
  AllocationFailure               fail(0);

  // Act & Assert
  EXPECT_THROW((void)Bip32PrivateKey(bytes), std::bad_alloc);
}

TEST(Bip32PrivateKey, FailedDerivationReleasesWorkingKeys)
{
  // Arrange
  const Bip32PrivateKey            source = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0));
  const std::vector<std::uint32_t> path { hardenIndex(1852), hardenIndex(1815), hardenIndex(0), 0, 0 };
  const std::string                before      = source.toHex();
  std::size_t                      allocations = 0;

  {
    AllocationFailure     tracker(std::numeric_limits<std::size_t>::max());
    const Bip32PrivateKey result = source.derive(path);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);

      EXPECT_THROW(source.derive(path), std::bad_alloc);
    }

    EXPECT_EQ(source.toHex(), before);
  }
}

TEST(Ed25519PrivateKey, FailedSigningAndPublicKeyCreationPreserveKey)
{
  // Arrange
  const Ed25519PrivateKey key    = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 1));
  const std::string       before = key.toHex();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(key.sign({ 1, 2, 3 }), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);

    EXPECT_THROW(key.publicKey(), std::bad_alloc);
  }

  EXPECT_EQ(key.toHex(), before);
}

TEST(Bip32PublicKey, FailedDerivationReleasesWorkingKeys)
{
  // Arrange
  const Bip32PublicKey             source = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0)).publicKey();
  const std::vector<std::uint32_t> path { 0, 1, 2 };
  const std::string                before      = source.toHex();
  std::size_t                      allocations = 0;

  {
    AllocationFailure    tracker(std::numeric_limits<std::size_t>::max());
    const Bip32PublicKey result = source.derive(path);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);

      EXPECT_THROW(source.derive(path), std::bad_alloc);
    }

    EXPECT_EQ(source.toHex(), before);
  }
}

TEST(Ed25519PrivateKey, ExtendedSigningReleasesEveryPartialAllocation)
{
  // Arrange
  const Ed25519PrivateKey         key = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0)).toEd25519();
  const std::vector<std::uint8_t> message { 1, 2, 3 };
  const std::string               before      = key.toHex();
  std::size_t                     allocations = 0;

  {
    AllocationFailure      tracker(std::numeric_limits<std::size_t>::max());
    const Ed25519Signature signature = key.sign(message);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    {
      AllocationFailure fail(index);

      EXPECT_THROW(key.sign(message), std::bad_alloc);
    }

    EXPECT_EQ(key.toHex(), before);
  }
}

TEST(Emip3, AllocationFailuresReleaseTemporaryBuffers)
{
  // Arrange
  const std::vector<std::uint8_t> plaintext { 1, 2, 3 };
  const std::vector<std::uint8_t> encrypted             = encryptEmip3(plaintext, "password");
  std::size_t                     encryptionAllocations = 0;
  std::size_t                     decryptionAllocations = 0;

  {
    AllocationFailure               tracker(std::numeric_limits<std::size_t>::max());
    const std::vector<std::uint8_t> result = encryptEmip3(plaintext, "password");

    encryptionAllocations = allocationIndex;
    EXPECT_EQ(result.size(), plaintext.size() + 60);
  }
  {
    AllocationFailure               tracker(std::numeric_limits<std::size_t>::max());
    const std::vector<std::uint8_t> result = decryptEmip3(encrypted, "password");

    decryptionAllocations = allocationIndex;
    EXPECT_EQ(result, plaintext);
  }

  ASSERT_GT(encryptionAllocations, 0U);
  ASSERT_GT(decryptionAllocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < encryptionAllocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW(encryptEmip3(plaintext, "password"), std::bad_alloc);
  }
  for (std::size_t index = 0; index < decryptionAllocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW(decryptEmip3(encrypted, "password"), std::bad_alloc);
  }

  EXPECT_EQ(decryptEmip3(encrypted, "password"), plaintext);
}

TEST(Address, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address    source      = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  std::size_t      allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const Address     copy(source);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Address           destination = source;
    const std::string before      = destination.toString();

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), before);
    EXPECT_EQ(source, destination);
  }
}

TEST(Address, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::size_t      allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const Address     address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW((void)EnterpriseAddress(NetworkId::Testnet, credential).toAddress(), std::exception);
  }
}

TEST(BaseAddress, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential  credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const BaseAddress source      = BaseAddress(NetworkId::Testnet, credential, credential);
  std::size_t       allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const BaseAddress copy(source);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    BaseAddress       destination = source;
    const std::string before      = destination.toString();

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), before);
    EXPECT_EQ(source, destination);
  }
}

TEST(BaseAddress, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::size_t      allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const BaseAddress address = BaseAddress(NetworkId::Testnet, credential, credential);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW((void)BaseAddress(NetworkId::Testnet, credential, credential), std::exception);
  }
}

TEST(EnterpriseAddress, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential        credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const EnterpriseAddress source      = EnterpriseAddress(NetworkId::Testnet, credential);
  std::size_t             allocations = 0;

  {
    AllocationFailure       tracker(std::numeric_limits<std::size_t>::max());
    const EnterpriseAddress copy(source);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    EnterpriseAddress destination = source;
    const std::string before      = destination.toString();

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), before);
    EXPECT_EQ(source, destination);
  }
}

TEST(EnterpriseAddress, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::size_t      allocations = 0;

  {
    AllocationFailure       tracker(std::numeric_limits<std::size_t>::max());
    const EnterpriseAddress address = EnterpriseAddress(NetworkId::Testnet, credential);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW((void)EnterpriseAddress(NetworkId::Testnet, credential), std::exception);
  }
}

TEST(RewardAddress, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential    credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const RewardAddress source      = RewardAddress(NetworkId::Testnet, credential);
  std::size_t         allocations = 0;

  {
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max());
    const RewardAddress copy(source);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    RewardAddress     destination = source;
    const std::string before      = destination.toString();

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), before);
    EXPECT_EQ(source, destination);
  }
}

TEST(RewardAddress, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::size_t      allocations = 0;

  {
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max());
    const RewardAddress address = RewardAddress(NetworkId::Testnet, credential);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW((void)RewardAddress(NetworkId::Testnet, credential), std::exception);
  }
}

TEST(PointerAddress, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential     credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const PointerAddress source      = PointerAddress(NetworkId::Testnet, credential, { 42, 1, 2 });
  std::size_t          allocations = 0;

  {
    AllocationFailure    tracker(std::numeric_limits<std::size_t>::max());
    const PointerAddress copy(source);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    PointerAddress    destination = source;
    const std::string before      = destination.toString();

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), before);
    EXPECT_EQ(source, destination);
  }
}

TEST(PointerAddress, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::size_t      allocations = 0;

  {
    AllocationFailure    tracker(std::numeric_limits<std::size_t>::max());
    const PointerAddress address = PointerAddress(NetworkId::Testnet, credential, { 42, 1, 2 });

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW((void)PointerAddress(NetworkId::Testnet, credential, { 42, 1, 2 }), std::exception);
  }
}

TEST(ByronAddress, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential   credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const ByronAddress source      = ByronAddress(credential.hash(), {}, ByronAddressType::PublicKey);
  std::size_t        allocations = 0;

  {
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max());
    const ByronAddress copy(source);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    ByronAddress      destination = source;
    const std::string before      = destination.toString();

    {
      AllocationFailure fail(index);

      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination.toString(), before);
    EXPECT_EQ(source, destination);
  }
}

TEST(ByronAddress, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::size_t      allocations = 0;

  {
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max());
    const ByronAddress address = ByronAddress(credential.hash(), {}, ByronAddressType::PublicKey);

    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);

    EXPECT_THROW((void)ByronAddress(credential.hash(), {}, ByronAddressType::PublicKey), std::exception);
  }
}

TEST(Address, OptionalConversionsPropagateAllocationFailures)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address    address = BaseAddress(NetworkId::Testnet, credential, credential).toAddress();

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(address.asBase(), std::bad_alloc);
  }

  EXPECT_TRUE(address.asBase().has_value());
}

TEST(BaseAddress, FailedCredentialCopyDoesNotMutateAddress)
{
  // Arrange
  const Credential  credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const BaseAddress address(NetworkId::Testnet, credential, credential);

  // Act & Assert
  {
    AllocationFailure fail(0);

    EXPECT_THROW(address.paymentCredential(), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);

    EXPECT_THROW(address.stakeCredential(), std::bad_alloc);
  }

  EXPECT_EQ(address.paymentCredential(), credential);
  EXPECT_EQ(address.stakeCredential(), credential);
}

TEST(AssetId, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const AssetId source      = AssetId::fromHex(std::string(56, '1') + "0100");
  std::size_t   allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const AssetId     copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetId destination = AssetId::lovelace();

    {
      AllocationFailure fail(index);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_TRUE(destination.isLovelace());
    EXPECT_EQ(source.toHex(), std::string(56, '1') + "0100");
  }
}

TEST(AssetId, ComponentConstructionReleasesEveryPartialAllocation)
{
  // Arrange
  const Blake2bHash policy(std::vector<std::uint8_t>(28, 1));
  const AssetName   name        = AssetName::fromString("test");
  std::size_t       allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const AssetId     value(policy, name);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);
    EXPECT_THROW((void)AssetId(policy, name), std::bad_alloc);
  }
}

TEST(AssetId, GettersPropagateCopyFailures)
{
  // Arrange
  const AssetId value = AssetId::fromHex(std::string(56, '1') + "01");

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(value.policyId(), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(value.assetName(), std::bad_alloc);
  }

  EXPECT_EQ(value.policyId()->toHex(), std::string(56, '1'));
  EXPECT_EQ(value.assetName()->toHex(), "01");
}

TEST(AssetId, ByteParsingReleasesEveryPartialAllocation)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(60, 1);
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const AssetId     value(bytes);
    allocations = allocationIndex;
  }

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index);
    EXPECT_THROW((void)AssetId(bytes), std::bad_alloc);
  }

  {
    AllocationFailure fail(0);
    EXPECT_THROW(AssetId::lovelace(), std::bad_alloc);
  }
}

TEST(AssetNameMap, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max(), true);
    const AssetNameMap copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetNameMap       destination = AssetNameMap();
    const AssetNameMap before(destination);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination, before);
  }
}

TEST(AssetNameMap, FailedAdditionPreservesDestination)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AssetNameMap      destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination += source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetNameMap destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination += source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(AssetNameMap, FailedSubtractionPreservesDestination)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AssetNameMap      destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination -= source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetNameMap destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination -= source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(AssetNameMap, FailedKeyRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.keys();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.keys(), std::exception);
  }
}

TEST(AssetNameMap, FailedEntryRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.entries();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.entries(), std::exception);
  }
}

TEST(AssetNameMap, FailedCborSerializationReleasesPartialResults)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toCbor(), std::exception);
  }
}

TEST(AssetNameMap, FailedJsonSerializationReleasesPartialResults)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toJson(), std::exception);
  }
}

TEST(AssetNameMap, FailedParsingReleasesPartialResults)
{
  // Arrange
  const AssetName                 name = AssetName::fromHex("01");
  const AssetNameMap              source { { name, 3 } };
  const std::vector<std::uint8_t> cbor        = source.toCbor();
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)AssetNameMap::fromCbor(cbor);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(AssetNameMap::fromCbor(cbor), std::exception);
  }
}

TEST(AssetNameMap, FailedInsertionPreservesOriginalValue)
{
  // Arrange
  const AssetName    name = AssetName::fromHex("01");
  const AssetNameMap source { { name, 3 } };
  std::size_t        allocations = 0;

  {
    AssetNameMap      destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.set(AssetName::fromHex("02"), 5);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetNameMap destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.set(AssetName::fromHex("02"), 5), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(AssetIdMap, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap source { { id, 3 } };
  std::size_t      allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    const AssetIdMap  copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetIdMap       destination = AssetIdMap();
    const AssetIdMap before(destination);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination, before);
  }
}

TEST(AssetIdMap, FailedAdditionPreservesDestination)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap source { { id, 3 } };
  std::size_t      allocations = 0;

  {
    AssetIdMap        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination += source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetIdMap destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination += source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(AssetIdMap, FailedSubtractionPreservesDestination)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap source { { id, 3 } };
  std::size_t      allocations = 0;

  {
    AssetIdMap        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination -= source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetIdMap destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination -= source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(AssetIdMap, FailedKeyRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap source { { id, 3 } };
  std::size_t      allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.keys();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.keys(), std::exception);
  }
}

TEST(AssetIdMap, FailedEntryRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap source { { id, 3 } };
  std::size_t      allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.entries();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.entries(), std::exception);
  }
}

TEST(AssetIdMap, FailedInsertionPreservesOriginalValue)
{
  // Arrange
  const AssetId    id = AssetId::fromHex(std::string(56, '1'));
  const AssetIdMap source { { id, 3 } };
  std::size_t      allocations = 0;

  {
    AssetIdMap        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.set(AssetId::lovelace(), 5);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AssetIdMap destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.set(AssetId::lovelace(), 5), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(MultiAsset, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    const MultiAsset  copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    MultiAsset       destination = MultiAsset();
    const MultiAsset before(destination);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination, before);
  }
}

TEST(MultiAsset, FailedAdditionPreservesDestination)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    MultiAsset        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination += source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    MultiAsset destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination += source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(MultiAsset, FailedSubtractionPreservesDestination)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    MultiAsset        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination -= source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    MultiAsset destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination -= source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(MultiAsset, FailedPolicyRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.policies();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.policies(), std::exception);
  }
}

TEST(MultiAsset, FailedAssetRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.assets(*id.policyId());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.assets(*id.policyId()), std::exception);
  }
}

TEST(MultiAsset, FailedCborSerializationReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toCbor(), std::exception);
  }
}

TEST(MultiAsset, FailedJsonSerializationReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toJson(), std::exception);
  }
}

TEST(MultiAsset, FailedParsingReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  const std::vector<std::uint8_t> cbor        = source.toCbor();
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)MultiAsset::fromCbor(cbor);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(MultiAsset::fromCbor(cbor), std::exception);
  }
}

TEST(MultiAsset, FailedAssetUpdatePreservesOriginalValue)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    MultiAsset        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.set(AssetId::fromHex(std::string(56, '2')), 5);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    MultiAsset destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.set(AssetId::fromHex(std::string(56, '2')), 5), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(MultiAsset, FailedPolicyReplacementPreservesOriginalValue)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  MultiAsset    source;
  source.set(id, 3);
  std::size_t allocations = 0;

  {
    MultiAsset        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.setAssets(*id.policyId(), AssetNameMap());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    MultiAsset destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.setAssets(*id.policyId(), AssetNameMap()), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Value, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    const Value       copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value       destination = Value();
    const Value before(destination);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_EQ(destination, before);
  }
}

TEST(Value, FailedAdditionPreservesDestination)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    Value             destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination += source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination += source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Value, FailedSubtractionPreservesDestination)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    Value             destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination -= source;
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination -= source, std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Value, FailedAssetRetrievalReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.assets();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.assets(), std::exception);
  }
}

TEST(Value, FailedAssetMapConversionReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toAssetMap();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toAssetMap(), std::exception);
  }
}

TEST(Value, FailedCborSerializationReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toCbor(), std::exception);
  }
}

TEST(Value, FailedJsonSerializationReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toJson(), std::exception);
  }
}

TEST(Value, FailedParsingReleasesPartialResults)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  const std::vector<std::uint8_t> cbor        = source.toCbor();
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)Value::fromCbor(cbor);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(Value::fromCbor(cbor), std::exception);
  }
}

TEST(Value, FailedAssetAdditionPreservesOriginalValue)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    Value             destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.addAsset(id, 5);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.addAsset(id, 5), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Value, FailedAssetReplacementPreservesOriginalValue)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    Value             destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.setAssets(MultiAsset());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.setAssets(MultiAsset()), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Value, FailedAssetCollectionAdditionPreservesOriginalValue)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    Value             destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.addAssets(source.assets());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.addAssets(source.assets()), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Value, FailedAssetCollectionSubtractionPreservesOriginalValue)
{
  // Arrange
  const AssetId id = AssetId::fromHex(std::string(56, '1'));
  Value         source(7);
  source.addAsset(id, 3);
  std::size_t allocations = 0;

  {
    Value             destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.subtractAssets(source.assets());
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Value destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.subtractAssets(source.assets()), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Blake2bHashSet, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  std::size_t          allocations = 0;

  {
    AllocationFailure    tracker(std::numeric_limits<std::size_t>::max(), true);
    const Blake2bHashSet copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Blake2bHashSet destination;

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_TRUE(destination.empty());
    EXPECT_EQ(source.size(), 1U);
  }
}

TEST(Blake2bHashSet, FailedInsertionPreservesElements)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  const Blake2bHash    second      = Blake2bHash::fromHex("02");
  std::size_t          allocations = 0;

  {
    Blake2bHashSet    destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    EXPECT_TRUE(destination.insert(second));
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    Blake2bHashSet destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.insert(second), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(Blake2bHashSet, FailedElementRetrievalReleasesPartialResults)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  std::size_t          allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.at(0);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.at(0), std::exception);
  }
}

TEST(Blake2bHashSet, FailedSnapshotRetrievalReleasesPartialResults)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  std::size_t          allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.values();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.values(), std::exception);
  }
}

TEST(Blake2bHashSet, FailedCborEncodingReleasesPartialResults)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  std::size_t          allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toCbor(), std::exception);
  }
}

TEST(Blake2bHashSet, FailedJsonEncodingReleasesPartialResults)
{
  // Arrange
  const Blake2bHash    first = Blake2bHash::fromHex("01");
  const Blake2bHashSet source { first };
  std::size_t          allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toJson(), std::exception);
  }
}

TEST(Blake2bHashSet, FailedCborParsingReleasesPartialResults)
{
  // Arrange
  const Blake2bHash               first = Blake2bHash::fromHex("01");
  const Blake2bHashSet            source { first };
  const std::vector<std::uint8_t> cbor        = source.toCbor();
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)Blake2bHashSet::fromCbor(cbor);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(Blake2bHashSet::fromCbor(cbor), std::exception);
  }
}

TEST(CredentialSet, FailedCopiesPreserveDestinationAndReleasePartialState)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  std::size_t         allocations = 0;

  {
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max(), true);
    const CredentialSet copy(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    CredentialSet destination;

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }

    EXPECT_TRUE(destination.empty());
    EXPECT_EQ(source.size(), 1U);
  }
}

TEST(CredentialSet, FailedInsertionPreservesElements)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  const Credential    second      = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '2')));
  std::size_t         allocations = 0;

  {
    CredentialSet     destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    EXPECT_TRUE(destination.insert(second));
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    CredentialSet destination(source);

    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.insert(second), std::exception);
    }

    EXPECT_EQ(destination, source);
  }
}

TEST(CredentialSet, FailedElementRetrievalReleasesPartialResults)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  std::size_t         allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.at(0);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.at(0), std::exception);
  }
}

TEST(CredentialSet, FailedSnapshotRetrievalReleasesPartialResults)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  std::size_t         allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.values();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.values(), std::exception);
  }
}

TEST(CredentialSet, FailedCborEncodingReleasesPartialResults)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  std::size_t         allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toCbor(), std::exception);
  }
}

TEST(CredentialSet, FailedJsonEncodingReleasesPartialResults)
{
  // Arrange
  const Credential    first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet source { first };
  std::size_t         allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(source.toJson(), std::exception);
  }
}

TEST(CredentialSet, FailedCborParsingReleasesPartialResults)
{
  // Arrange
  const Credential                first = Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const CredentialSet             source { first };
  const std::vector<std::uint8_t> cbor        = source.toCbor();
  std::size_t                     allocations = 0;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)CredentialSet::fromCbor(cbor);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW(CredentialSet::fromCbor(cbor), std::exception);
  }
}

TEST(PlutusData, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const PlutusData source      = sampleValue<PlutusData>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(PlutusData, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const PlutusData source      = sampleValue<PlutusData>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(PlutusData, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const PlutusData source      = sampleValue<PlutusData>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)PlutusData(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)PlutusData(source), std::exception);
  }
}

TEST(PlutusList, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const PlutusList source      = sampleValue<PlutusList>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(PlutusList, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const PlutusList source      = sampleValue<PlutusList>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(PlutusList, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const PlutusList source      = sampleValue<PlutusList>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)PlutusList(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)PlutusList(source), std::exception);
  }
}

TEST(PlutusMap, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const PlutusMap source      = sampleValue<PlutusMap>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(PlutusMap, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const PlutusMap source      = sampleValue<PlutusMap>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(PlutusMap, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const PlutusMap source      = sampleValue<PlutusMap>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)PlutusMap(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)PlutusMap(source), std::exception);
  }
}

TEST(ConstrPlutusData, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ConstrPlutusData source      = sampleValue<ConstrPlutusData>();
  std::size_t            allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ConstrPlutusData, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ConstrPlutusData source      = sampleValue<ConstrPlutusData>();
  std::size_t            allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ConstrPlutusData, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const ConstrPlutusData source      = sampleValue<ConstrPlutusData>();
  std::size_t            allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ConstrPlutusData(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ConstrPlutusData(source), std::exception);
  }
}

TEST(Datum, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const Datum source      = sampleValue<Datum>();
  std::size_t allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(Datum, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const Datum source      = sampleValue<Datum>();
  std::size_t allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(Datum, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const Datum source      = sampleValue<Datum>();
  std::size_t allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)Datum(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)Datum(source), std::exception);
  }
}

TEST(PlutusList, FailedMutationPreservesValue)
{
  // Arrange
  const PlutusList source = sampleValue<PlutusList>();
  const PlutusData value(9);
  std::size_t      allocations = 0U;

  {
    PlutusList        destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.pushBack(value);
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    PlutusList destination(source);
    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.pushBack(value), std::exception);
    }
    EXPECT_EQ(destination, source);
    EXPECT_EQ(destination.toCbor(), source.toCbor());
  }
}

TEST(PlutusMap, FailedMutationPreservesValue)
{
  // Arrange
  const PlutusMap  source = sampleValue<PlutusMap>();
  const PlutusData key(9);
  const PlutusData value(10);
  std::size_t      allocations = 0U;

  {
    PlutusMap         destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.insert(key, value);
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    PlutusMap destination(source);
    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.insert(key, value), std::exception);
    }
    EXPECT_EQ(destination, source);
    EXPECT_EQ(destination.toCbor(), source.toCbor());
  }
}

TEST(ConstrPlutusData, FailedMutationPreservesValue)
{
  // Arrange
  const ConstrPlutusData source      = sampleValue<ConstrPlutusData>();
  const PlutusList       fields      = PlutusList::fromCbor("8109");
  std::size_t            allocations = 0U;

  {
    ConstrPlutusData  destination(source);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    destination.setFields(fields);
    allocations = allocationIndex;
    EXPECT_NE(destination, source);
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    ConstrPlutusData destination(source);
    {
      AllocationFailure fail(index, true);
      EXPECT_THROW(destination.setFields(fields), std::exception);
    }
    EXPECT_EQ(destination, source);
    EXPECT_EQ(destination.toCbor(), source.toCbor());
  }
}

TEST(ScriptPubkey, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptPubkey source      = ScriptPubkey(Blake2bHash::fromHex(std::string(56, '1')));
  std::size_t        allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptPubkey, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptPubkey source      = ScriptPubkey(Blake2bHash::fromHex(std::string(56, '1')));
  std::size_t        allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptPubkey, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = "{\"type\":\"sig\",\"keyHash\":\"" + std::string(56, '1') + "\"}";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptPubkey::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptPubkey::fromJson(json), std::exception);
  }
}

TEST(ScriptInvalidBefore, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptInvalidBefore source      = ScriptInvalidBefore(42);
  std::size_t               allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptInvalidBefore, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptInvalidBefore source      = ScriptInvalidBefore(42);
  std::size_t               allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptInvalidBefore, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = "{\"type\":\"after\",\"slot\":42}";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptInvalidBefore::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptInvalidBefore::fromJson(json), std::exception);
  }
}

TEST(ScriptInvalidAfter, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptInvalidAfter source      = ScriptInvalidAfter(42);
  std::size_t              allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptInvalidAfter, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptInvalidAfter source      = ScriptInvalidAfter(42);
  std::size_t              allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptInvalidAfter, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = "{\"type\":\"before\",\"slot\":42}";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptInvalidAfter::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptInvalidAfter::fromJson(json), std::exception);
  }
}

TEST(ScriptRequireGuard, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptRequireGuard source      = ScriptRequireGuard(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
  std::size_t              allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptRequireGuard, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptRequireGuard source      = ScriptRequireGuard(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))));
  std::size_t              allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptRequireGuard, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = "{\"type\":\"guard\",\"keyHash\":\"" + std::string(56, '1') + "\"}";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptRequireGuard::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptRequireGuard::fromJson(json), std::exception);
  }
}

TEST(NativeScript, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const NativeScript source      = sampleValue<NativeScript>();
  std::size_t        allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(NativeScript, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const NativeScript source      = sampleValue<NativeScript>();
  std::size_t        allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(NativeScript, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = R"({"type":"after","slot":5})";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)NativeScript::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)NativeScript::fromJson(json), std::exception);
  }
}

TEST(NativeScript, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const NativeScript source      = sampleValue<NativeScript>();
  std::size_t        allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)NativeScript(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)NativeScript(source), std::exception);
  }
}

TEST(NativeScriptList, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const NativeScriptList source      = sampleValue<NativeScriptList>();
  std::size_t            allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(NativeScriptList, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const NativeScriptList source      = sampleValue<NativeScriptList>();
  std::size_t            allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(NativeScriptList, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = R"({"scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)NativeScriptList::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)NativeScriptList::fromJson(json), std::exception);
  }
}

TEST(NativeScriptList, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const NativeScriptList source      = sampleValue<NativeScriptList>();
  std::size_t            allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)NativeScriptList(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)NativeScriptList(source), std::exception);
  }
}

TEST(ScriptAll, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptAll source      = sampleValue<ScriptAll>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptAll, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptAll source      = sampleValue<ScriptAll>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptAll, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = R"({"type":"all","scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptAll::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptAll::fromJson(json), std::exception);
  }
}

TEST(ScriptAll, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const ScriptAll source      = sampleValue<ScriptAll>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptAll(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptAll(source), std::exception);
  }
}

TEST(ScriptAny, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptAny source      = sampleValue<ScriptAny>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptAny, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptAny source      = sampleValue<ScriptAny>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptAny, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = R"({"type":"any","scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptAny::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptAny::fromJson(json), std::exception);
  }
}

TEST(ScriptAny, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const ScriptAny source      = sampleValue<ScriptAny>();
  std::size_t     allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptAny(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptAny(source), std::exception);
  }
}

TEST(ScriptNOfK, FailedCborEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptNOfK source      = sampleValue<ScriptNOfK>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toCbor();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toCbor(), std::exception);
  }
}

TEST(ScriptNOfK, FailedJsonEncodingReleasesEveryAllocation)
{
  // Arrange
  const ScriptNOfK source      = sampleValue<ScriptNOfK>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toJson();
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)source.toJson(), std::exception);
  }
}

TEST(ScriptNOfK, FailedJsonParsingReleasesEveryAllocation)
{
  // Arrange
  const std::string json        = R"({"type":"atLeast","required":1,"scripts":[{"type":"after","slot":5},{"type":"before","slot":7}]})";
  std::size_t       allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptNOfK::fromJson(json);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptNOfK::fromJson(json), std::exception);
  }
}

TEST(ScriptNOfK, FailedCopyReleasesEveryAllocation)
{
  // Arrange
  const ScriptNOfK source      = sampleValue<ScriptNOfK>();
  std::size_t      allocations = 0U;

  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)ScriptNOfK(source);
    allocations = allocationIndex;
  }

  ASSERT_GT(allocations, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < allocations; ++index)
  {
    AllocationFailure fail(index, true);
    EXPECT_THROW((void)ScriptNOfK(source), std::exception);
  }
}

TEST(PlutusV1Script, ReleasesEveryAllocationDuringParameterApplication)
{
  // Arrange
  const PlutusV1Script script     = PlutusV1Script::fromHex("450100002499");
  const PlutusList     parameters = PlutusList::fromCbor("820041ff");
  std::size_t          count      = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.applyParameters(parameters);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.applyParameters(parameters), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV1Script, ReleasesEveryAllocationDuringInspection)
{
  // Arrange
  const PlutusV1Script script = PlutusV1Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.toJson();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.toJson(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV1Script, ReleasesEveryAllocationDuringHashing)
{
  // Arrange
  const PlutusV1Script script = PlutusV1Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.hash();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.hash(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV2Script, ReleasesEveryAllocationDuringParameterApplication)
{
  // Arrange
  const PlutusV2Script script     = PlutusV2Script::fromHex("450100002499");
  const PlutusList     parameters = PlutusList::fromCbor("820041ff");
  std::size_t          count      = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.applyParameters(parameters);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.applyParameters(parameters), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV2Script, ReleasesEveryAllocationDuringInspection)
{
  // Arrange
  const PlutusV2Script script = PlutusV2Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.toJson();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.toJson(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV2Script, ReleasesEveryAllocationDuringHashing)
{
  // Arrange
  const PlutusV2Script script = PlutusV2Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.hash();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.hash(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV3Script, ReleasesEveryAllocationDuringParameterApplication)
{
  // Arrange
  const PlutusV3Script script     = PlutusV3Script::fromHex("450100002499");
  const PlutusList     parameters = PlutusList::fromCbor("820041ff");
  std::size_t          count      = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.applyParameters(parameters);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.applyParameters(parameters), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV3Script, ReleasesEveryAllocationDuringInspection)
{
  // Arrange
  const PlutusV3Script script = PlutusV3Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.toJson();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.toJson(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV3Script, ReleasesEveryAllocationDuringHashing)
{
  // Arrange
  const PlutusV3Script script = PlutusV3Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.hash();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.hash(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV4Script, ReleasesEveryAllocationDuringParameterApplication)
{
  // Arrange
  const PlutusV4Script script     = PlutusV4Script::fromHex("450100002499");
  const PlutusList     parameters = PlutusList::fromCbor("820041ff");
  std::size_t          count      = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.applyParameters(parameters);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.applyParameters(parameters), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV4Script, ReleasesEveryAllocationDuringInspection)
{
  // Arrange
  const PlutusV4Script script = PlutusV4Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.toJson();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.toJson(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(PlutusV4Script, ReleasesEveryAllocationDuringHashing)
{
  // Arrange
  const PlutusV4Script script = PlutusV4Script::fromHex("450100002499");
  std::size_t          count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.hash();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.hash(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.toHex(), "450100002499");
}

TEST(Script, ReleasesEveryAllocationDuringNativeAccess)
{
  // Arrange
  const Script script { NativeScript(ScriptInvalidBefore(5)) };
  std::size_t  count = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.asNative();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.asNative(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.language(), ScriptLanguage::Native);
}

TEST(Script, ReleasesEveryAllocationDuringPlutusV1Access)
{
  // Arrange
  const Script script { PlutusV1Script::fromHex("450100002499") };
  std::size_t  count = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.asPlutusV1();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.asPlutusV1(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.language(), ScriptLanguage::PlutusV1);
}

TEST(Script, ReleasesEveryAllocationDuringPlutusV2Access)
{
  // Arrange
  const Script script { PlutusV2Script::fromHex("450100002499") };
  std::size_t  count = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.asPlutusV2();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.asPlutusV2(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.language(), ScriptLanguage::PlutusV2);
}

TEST(Script, ReleasesEveryAllocationDuringPlutusV3Access)
{
  // Arrange
  const Script script { PlutusV3Script::fromHex("450100002499") };
  std::size_t  count = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.asPlutusV3();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.asPlutusV3(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.language(), ScriptLanguage::PlutusV3);
}

TEST(Script, ReleasesEveryAllocationDuringPlutusV4Access)
{
  // Arrange
  const Script script { PlutusV4Script::fromHex("450100002499") };
  std::size_t  count = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)script.asPlutusV4();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(script.asPlutusV4(), std::bad_alloc) << index;
  }

  EXPECT_EQ(script.language(), ScriptLanguage::PlutusV4);
}

TEST(TransactionOutput, FailedDatumPreservesValue)
{
  // Arrange
  const TransactionOutput original    = sampleValue<TransactionOutput>();
  const Datum             replacement = Datum(PlutusData(std::int64_t { 7 }));
  std::size_t             count       = 0U;
  {
    TransactionOutput value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.setDatum(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    TransactionOutput value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setDatum(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(TransactionOutput, FailedScriptRefPreservesValue)
{
  // Arrange
  const TransactionOutput original    = sampleValue<TransactionOutput>();
  const Script            replacement = Script(PlutusV3Script::fromHex("450100002499"));
  std::size_t             count       = 0U;
  {
    TransactionOutput value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.setScriptRef(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    TransactionOutput value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setScriptRef(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(TransactionOutput, FailedAmountPreservesValue)
{
  // Arrange
  const TransactionOutput original    = sampleValue<TransactionOutput>();
  const Value             replacement = Value(20);
  std::size_t             count       = 0U;
  {
    TransactionOutput value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.setAmount(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    TransactionOutput value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setAmount(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(Utxo, FailedInputPreservesValue)
{
  // Arrange
  const Utxo             original    = sampleValue<Utxo>();
  const TransactionInput replacement = TransactionInput::fromHex(std::string(64, '2'), 8);
  std::size_t            count       = 0U;
  {
    Utxo              value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.setInput(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    Utxo value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setInput(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(Utxo, FailedOutputPreservesValue)
{
  // Arrange
  const Utxo              original    = sampleValue<Utxo>();
  const TransactionOutput replacement = TransactionOutput(sampleValue<TransactionOutput>().address(), Value(20));
  std::size_t             count       = 0U;
  {
    Utxo              value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.setOutput(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    Utxo value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setOutput(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(TransactionInputSet, FailedInsertionPreservesValue)
{
  // Arrange
  const TransactionInputSet original    = sampleValue<TransactionInputSet>();
  const TransactionInput    replacement = TransactionInput::fromHex(std::string(64, '2'), 8);
  std::size_t               count       = 0U;
  {
    TransactionInputSet value(original);
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.insert(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    TransactionInputSet value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.insert(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(TransactionOutputList, FailedAppendPreservesValue)
{
  // Arrange
  const TransactionOutputList original    = sampleValue<TransactionOutputList>();
  const TransactionOutput     replacement = sampleValue<TransactionOutput>();
  std::size_t                 count       = 0U;
  {
    TransactionOutputList value(original);
    AllocationFailure     tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    TransactionOutputList value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(WithdrawalMap, FailedReplacementPreservesAllAmounts)
{
  // Arrange
  const WithdrawalMap original = sampleValue<WithdrawalMap>();
  const RewardAddress key      = original.keys().at(0);
  std::size_t         count    = 0U;
  {
    WithdrawalMap     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.set(key, 20);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    WithdrawalMap value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.set(key, 20), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(PlutusV1ScriptList, FailedAppendPreservesValue)
{
  // Arrange
  const PlutusV1ScriptList original    = sampleValue<PlutusV1ScriptList>();
  const PlutusV1Script     replacement = sampleValue<PlutusV1Script>();
  std::size_t              count       = 0U;
  {
    PlutusV1ScriptList value(original);
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    PlutusV1ScriptList value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(PlutusV2ScriptList, FailedAppendPreservesValue)
{
  // Arrange
  const PlutusV2ScriptList original    = sampleValue<PlutusV2ScriptList>();
  const PlutusV2Script     replacement = sampleValue<PlutusV2Script>();
  std::size_t              count       = 0U;
  {
    PlutusV2ScriptList value(original);
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    PlutusV2ScriptList value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(PlutusV3ScriptList, FailedAppendPreservesValue)
{
  // Arrange
  const PlutusV3ScriptList original    = sampleValue<PlutusV3ScriptList>();
  const PlutusV3Script     replacement = sampleValue<PlutusV3Script>();
  std::size_t              count       = 0U;
  {
    PlutusV3ScriptList value(original);
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    PlutusV3ScriptList value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(PlutusV4ScriptList, FailedAppendPreservesValue)
{
  // Arrange
  const PlutusV4ScriptList original    = sampleValue<PlutusV4ScriptList>();
  const PlutusV4Script     replacement = sampleValue<PlutusV4Script>();
  std::size_t              count       = 0U;
  {
    PlutusV4ScriptList value(original);
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    PlutusV4ScriptList value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(MetadatumList, FailedAppendPreservesValue)
{
  // Arrange
  const MetadatumList original    = sampleValue<MetadatumList>();
  const Metadatum     replacement = sampleValue<Metadatum>();
  std::size_t         count       = 0U;
  {
    MetadatumList     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    MetadatumList value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
  }
}

TEST(Metadatum, PlainJsonReleasesTemporaryValuesOnAllocationFailure)
{
  // Arrange
  const std::string json   = R"({"a":[42,"text"],"b":18446744073709551616})";
  const Metadatum   source = Metadatum::fromPlainJson(json);
  std::size_t       count  = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)source.toPlainJson();
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(source.toPlainJson(), std::bad_alloc) << index;
  }
  EXPECT_EQ(source.toPlainJson(), json);
}

TEST(Metadatum, PlainJsonParsingReleasesPartialValues)
{
  // Arrange
  const std::string json  = R"({"a":[42,"text"],"b":18446744073709551616})";
  std::size_t       count = 0U;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    (void)Metadatum::fromPlainJson(json);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AllocationFailure failure(index, true);
    EXPECT_THROW(Metadatum::fromPlainJson(json), std::bad_alloc) << index;
  }
}

TEST(MetadatumMap, FailedInsertPreservesEntries)
{
  // Arrange
  const MetadatumMap original = sampleValue<MetadatumMap>();
  const Metadatum    key("new");
  const Metadatum    replacement = sampleValue<Metadatum>();
  std::size_t        count       = 0U;
  {
    MetadatumMap      map(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    map.insert(key, replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    MetadatumMap map(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(map.insert(key, replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(map, original);
  }
}

TEST(TransactionMetadata, FailedSetPreservesValue)
{
  // Arrange
  const TransactionMetadata original = sampleValue<TransactionMetadata>();
  const Metadatum           replacement("updated");
  std::size_t               count = 0U;
  {
    TransactionMetadata value(original);
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max(), true);
    value.set(1, replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    TransactionMetadata value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.set(1, replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(AuxiliaryData, FailedSetMetadataPreservesValue)
{
  // Arrange
  const AuxiliaryData       original    = sampleValue<AuxiliaryData>();
  const TransactionMetadata replacement = *original.metadata();
  std::size_t               count       = 0U;
  {
    AuxiliaryData     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setMetadata(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AuxiliaryData value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setMetadata(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(AuxiliaryData, FailedSetNativeScriptsPreservesValue)
{
  // Arrange
  const AuxiliaryData    original    = sampleValue<AuxiliaryData>();
  const NativeScriptList replacement = *original.nativeScripts();
  std::size_t            count       = 0U;
  {
    AuxiliaryData     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setNativeScripts(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AuxiliaryData value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setNativeScripts(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(AuxiliaryData, FailedSetPlutusV1ScriptsPreservesValue)
{
  // Arrange
  const AuxiliaryData      original    = sampleValue<AuxiliaryData>();
  const PlutusV1ScriptList replacement = *original.plutusV1Scripts();
  std::size_t              count       = 0U;
  {
    AuxiliaryData     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setPlutusV1Scripts(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AuxiliaryData value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setPlutusV1Scripts(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(AuxiliaryData, FailedSetPlutusV2ScriptsPreservesValue)
{
  // Arrange
  const AuxiliaryData      original    = sampleValue<AuxiliaryData>();
  const PlutusV2ScriptList replacement = *original.plutusV2Scripts();
  std::size_t              count       = 0U;
  {
    AuxiliaryData     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setPlutusV2Scripts(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AuxiliaryData value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setPlutusV2Scripts(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(AuxiliaryData, FailedSetPlutusV3ScriptsPreservesValue)
{
  // Arrange
  const AuxiliaryData      original    = sampleValue<AuxiliaryData>();
  const PlutusV3ScriptList replacement = *original.plutusV3Scripts();
  std::size_t              count       = 0U;
  {
    AuxiliaryData     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setPlutusV3Scripts(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AuxiliaryData value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setPlutusV3Scripts(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(AuxiliaryData, FailedSetPlutusV4ScriptsPreservesValue)
{
  // Arrange
  const AuxiliaryData      original    = sampleValue<AuxiliaryData>();
  const PlutusV4ScriptList replacement = *original.plutusV4Scripts();
  std::size_t              count       = 0U;
  {
    AuxiliaryData     value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setPlutusV4Scripts(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    AuxiliaryData value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setPlutusV4Scripts(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(PoolMetadata, FailedSetHashPreservesValue)
{
  // Arrange
  const PoolMetadata original    = sampleValue<PoolMetadata>();
  const Blake2bHash  replacement = Blake2bHash::fromHex(std::string(64, '2'));
  std::size_t        count       = 0U;
  {
    PoolMetadata      value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setHash(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    PoolMetadata value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setHash(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(SingleHostAddrRelay, FailedSetIpv4PreservesValue)
{
  // Arrange
  const SingleHostAddrRelay original    = sampleValue<SingleHostAddrRelay>();
  const std::optional<Ipv4> replacement = Ipv4::fromString("192.0.2.1");
  std::size_t               count       = 0U;
  {
    SingleHostAddrRelay value(original);
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setIpv4(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    SingleHostAddrRelay value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setIpv4(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(SingleHostAddrRelay, FailedSetIpv6PreservesValue)
{
  // Arrange
  const SingleHostAddrRelay original    = sampleValue<SingleHostAddrRelay>();
  const std::optional<Ipv6> replacement = std::nullopt;
  std::size_t               count       = 0U;
  {
    SingleHostAddrRelay value(original);
    AllocationFailure   tracker(std::numeric_limits<std::size_t>::max(), true);
    value.setIpv6(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    SingleHostAddrRelay value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.setIpv6(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(Relays, FailedPushBackPreservesValue)
{
  // Arrange
  const Relays original    = sampleValue<Relays>();
  const Relay  replacement = Relay(sampleValue<SingleHostNameRelay>());
  std::size_t  count       = 0U;
  {
    Relays            value(original);
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max(), true);
    value.pushBack(replacement);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0U);

  // Act & Assert
  for (std::size_t index = 0U; index < count; ++index)
  {
    Relays value(original);
    {
      AllocationFailure failure(index, true);
      EXPECT_THROW(value.pushBack(replacement), std::bad_alloc) << index;
    }
    EXPECT_EQ(value, original);
    EXPECT_EQ(value.toCbor(), original.toCbor());
  }
}

TEST(SingleHostAddrRelay, FailedPortAllocationPreservesAbsentPort)
{
  // Arrange
  SingleHostAddrRelay       value = SingleHostAddrRelay();
  const SingleHostAddrRelay original(value);

  // Act & Assert
  {
    AllocationFailure failure(0U, true);
    EXPECT_THROW(value.setPort(3001), std::bad_alloc);
  }
  EXPECT_FALSE(value.port());
  EXPECT_EQ(value, original);
}

TEST(SingleHostNameRelay, FailedPortAllocationPreservesAbsentPort)
{
  // Arrange
  SingleHostNameRelay       value = SingleHostNameRelay("example.com");
  const SingleHostNameRelay original(value);

  // Act & Assert
  {
    AllocationFailure failure(0U, true);
    EXPECT_THROW(value.setPort(3001), std::bad_alloc);
  }
  EXPECT_FALSE(value.port());
  EXPECT_EQ(value, original);
}
