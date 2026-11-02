/**
 * @file MirCertTest.cpp
 *
 * @brief Unit tests for historical MIR certificate variants.
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

TEST(MirCert, MatchesNativePotFixtureAndCopiesInput)
{
  // Arrange
  const std::string cbor = "820682001a000f4240";
  MirToPotCert      transfer(MirPotType::Reserve, 1'000'000);
  CborReader        reader(cbor);

  // Act
  const MirCert cert(transfer);
  transfer.setAmount(2);
  MirToPotCert returned = cert.asToPot();
  returned.setAmount(3);

  // Assert
  EXPECT_EQ(cert.type(), MirCertType::ToPot);
  EXPECT_EQ(cert.asToPot().amount(), 1'000'000U);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(MirCert::fromCbor(cbor), cert);
  EXPECT_THROW(cert.asToStakeCredentials(), std::logic_error);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(MirCert, MatchesNativeCredentialFixtureAndCopiesInput)
{
  // Arrange
  const std::string         cbor = "82068200a18200581c0101010101010101010101010101010101010101010101010101010100";
  const Credential          credential(CredentialType::KeyHash, Blake2bHash::fromHex("01010101010101010101010101010101010101010101010101010101"));
  MirToStakeCredentialsCert distribution(MirPotType::Reserve);
  distribution.insert(credential, 0);
  CborReader reader(cbor);

  // Act
  const MirCert cert(distribution);
  distribution.insert(credential, 1);
  MirToStakeCredentialsCert returned = cert.asToStakeCredentials();
  returned.setPot(MirPotType::Treasury);

  // Assert
  EXPECT_EQ(cert.type(), MirCertType::ToStakeCredentials);
  EXPECT_EQ(cert.asToStakeCredentials().pot(), MirPotType::Reserve);
  EXPECT_EQ(cert.asToStakeCredentials().size(), 1U);
  EXPECT_EQ(cert.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(MirCert::fromCbor(cbor), cert);
  EXPECT_THROW(cert.asToPot(), std::logic_error);
}

TEST(MirCert, CopiesAndRejectsTrailingCbor)
{
  // Arrange
  const MirCert             original(MirToPotCert(MirPotType::Reserve, 1));
  MirCert                   copy(original);
  std::vector<std::uint8_t> trailing = original.toCbor();
  trailing.push_back(0);

  // Act
  copy = MirCert(MirToPotCert(MirPotType::Treasury, 2));

  // Assert
  EXPECT_NE(copy, original);
  EXPECT_EQ(original.asToPot().amount(), 1U);
  EXPECT_EQ(MirCert::fromCbor(original.toCbor()), original);
  EXPECT_THROW(MirCert::fromCbor(trailing), std::invalid_argument);
}
