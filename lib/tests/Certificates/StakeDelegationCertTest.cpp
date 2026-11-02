/**
 * @file StakeDelegationCertTest.cpp
 *
 * @brief Unit tests for stake delegation certificates.
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

TEST(StakeDelegationCert, MatchesNativeCborAndInspection)
{
  // Arrange
  const std::string cbor = "83028200581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef92";
  const Credential  credential(CredentialType::KeyHash, Blake2bHash::fromHex("cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f"));
  const Blake2bHash poolHash = Blake2bHash::fromHex("d85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef92");
  CborReader        encoded(cbor);

  // Act
  const StakeDelegationCert cert(credential, poolHash);

  // Assert
  EXPECT_EQ(cert.credential(), credential);
  EXPECT_EQ(cert.poolKeyHash(), poolHash);
  EXPECT_EQ(cert.toCbor(), encoded.readEncodedValue());
  EXPECT_EQ(StakeDelegationCert::fromCbor(cbor), cert);
  EXPECT_EQ(StakeDelegationCert::fromCbor(cert.toCbor()), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(StakeDelegationCert, CopiesAndReplacesCredential)
{
  // Arrange
  const Credential          first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential          second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  StakeDelegationCert       cert(first, Blake2bHash::fromHex(std::string(56, '3')));
  const StakeDelegationCert original(cert);
  Credential                returned = cert.credential();

  // Act
  returned = second;
  cert.setCredential(second);
  cert.setPoolKeyHash(Blake2bHash::fromHex(std::string(56, '4')));

  // Assert
  EXPECT_EQ(cert.credential(), second);
  EXPECT_EQ(cert.poolKeyHash().toHex(), std::string(56, '4'));
  EXPECT_EQ(original.credential(), first);
  EXPECT_EQ(returned, second);
  EXPECT_NE(cert, original);
  EXPECT_EQ(StakeDelegationCert::fromCbor(cert.toCbor()), cert);
}

TEST(StakeDelegationCert, RejectsMalformedCbor)
{
  // Arrange
  StakeDelegationCert       cert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))), Blake2bHash::fromHex(std::string(56, '3')));
  const StakeDelegationCert original(cert);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(StakeDelegationCert::fromCbor(trailing), std::invalid_argument);
  EXPECT_THROW(StakeDelegationCert::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(cert.setPoolKeyHash(Blake2bHash::fromHex("11")), std::invalid_argument);
  EXPECT_EQ(cert, original);
}
