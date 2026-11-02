/**
 * @file AuthCommitteeHotCertTest.cpp
 *
 * @brief Unit tests for committee hot-key authorization certificates.
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

TEST(AuthCommitteeHotCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string cbor =
    "830e8200581c000000000000000000000000000000000000000000000000000000008200581c00000000000000000000000000000000000000000000000000000000";
  const Credential credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  CborReader       reader(cbor);

  // Act
  const AuthCommitteeHotCert cert(credential, credential);

  // Assert
  EXPECT_EQ(cert.coldCredential(), credential);
  EXPECT_EQ(cert.hotCredential(), credential);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(AuthCommitteeHotCert::fromCbor(cbor), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(AuthCommitteeHotCert, CopiesAndReplacesCredentials)
{
  // Arrange
  const Credential           first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential           second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  AuthCommitteeHotCert       cert(first, first);
  const AuthCommitteeHotCert original(cert);
  Credential                 returned = cert.hotCredential();

  // Act
  returned.setHash(Blake2bHash::fromHex(std::string(56, '3')));
  cert.setColdCredential(second);
  cert.setHotCredential(second);

  // Assert
  EXPECT_EQ(original.coldCredential(), first);
  EXPECT_EQ(original.hotCredential(), first);
  EXPECT_EQ(cert.coldCredential(), second);
  EXPECT_EQ(cert.hotCredential(), second);
  EXPECT_NE(cert, original);
  EXPECT_EQ(AuthCommitteeHotCert::fromCbor(cert.toCbor()), cert);
}

TEST(AuthCommitteeHotCert, RejectsTrailingCbor)
{
  // Arrange
  const Credential           credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const AuthCommitteeHotCert cert(credential, credential);
  std::vector<std::uint8_t>  trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(AuthCommitteeHotCert::fromCbor(trailing), std::invalid_argument);
}
