/**
 * @file CertificateTest.cpp
 *
 * @brief Unit tests for the ledger certificate variant.
 *
 * @author angel.castillo
 * @date   Nov 02, 2026
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

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Certificate, AuthorizeCommitteeHotMatchesNativeCbor)
{
  // Arrange
  const std::string          cbor  = "830e8200581c000000000000000000000000000000000000000000000000000000008200581c00000000000000000000000000000000000000000000000000000000";
  const AuthCommitteeHotCert inner = AuthCommitteeHotCert::fromCbor(cbor);
  CborReader                 reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::AuthorizeCommitteeHot);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asAuthCommitteeHot(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, GenesisKeyDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string              cbor  = "8405581c00010001000100010001000100010001000100010001000100010001581c0002000200020002000200020002000200020002000200020002000258200003000300030003000300030003000300030003000300030003000300030003";
  const GenesisKeyDelegationCert inner = GenesisKeyDelegationCert::fromCbor(cbor);
  CborReader                     reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::GenesisKeyDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asGenesisKeyDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, MoveInstantaneousRewardsMatchesNativeCbor)
{
  // Arrange
  const std::string cbor  = "820682001a000f4240";
  const MirCert     inner = MirCert::fromCbor(cbor);
  CborReader        reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::MoveInstantaneousRewards);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asMir(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, PoolRegistrationMatchesNativeCbor)
{
  // Arrange
  const std::string          cbor  = "8a03581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef9258208dd154228946bd12967c12bedb1cb6038b78f8b84a1760b1a788fa72a4af3db01927101903e8d81e820105581de1cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810fd9010281581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f8383011913886b6578616d706c652e636f6d8400191770447f000001f682026b6578616d706c652e636f6d827368747470733a2f2f6578616d706c652e636f6d58200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d5";
  const PoolRegistrationCert inner = PoolRegistrationCert::fromCbor(cbor);
  CborReader                 reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::PoolRegistration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asPoolRegistration(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, PoolRetirementMatchesNativeCbor)
{
  // Arrange
  const std::string        cbor  = "8304581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef921903e8";
  const PoolRetirementCert inner = PoolRetirementCert::fromCbor(cbor);
  CborReader               reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::PoolRetirement);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asPoolRetirement(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, DRepRegistrationMatchesNativeCbor)
{
  // Arrange
  const std::string      cbor  = "84108200581c0000000000000000000000000000000000000000000000000000000000f6";
  const RegisterDRepCert inner = RegisterDRepCert::fromCbor(cbor);
  CborReader             reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::DRepRegistration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asRegisterDRep(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, RegistrationMatchesNativeCbor)
{
  // Arrange
  const std::string      cbor  = "83078200581c0000000000000000000000000000000000000000000000000000000000";
  const RegistrationCert inner = RegistrationCert::fromCbor(cbor);
  CborReader             reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::Registration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asRegistration(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, ResignCommitteeColdMatchesNativeCbor)
{
  // Arrange
  const std::string             cbor  = "830f8200581c00000000000000000000000000000000000000000000000000000000f6";
  const ResignCommitteeColdCert inner = ResignCommitteeColdCert::fromCbor(cbor);
  CborReader                    reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::ResignCommitteeCold);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asResignCommitteeCold(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, StakeDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string         cbor  = "83028200581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef92";
  const StakeDelegationCert inner = StakeDelegationCert::fromCbor(cbor);
  CborReader                reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::StakeDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asStakeDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, StakeDeregistrationMatchesNativeCbor)
{
  // Arrange
  const std::string             cbor  = "82018200581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f";
  const StakeDeregistrationCert inner = StakeDeregistrationCert::fromCbor(cbor);
  CborReader                    reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::StakeDeregistration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asStakeDeregistration(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, StakeRegistrationMatchesNativeCbor)
{
  // Arrange
  const std::string           cbor  = "82008200581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f";
  const StakeRegistrationCert inner = StakeRegistrationCert::fromCbor(cbor);
  CborReader                  reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::StakeRegistration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asStakeRegistration(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, StakeRegistrationDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string                     cbor  = "840b8200581c00000000000000000000000000000000000000000000000000000000581c0000000000000000000000000000000000000000000000000000000000";
  const StakeRegistrationDelegationCert inner = StakeRegistrationDelegationCert::fromCbor(cbor);
  CborReader                            reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::StakeRegistrationDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asStakeRegistrationDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, StakeVoteDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string             cbor  = "840a8200581c00000000000000000000000000000000000000000000000000000000581c000000000000000000000000000000000000000000000000000000008200581c00000000000000000000000000000000000000000000000000000000";
  const StakeVoteDelegationCert inner = StakeVoteDelegationCert::fromCbor(cbor);
  CborReader                    reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::StakeVoteDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asStakeVoteDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, StakeVoteRegistrationDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string                         cbor  = "850d8200581c00000000000000000000000000000000000000000000000000000000581c000000000000000000000000000000000000000000000000000000008200581c0000000000000000000000000000000000000000000000000000000000";
  const StakeVoteRegistrationDelegationCert inner = StakeVoteRegistrationDelegationCert::fromCbor(cbor);
  CborReader                                reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::StakeVoteRegistrationDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asStakeVoteRegistrationDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, DRepUnregistrationMatchesNativeCbor)
{
  // Arrange
  const std::string        cbor  = "83118200581c0000000000000000000000000000000000000000000000000000000000";
  const UnregisterDRepCert inner = UnregisterDRepCert::fromCbor(cbor);
  CborReader               reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::DRepUnregistration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asUnregisterDRep(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, UnregistrationMatchesNativeCbor)
{
  // Arrange
  const std::string        cbor  = "83088200581c0000000000000000000000000000000000000000000000000000000000";
  const UnregistrationCert inner = UnregistrationCert::fromCbor(cbor);
  CborReader               reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::Unregistration);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asUnregistration(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, UpdateDRepMatchesNativeCbor)
{
  // Arrange
  const std::string    cbor  = "83128200581c00000000000000000000000000000000000000000000000000000000827668747470733a2f2f7777772e736f6d6575726c2e696f58200000000000000000000000000000000000000000000000000000000000000000";
  const UpdateDRepCert inner = UpdateDRepCert::fromCbor(cbor);
  CborReader           reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::UpdateDRep);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asUpdateDRep(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, VoteDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string        cbor  = "83098200581c000000000000000000000000000000000000000000000000000000008200581c00000000000000000000000000000000000000000000000000000000";
  const VoteDelegationCert inner = VoteDelegationCert::fromCbor(cbor);
  CborReader               reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::VoteDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asVoteDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, VoteRegistrationDelegationMatchesNativeCbor)
{
  // Arrange
  const std::string                    cbor  = "840c8200581c000000000000000000000000000000000000000000000000000000008200581c0000000000000000000000000000000000000000000000000000000000";
  const VoteRegistrationDelegationCert inner = VoteRegistrationDelegationCert::fromCbor(cbor);
  CborReader                           reader(cbor);

  // Act
  const Certificate cert(inner);
  const Certificate decoded = Certificate::fromCbor(cbor);

  // Assert
  EXPECT_EQ(cert.type(), CertificateType::VoteRegistrationDelegation);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(cert.asVoteRegistrationDelegation(), inner);
  EXPECT_EQ(decoded, cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(Certificate, CopiesVariantAndRejectsWrongAccessors)
{
  // Arrange
  const Credential          first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential          second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  StakeRegistrationCert     inner(first);
  const Certificate         cert(inner);
  Certificate               copy(cert);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act
  inner.setCredential(second);
  StakeRegistrationCert extracted = copy.asStakeRegistration();
  extracted.setCredential(second);

  // Assert
  EXPECT_EQ(cert.asStakeRegistration().credential(), first);
  EXPECT_EQ(copy.asStakeRegistration().credential(), first);
  EXPECT_THROW(cert.asStakeDelegation(), std::logic_error);
  EXPECT_THROW(Certificate::fromCbor(trailing), std::invalid_argument);
  copy = Certificate(StakeDeregistrationCert(first));
  EXPECT_NE(copy, cert);
}
