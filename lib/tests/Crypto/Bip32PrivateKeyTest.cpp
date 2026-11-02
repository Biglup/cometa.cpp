/**
 * @file Bip32PrivateKeyTest.cpp
 *
 * @brief Unit tests for Bip32PrivateKey.
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

#include <Cometa/Crypto/Bip32PrivateKey.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Bip32PrivateKeyTest, CopiesBytesAndRespectsHexadecimalViewBounds)
{
  // Arrange
  std::vector<std::uint8_t> bytes(96, 1);
  const Bip32PrivateKey     value  = Bip32PrivateKey(bytes);
  const std::string         hex    = value.toHex();
  const std::string         padded = hex + "trailing";

  // Act
  bytes[0]                           = 9;
  std::vector<std::uint8_t> exported = value.toBytes();
  exported[0]                        = 8;
  const Bip32PrivateKey parsed       = Bip32PrivateKey::fromHex(std::string_view(padded.data(), hex.size()));

  // Assert
  EXPECT_EQ(value.toBytes().front(), 1);
  EXPECT_EQ(exported.front(), 8);
  EXPECT_EQ(parsed.toHex(), hex);
  EXPECT_EQ(hex.size(), 192U);
}

TEST(Bip32PrivateKeyTest, SupportsIndependentCopiesMovesAndSwap)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(96, 1);
  Bip32PrivateKey                 value = Bip32PrivateKey(bytes);
  Bip32PrivateKey                 copy(value);
  Bip32PrivateKey                 assigned = Bip32PrivateKey(std::vector<std::uint8_t>(96, 2));
  Bip32PrivateKey*                alias    = &value;

  // Act
  assigned = value;
  value    = *alias;
  value    = std::move(*alias);
  Bip32PrivateKey moved(std::move(value));
  swap(moved, assigned);
  value = std::move(moved);
  copy  = Bip32PrivateKey(std::vector<std::uint8_t>(96, 2));

  // Assert
  EXPECT_EQ(value.toBytes(), bytes);
  EXPECT_EQ(assigned.toBytes(), bytes);
  EXPECT_NE(copy.toBytes(), bytes);
  EXPECT_THROW(moved.toHex(), std::logic_error);
  EXPECT_THROW(Bip32PrivateKey invalid(moved), std::logic_error);
}

TEST(Bip32PrivateKeyTest, RejectsWrongSizesAndMalformedHex)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(95, 0);

  // Act & Assert
  EXPECT_THROW((void)Bip32PrivateKey(bytes), Error);
  EXPECT_THROW(Bip32PrivateKey::fromHex("00"), Error);
  EXPECT_THROW(Bip32PrivateKey::fromHex("0"), std::invalid_argument);
  EXPECT_THROW(Bip32PrivateKey::fromHex(std::string(192, 'g')), std::invalid_argument);
}

TEST(Bip32PrivateKeyTest, EmptyPathReturnsIndependentCopy)
{
  // Arrange
  Bip32PrivateKey   key      = Bip32PrivateKey::fromHex("60ce7dbec3616e9fc17e0c32578b3f380337b1b61a1f3cb9651aee30670e6f53970419a23a2e4e4082d12bf78faa8645dfc882cee2ae7179e2b07fe88098abb2072310084784c7308182dbbdb1449b2706586f1ff5cbf13d15e9b6e78c15f067");
  const std::string original = key.toHex();

  // Act
  const Bip32PrivateKey same = key.derive({});
  key                        = key.derive({ 0, 1 });

  // Assert
  EXPECT_EQ(same.toHex(), original);
  EXPECT_NE(key.toHex(), original);
}

TEST(Bip32PrivateKeyTest, PublicAndPrivateNonHardenedDerivationAgree)
{
  // Arrange
  const Bip32PrivateKey            root = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0));
  const std::vector<std::uint32_t> path { 0, 1, 42 };

  // Act
  const Bip32PrivateKey child        = root.derive(path);
  const Bip32PublicKey  public_child = root.publicKey().derive(path);

  // Assert
  EXPECT_EQ(child.publicKey(), public_child);
  EXPECT_EQ(child.toEd25519().publicKey(), public_child.toEd25519());
  EXPECT_TRUE(child.toEd25519().isExtended());
  EXPECT_THROW(Bip32PrivateKey::fromBip39Entropy({}), Error);
}

TEST(Bip32PrivateKeyTest, PasswordStringViewsPreserveLengthAndEmbeddedNulls)
{
  // Arrange
  const std::vector<std::uint8_t> entropy(16, 0);
  const std::string               password("a\0b", 3);

  // Act
  const Bip32PrivateKey full   = Bip32PrivateKey::fromBip39Entropy(entropy, password);
  const Bip32PrivateKey prefix = Bip32PrivateKey::fromBip39Entropy(entropy, std::string_view(password.data(), 1));

  // Assert
  EXPECT_NE(full.toHex(), prefix.toHex());
}
