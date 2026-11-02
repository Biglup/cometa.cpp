/**
 * @file ResignCommitteeColdCertTest.cpp
 *
 * @brief Unit tests for committee cold-key resignation certificates.
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

TEST(ResignCommitteeColdCert, MatchesNativeCborFixtures)
{
  // Arrange
  const std::string withoutAnchor = "830f8200581c00000000000000000000000000000000000000000000000000000000f6";
  const std::string withAnchor =
    "830f8200581c00000000000000000000000000000000000000000000000000000000827668747470733a2f2f7777772e736f6d6575726c2e696f58200000000000000000000000000000000000000000000000000000000000000000";
  const Credential credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const Anchor     anchor("https://www.someurl.io", Blake2bHash::fromHex(std::string(64, '0')));
  CborReader       withoutReader(withoutAnchor);
  CborReader       withReader(withAnchor);

  // Act
  const ResignCommitteeColdCert plain(credential);
  const ResignCommitteeColdCert anchored(credential, anchor);

  // Assert
  EXPECT_EQ(plain.credential(), credential);
  EXPECT_FALSE(plain.anchor().has_value());
  EXPECT_EQ(plain.toCbor(), withoutReader.readEncodedValue());
  EXPECT_EQ(anchored.toCbor(), withReader.readEncodedValue());
  EXPECT_EQ(anchored.anchor().value(), anchor);
  EXPECT_EQ(ResignCommitteeColdCert::fromCbor(withAnchor), anchored);
  EXPECT_FALSE(plain.toJson().empty());
}

TEST(ResignCommitteeColdCert, CopiesAndReplacesFields)
{
  // Arrange
  const Credential              first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential              second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  const Anchor                  anchor("https://example.com", Blake2bHash::fromHex(std::string(64, '3')));
  ResignCommitteeColdCert       cert(first);
  const ResignCommitteeColdCert original(cert);

  // Act
  cert.setCredential(second);
  cert.setAnchor(anchor);
  Anchor returned = cert.anchor().value();
  returned.setUrl("https://changed.example");

  // Assert
  EXPECT_EQ(original.credential(), first);
  EXPECT_FALSE(original.anchor().has_value());
  EXPECT_EQ(cert.credential(), second);
  EXPECT_EQ(cert.anchor().value(), anchor);
  EXPECT_NE(cert, original);
  EXPECT_EQ(ResignCommitteeColdCert::fromCbor(cert.toCbor()), cert);
  cert.setAnchor(std::nullopt);
  EXPECT_FALSE(cert.anchor().has_value());
}

TEST(ResignCommitteeColdCert, RejectsTrailingCbor)
{
  // Arrange
  const Credential              credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const ResignCommitteeColdCert cert(credential);
  std::vector<std::uint8_t>     trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(ResignCommitteeColdCert::fromCbor(trailing), std::invalid_argument);
}
