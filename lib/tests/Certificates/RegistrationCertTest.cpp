/**
 * @file RegistrationCertTest.cpp
 *
 * @brief Unit tests for registration certificates.
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

TEST(RegistrationCert, MatchesNativeCborAndInspection)
{
  // Arrange
  const std::string cbor = "83078200581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f1a001e8480";
  const Credential  credential(CredentialType::KeyHash, Blake2bHash::fromHex("cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f"));
  CborReader        encoded(cbor);

  // Act
  const RegistrationCert cert(credential, 2'000'000);

  // Assert
  EXPECT_EQ(cert.credential(), credential);
  EXPECT_EQ(cert.deposit(), 2'000'000U);
  EXPECT_EQ(cert.toCbor(), encoded.readEncodedValue());
  EXPECT_EQ(RegistrationCert::fromCbor(cbor), cert);
  EXPECT_EQ(RegistrationCert::fromCbor(cert.toCbor()), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(RegistrationCert, CopiesAndReplacesCredential)
{
  // Arrange
  const Credential       first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential       second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  RegistrationCert       cert(first, 2'000'000);
  const RegistrationCert original(cert);
  Credential             returned = cert.credential();

  // Act
  returned = second;
  cert.setCredential(second);
  cert.setDeposit(3'000'000);

  // Assert
  EXPECT_EQ(cert.credential(), second);
  EXPECT_EQ(cert.deposit(), 3'000'000U);
  EXPECT_EQ(original.credential(), first);
  EXPECT_EQ(original.deposit(), 2'000'000U);
  EXPECT_EQ(returned, second);
  EXPECT_NE(cert, original);
  EXPECT_EQ(RegistrationCert::fromCbor(cert.toCbor()), cert);
}

TEST(RegistrationCert, RejectsMalformedCbor)
{
  // Arrange
  RegistrationCert          cert(Credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1'))), 2'000'000);
  const RegistrationCert    original(cert);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(RegistrationCert::fromCbor(trailing), std::invalid_argument);
  EXPECT_THROW(RegistrationCert::fromCbor("z"), std::invalid_argument);
  EXPECT_EQ(cert, original);
}
