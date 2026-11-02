/**
 * @file UnregisterDRepCertTest.cpp
 *
 * @brief Unit tests for DRep unregistration certificates.
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

TEST(UnregisterDRepCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string cbor = "83118200581c0000000000000000000000000000000000000000000000000000000000";
  const Credential  credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  CborReader        reader(cbor);

  // Act
  const UnregisterDRepCert cert(credential, 0);

  // Assert
  EXPECT_EQ(cert.credential(), credential);
  EXPECT_EQ(cert.deposit(), 0U);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(UnregisterDRepCert::fromCbor(cbor), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(UnregisterDRepCert, CopiesAndReplacesFields)
{
  // Arrange
  const Credential         first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential         second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  UnregisterDRepCert       cert(first, 1);
  const UnregisterDRepCert original(cert);
  Credential               returned = cert.credential();

  // Act
  returned.setHash(Blake2bHash::fromHex(std::string(56, '3')));
  cert.setCredential(second);
  cert.setDeposit(2);

  // Assert
  EXPECT_EQ(original.credential(), first);
  EXPECT_EQ(original.deposit(), 1U);
  EXPECT_EQ(cert.credential(), second);
  EXPECT_EQ(cert.deposit(), 2U);
  EXPECT_NE(cert, original);
  EXPECT_EQ(UnregisterDRepCert::fromCbor(cert.toCbor()), cert);
}

TEST(UnregisterDRepCert, RejectsTrailingCbor)
{
  // Arrange
  const Credential          credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const UnregisterDRepCert  cert(credential, 0);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(UnregisterDRepCert::fromCbor(trailing), std::invalid_argument);
}
