/**
 * @file PoolRetirementCertTest.cpp
 *
 * @brief Unit tests for pool retirement certificates.
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

TEST(PoolRetirementCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string cbor     = "8304581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef921903e8";
  const Blake2bHash poolHash = Blake2bHash::fromHex("d85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef92");
  CborReader        encoded(cbor);

  // Act
  const PoolRetirementCert cert(poolHash, 1000);

  // Assert
  EXPECT_EQ(cert.poolKeyHash(), poolHash);
  EXPECT_EQ(cert.epoch(), 1000U);
  EXPECT_EQ(cert.toCbor(), encoded.readEncodedValue());
  EXPECT_EQ(PoolRetirementCert::fromCbor(cbor), cert);
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(PoolRetirementCert, CopiesAndReplacesFields)
{
  // Arrange
  const Blake2bHash        first  = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash        second = Blake2bHash::fromHex(std::string(56, '2'));
  PoolRetirementCert       cert(first, 10);
  const PoolRetirementCert original(cert);
  Blake2bHash              returned = cert.poolKeyHash();

  // Act
  returned = second;
  cert.setPoolKeyHash(second);
  cert.setEpoch(20);

  // Assert
  EXPECT_EQ(cert.poolKeyHash(), second);
  EXPECT_EQ(cert.epoch(), 20U);
  EXPECT_EQ(original.poolKeyHash(), first);
  EXPECT_EQ(original.epoch(), 10U);
  EXPECT_NE(cert, original);
  EXPECT_EQ(PoolRetirementCert::fromCbor(cert.toCbor()), cert);
}

TEST(PoolRetirementCert, RejectsInvalidHashAndTrailingCbor)
{
  // Arrange
  const Blake2bHash         shortHash = Blake2bHash::fromHex("11");
  PoolRetirementCert        cert(Blake2bHash::fromHex(std::string(56, '1')), 10);
  const PoolRetirementCert  original(cert);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(PoolRetirementCert(shortHash, 10), std::invalid_argument);
  EXPECT_THROW(cert.setPoolKeyHash(shortHash), std::invalid_argument);
  EXPECT_THROW(PoolRetirementCert::fromCbor(trailing), std::invalid_argument);
  EXPECT_EQ(cert, original);
}
