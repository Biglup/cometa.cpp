/**
 * @file MirToStakeCredentialsCertTest.cpp
 *
 * @brief Unit tests for historical MIR credential distributions.
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

TEST(MirToStakeCredentialsCert, MatchesNativeCborFixtures)
{
  // Arrange
  const std::string         reserveCbor  = "8200a18200581c0101010101010101010101010101010101010101010101010101010100";
  const std::string         treasuryCbor = "8201a18200581c0101010101010101010101010101010101010101010101010101010100";
  const Credential          credential(CredentialType::KeyHash, Blake2bHash::fromHex("01010101010101010101010101010101010101010101010101010101"));
  MirToStakeCredentialsCert reserve(MirPotType::Reserve);
  MirToStakeCredentialsCert treasury(MirPotType::Treasury);

  // Act
  reserve.insert(credential, 0);
  treasury.insert(credential, 0);

  // Assert
  EXPECT_EQ(reserve.pot(), MirPotType::Reserve);
  EXPECT_EQ(reserve.size(), 1U);
  EXPECT_EQ(reserve.at(0).first, credential);
  EXPECT_EQ(reserve.at(0).second, 0U);
  CborReader reserveReader(reserveCbor);
  CborReader treasuryReader(treasuryCbor);
  EXPECT_EQ(reserve.toCbor(), reserveReader.readEncodedValue());
  EXPECT_EQ(treasury.toCbor(), treasuryReader.readEncodedValue());
  EXPECT_EQ(MirToStakeCredentialsCert::fromCbor(reserveCbor), reserve);
  EXPECT_EQ(MirToStakeCredentialsCert::fromCbor(treasuryCbor), treasury);
  EXPECT_FALSE(reserve.toJson().empty());
}

TEST(MirToStakeCredentialsCert, CopiesAndChangesEntries)
{
  // Arrange
  const Credential          first(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential          second(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '2')));
  MirToStakeCredentialsCert cert(MirPotType::Reserve);
  cert.insert(first, 5);
  const MirToStakeCredentialsCert original(cert);

  // Act
  cert.setPot(MirPotType::Treasury);
  cert.insert(second, 7);
  Credential returned = cert.at(0).first;
  returned.setHash(Blake2bHash::fromHex(std::string(56, '3')));

  // Assert
  EXPECT_EQ(cert.size(), 2U);
  EXPECT_EQ(original.size(), 1U);
  EXPECT_EQ(original.pot(), MirPotType::Reserve);
  EXPECT_EQ(original.at(0).first, first);
  EXPECT_EQ(cert.at(0).first, first);
  EXPECT_NE(cert, original);
  EXPECT_EQ(MirToStakeCredentialsCert::fromCbor(cert.toCbor()), cert);
}

TEST(MirToStakeCredentialsCert, RejectsInvalidPotAndIndex)
{
  // Arrange
  MirToStakeCredentialsCert cert(MirPotType::Reserve);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(MirToStakeCredentialsCert(static_cast<MirPotType>(2)), std::invalid_argument);
  EXPECT_THROW(cert.setPot(static_cast<MirPotType>(2)), std::invalid_argument);
  EXPECT_THROW(cert.at(0), std::out_of_range);
  EXPECT_THROW(MirToStakeCredentialsCert::fromCbor(trailing), std::invalid_argument);
}
