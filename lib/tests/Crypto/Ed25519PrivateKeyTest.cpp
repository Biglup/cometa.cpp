/**
 * @file Ed25519PrivateKeyTest.cpp
 *
 * @brief Unit tests for Ed25519PrivateKey.
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

#include <Cometa/Crypto/Ed25519PrivateKey.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Ed25519PrivateKeyTest, CopiesBytesAndRespectsHexadecimalViewBounds)
{
  // Arrange
  std::vector<std::uint8_t> bytes(32, 1);
  const Ed25519PrivateKey   value  = Ed25519PrivateKey::fromNormalBytes(bytes);
  const std::string         hex    = value.toHex();
  const std::string         padded = hex + "trailing";

  // Act
  bytes[0]                           = 9;
  std::vector<std::uint8_t> exported = value.toBytes();
  exported[0]                        = 8;
  const Ed25519PrivateKey parsed     = Ed25519PrivateKey::fromNormalHex(std::string_view(padded.data(), hex.size()));

  // Assert
  EXPECT_EQ(value.toBytes().front(), 1);
  EXPECT_EQ(exported.front(), 8);
  EXPECT_EQ(parsed.toHex(), hex);
  EXPECT_EQ(hex.size(), 64U);
}

TEST(Ed25519PrivateKeyTest, SupportsIndependentCopiesMovesAndSwap)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(32, 1);
  Ed25519PrivateKey               value = Ed25519PrivateKey::fromNormalBytes(bytes);
  Ed25519PrivateKey               copy(value);
  Ed25519PrivateKey               assigned = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 2));
  Ed25519PrivateKey*              alias    = &value;

  // Act
  assigned = value;
  value    = *alias;
  value    = std::move(*alias);
  Ed25519PrivateKey moved(std::move(value));
  swap(moved, assigned);
  value = std::move(moved);
  copy  = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 2));

  // Assert
  EXPECT_EQ(value.toBytes(), bytes);
  EXPECT_EQ(assigned.toBytes(), bytes);
  EXPECT_NE(copy.toBytes(), bytes);
  EXPECT_THROW(moved.toHex(), std::logic_error);
  EXPECT_THROW(Ed25519PrivateKey invalid(moved), std::logic_error);
}

TEST(Ed25519PrivateKeyTest, RejectsWrongSizesAndMalformedHex)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(31, 0);

  // Act & Assert
  EXPECT_THROW(Ed25519PrivateKey::fromNormalBytes(bytes), Error);
  EXPECT_THROW(Ed25519PrivateKey::fromNormalHex("00"), Error);
  EXPECT_THROW(Ed25519PrivateKey::fromNormalHex("0"), std::invalid_argument);
  EXPECT_THROW(Ed25519PrivateKey::fromNormalHex(std::string(64, 'g')), std::invalid_argument);
}

TEST(Ed25519PrivateKeyTest, MatchesKnownNormalAndExtendedPublicKeys)
{
  // Arrange
  const Ed25519PrivateKey normal   = Ed25519PrivateKey::fromNormalHex("9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");
  const Ed25519PrivateKey extended = Ed25519PrivateKey::fromExtendedHex("a0ab55b174ba8cd95e2362d035f377b4dc779a0fae65767e3b8dd790fa748250f3ef2cc372c207d7902607ffef01872a4c785cd27e7342de7f4332f2d5fdc3a8");
  const Ed25519PrivateKey copied   = Ed25519PrivateKey::fromExtendedBytes(extended.toBytes());

  // Act & Assert
  EXPECT_FALSE(normal.isExtended());
  EXPECT_TRUE(extended.isExtended());
  EXPECT_EQ(normal.publicKey().toHex(), "d75a980182b10ab7d54bfed3c964073a0ee172f3daa62325af021a68f707511a");
  EXPECT_EQ(extended.publicKey().toHex(), "311f8914b8934efbe7cbb8cc4745853de12e8ea402df6f9f69b18d2792c6bed8");
  EXPECT_EQ(copied.toHex(), extended.toHex());
  EXPECT_TRUE(extended.publicKey().verify(extended.sign({ 1, 2, 3 }), { 1, 2, 3 }));
  EXPECT_THROW(Ed25519PrivateKey::fromExtendedBytes({}), Error);
  EXPECT_THROW(Ed25519PrivateKey::fromExtendedHex("gg"), std::invalid_argument);
}

TEST(Ed25519PrivateKeyTest, MatchesRfc8032EmptyMessageSignature)
{
  // Arrange
  const Ed25519PrivateKey key = Ed25519PrivateKey::fromNormalHex("9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60");

  // Act
  const Ed25519Signature signature = key.sign({});

  // Assert
  EXPECT_EQ(signature.toHex(), "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b");
  EXPECT_TRUE(key.publicKey().verify(signature, {}));
  EXPECT_FALSE(key.publicKey().verify(signature, { 1 }));
}
