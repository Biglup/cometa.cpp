/**
 * @file Bip32PublicKeyTest.cpp
 *
 * @brief Unit tests for Bip32PublicKey.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/Crypto/Bip32PublicKey.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Bip32PublicKeyTest, CopiesBytesAndRespectsHexadecimalViewBounds)
{
  // Arrange
  std::vector<std::uint8_t> bytes(64, 1);
  const Bip32PublicKey      value  = Bip32PublicKey(bytes);
  const std::string         hex    = value.toHex();
  const std::string         padded = hex + "trailing";

  // Act
  bytes[0]                           = 9;
  std::vector<std::uint8_t> exported = value.toBytes();
  exported[0]                        = 8;
  const Bip32PublicKey parsed        = Bip32PublicKey::fromHex(std::string_view(padded.data(), hex.size()));

  // Assert
  EXPECT_EQ(value.toBytes().front(), 1);
  EXPECT_EQ(exported.front(), 8);
  EXPECT_EQ(parsed.toHex(), hex);
  EXPECT_EQ(hex.size(), 128U);
}

TEST(Bip32PublicKeyTest, SupportsIndependentCopiesMovesAndSwap)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(64, 1);
  Bip32PublicKey                  value = Bip32PublicKey(bytes);
  Bip32PublicKey                  copy(value);
  Bip32PublicKey                  assigned = Bip32PublicKey(std::vector<std::uint8_t>(64, 2));
  Bip32PublicKey*                 alias    = &value;

  // Act
  assigned = value;
  value    = *alias;
  value    = std::move(*alias);
  Bip32PublicKey moved(std::move(value));
  swap(moved, assigned);
  value = std::move(moved);
  copy  = Bip32PublicKey(std::vector<std::uint8_t>(64, 2));

  // Assert
  EXPECT_EQ(value.toBytes(), bytes);
  EXPECT_EQ(assigned.toBytes(), bytes);
  EXPECT_NE(copy.toBytes(), bytes);
  EXPECT_THROW(moved.toHex(), std::logic_error);
  EXPECT_THROW(Bip32PublicKey invalid(moved), std::logic_error);
}

TEST(Bip32PublicKeyTest, RejectsWrongSizesAndMalformedHex)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(63, 0);

  // Act & Assert
  EXPECT_THROW((void)Bip32PublicKey(bytes), Error);
  EXPECT_THROW(Bip32PublicKey::fromHex("00"), Error);
  EXPECT_THROW(Bip32PublicKey::fromHex("0"), std::invalid_argument);
  EXPECT_THROW(Bip32PublicKey::fromHex(std::string(128, 'g')), std::invalid_argument);
}

TEST(Bip32PublicKeyTest, ComparesEncodedValues)
{
  // Arrange
  const Bip32PublicKey first(std::vector<std::uint8_t>(64, 1));
  const Bip32PublicKey same = Bip32PublicKey::fromHex(first.toHex());
  const Bip32PublicKey different(std::vector<std::uint8_t>(64, 2));

  // Act & Assert
  EXPECT_TRUE(first == same);
  EXPECT_FALSE(first != same);
  EXPECT_TRUE(first != different);
}

TEST(Bip32PublicKeyTest, EmptyPathReturnsIndependentCopy)
{
  // Arrange
  Bip32PublicKey    key      = Bip32PublicKey::fromHex("37fdfdbe9ac856469f8d83c66c57880246cd8bf7f852bf5b94336fe535c0efc8072310084784c7308182dbbdb1449b2706586f1ff5cbf13d15e9b6e78c15f067");
  const std::string original = key.toHex();

  // Act
  const Bip32PublicKey same = key.derive({});
  key                       = key.derive({ 0, 1 });

  // Assert
  EXPECT_EQ(same.toHex(), original);
  EXPECT_NE(key.toHex(), original);
}

TEST(Bip32PublicKeyTest, RejectsHardenedDerivationAndInvalidCurvePoints)
{
  // Arrange
  const Bip32PublicKey key = Bip32PublicKey::fromHex("37fdfdbe9ac856469f8d83c66c57880246cd8bf7f852bf5b94336fe535c0efc8072310084784c7308182dbbdb1449b2706586f1ff5cbf13d15e9b6e78c15f067");
  const Bip32PublicKey invalid(std::vector<std::uint8_t>(64, 2));

  // Act & Assert
  EXPECT_THROW(key.derive({ 0x80000000U }), Error);
  EXPECT_THROW(invalid.derive({ 0 }), Error);
}

TEST(Bip32PublicKeyTest, HashesExtendedBytesAndExtractsVerificationKey)
{
  // Arrange
  const Bip32PublicKey key = Bip32PublicKey::fromHex("37fdfdbe9ac856469f8d83c66c57880246cd8bf7f852bf5b94336fe535c0efc8072310084784c7308182dbbdb1449b2706586f1ff5cbf13d15e9b6e78c15f067");

  // Act & Assert
  EXPECT_EQ(key.hash(), Blake2bHash::compute(key.toBytes(), 28));
  EXPECT_EQ(key.toEd25519().toHex(), key.toHex().substr(0, 64));
  EXPECT_NE(key.hash(), key.toEd25519().hash());
}
