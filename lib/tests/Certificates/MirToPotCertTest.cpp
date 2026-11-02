/**
 * @file MirToPotCertTest.cpp
 *
 * @brief Unit tests for historical MIR pot transfers certificates.
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

TEST(MirToPotCert, MatchesNativeCborFixtures)
{
  // Arrange
  CborReader reserveCbor("82001a000f4240");
  CborReader treasuryCbor("82011a000f4240");

  // Act
  const MirToPotCert reserve(MirPotType::Reserve, 1'000'000);
  const MirToPotCert treasury(MirPotType::Treasury, 1'000'000);

  // Assert
  EXPECT_EQ(reserve.pot(), MirPotType::Reserve);
  EXPECT_EQ(reserve.amount(), 1'000'000U);
  EXPECT_EQ(reserve.toCbor(), reserveCbor.readEncodedValue());
  EXPECT_EQ(treasury.toCbor(), treasuryCbor.readEncodedValue());
  EXPECT_EQ(MirToPotCert::fromCbor(reserve.toCbor()), reserve);
  EXPECT_EQ(MirToPotCert::fromCbor(treasury.toCbor()), treasury);
  EXPECT_FALSE(reserve.toJson().empty());
}

TEST(MirToPotCert, CopiesAndReplacesFields)
{
  // Arrange
  MirToPotCert       cert(MirPotType::Reserve, 1);
  const MirToPotCert original(cert);

  // Act
  cert.setPot(MirPotType::Treasury);
  cert.setAmount(2);

  // Assert
  EXPECT_EQ(cert.pot(), MirPotType::Treasury);
  EXPECT_EQ(cert.amount(), 2U);
  EXPECT_EQ(original.pot(), MirPotType::Reserve);
  EXPECT_EQ(original.amount(), 1U);
  EXPECT_NE(cert, original);
  EXPECT_EQ(MirToPotCert::fromCbor(cert.toCbor()), cert);
}

TEST(MirToPotCert, RejectsUnknownPotAndTrailingCbor)
{
  // Arrange
  MirToPotCert              cert(MirPotType::Reserve, 1);
  const MirToPotCert        original(cert);
  std::vector<std::uint8_t> trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(MirToPotCert(static_cast<MirPotType>(2), 1), std::invalid_argument);
  EXPECT_THROW(cert.setPot(static_cast<MirPotType>(2)), std::invalid_argument);
  EXPECT_THROW(MirToPotCert::fromCbor(trailing), std::invalid_argument);
  EXPECT_EQ(cert, original);
}
