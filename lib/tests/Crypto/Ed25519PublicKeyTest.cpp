/**
 * @file Ed25519PublicKeyTest.cpp
 *
 * @brief Unit tests for Ed25519PublicKey.
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

#include <Cometa/Crypto/Ed25519PublicKey.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Ed25519PublicKeyTest, CopiesBytesAndRespectsHexadecimalViewBounds)
{
  // Arrange
  std::vector<std::uint8_t> bytes(32, 1);
  const Ed25519PublicKey    value  = Ed25519PublicKey(bytes);
  const std::string         hex    = value.toHex();
  const std::string         padded = hex + "trailing";

  // Act
  bytes[0]                           = 9;
  std::vector<std::uint8_t> exported = value.toBytes();
  exported[0]                        = 8;
  const Ed25519PublicKey parsed      = Ed25519PublicKey::fromHex(std::string_view(padded.data(), hex.size()));

  // Assert
  EXPECT_EQ(value.toBytes().front(), 1);
  EXPECT_EQ(exported.front(), 8);
  EXPECT_EQ(parsed.toHex(), hex);
  EXPECT_EQ(hex.size(), 64U);
}

TEST(Ed25519PublicKeyTest, SupportsIndependentCopiesMovesAndSwap)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(32, 1);
  Ed25519PublicKey                value = Ed25519PublicKey(bytes);
  Ed25519PublicKey                copy(value);
  Ed25519PublicKey                assigned = Ed25519PublicKey(std::vector<std::uint8_t>(32, 2));
  Ed25519PublicKey*               alias    = &value;

  // Act
  assigned = value;
  value    = *alias;
  value    = std::move(*alias);
  Ed25519PublicKey moved(std::move(value));
  swap(moved, assigned);
  value = std::move(moved);
  copy  = Ed25519PublicKey(std::vector<std::uint8_t>(32, 2));

  // Assert
  EXPECT_EQ(value.toBytes(), bytes);
  EXPECT_EQ(assigned.toBytes(), bytes);
  EXPECT_NE(copy.toBytes(), bytes);
  EXPECT_THROW(moved.toHex(), std::logic_error);
  EXPECT_THROW(Ed25519PublicKey invalid(moved), std::logic_error);
}

TEST(Ed25519PublicKeyTest, RejectsWrongSizesAndMalformedHex)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(31, 0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519PublicKey(bytes), Error);
  EXPECT_THROW(Ed25519PublicKey::fromHex("00"), Error);
  EXPECT_THROW(Ed25519PublicKey::fromHex("0"), std::invalid_argument);
  EXPECT_THROW(Ed25519PublicKey::fromHex(std::string(64, 'g')), std::invalid_argument);
}

TEST(Ed25519PublicKeyTest, ComparesEncodedValues)
{
  // Arrange
  const Ed25519PublicKey first(std::vector<std::uint8_t>(32, 1));
  const Ed25519PublicKey same = Ed25519PublicKey::fromHex(first.toHex());
  const Ed25519PublicKey different(std::vector<std::uint8_t>(32, 2));

  // Act & Assert
  EXPECT_TRUE(first == same);
  EXPECT_FALSE(first != same);
  EXPECT_TRUE(first != different);
}

TEST(Ed25519PublicKeyTest, HashesVerificationKeyBytes)
{
  // Arrange
  const Ed25519PublicKey key = Ed25519PublicKey::fromHex("d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a");

  // Act & Assert
  EXPECT_EQ(key.hash(), Blake2bHash::compute(key.toBytes(), 28));
  EXPECT_FALSE(key.verify(Ed25519Signature(std::vector<std::uint8_t>(64, 0)), {}));
}
