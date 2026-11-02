/**
 * @file GenesisKeyDelegationCertTest.cpp
 *
 * @brief Unit tests for historical genesis key delegation certificates.
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

TEST(GenesisKeyDelegationCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string cbor =
    "8405581c00010001000100010001000100010001000100010001000100010001581c00020002000200020002000200020002"
    "00020002000200020002000258200003000300030003000300030003000300030003000300030003000300030003";
  const Blake2bHash genesis  = Blake2bHash::fromHex("00010001000100010001000100010001000100010001000100010001");
  const Blake2bHash delegate = Blake2bHash::fromHex("00020002000200020002000200020002000200020002000200020002");
  const Blake2bHash vrf      = Blake2bHash::fromHex("0003000300030003000300030003000300030003000300030003000300030003");
  CborReader        encoded(cbor);

  // Act
  const GenesisKeyDelegationCert cert(genesis, delegate, vrf);

  // Assert
  EXPECT_EQ(cert.genesisHash(), genesis);
  EXPECT_EQ(cert.delegateHash(), delegate);
  EXPECT_EQ(cert.vrfKeyHash(), vrf);
  EXPECT_EQ(cert.toCbor(), encoded.readEncodedValue());
  EXPECT_EQ(GenesisKeyDelegationCert::fromCbor(cbor), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(GenesisKeyDelegationCert, CopiesAndReplacesHashes)
{
  // Arrange
  const Blake2bHash              first  = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash              second = Blake2bHash::fromHex(std::string(56, '2'));
  const Blake2bHash              vrf    = Blake2bHash::fromHex(std::string(64, '3'));
  GenesisKeyDelegationCert       cert(first, first, vrf);
  const GenesisKeyDelegationCert original(cert);
  Blake2bHash                    returned = cert.genesisHash();

  // Act
  returned = second;
  cert.setGenesisHash(second);
  cert.setDelegateHash(second);
  cert.setVrfKeyHash(Blake2bHash::fromHex(std::string(64, '4')));

  // Assert
  EXPECT_EQ(cert.genesisHash(), second);
  EXPECT_EQ(cert.delegateHash(), second);
  EXPECT_EQ(cert.vrfKeyHash().toHex(), std::string(64, '4'));
  EXPECT_EQ(original.genesisHash(), first);
  EXPECT_EQ(original.vrfKeyHash(), vrf);
  EXPECT_NE(cert, original);
  EXPECT_EQ(GenesisKeyDelegationCert::fromCbor(cert.toCbor()), cert);
}

TEST(GenesisKeyDelegationCert, RejectsInvalidHashLengthsAndTrailingCbor)
{
  // Arrange
  const Blake2bHash              shortHash = Blake2bHash::fromHex("11");
  const Blake2bHash              hash28    = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash              hash32    = Blake2bHash::fromHex(std::string(64, '2'));
  GenesisKeyDelegationCert       cert(hash28, hash28, hash32);
  const GenesisKeyDelegationCert original(cert);
  std::vector<std::uint8_t>      trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(GenesisKeyDelegationCert(shortHash, hash28, hash32), std::invalid_argument);
  EXPECT_THROW(GenesisKeyDelegationCert(hash28, shortHash, hash32), std::invalid_argument);
  EXPECT_THROW(GenesisKeyDelegationCert(hash28, hash28, shortHash), std::invalid_argument);
  EXPECT_THROW(cert.setGenesisHash(shortHash), std::invalid_argument);
  EXPECT_THROW(cert.setDelegateHash(shortHash), std::invalid_argument);
  EXPECT_THROW(cert.setVrfKeyHash(shortHash), std::invalid_argument);
  EXPECT_THROW(GenesisKeyDelegationCert::fromCbor(trailing), std::invalid_argument);
  EXPECT_EQ(cert, original);
}
