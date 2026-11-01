/**
 * @file Blake2bHashTest.cpp
 *
 * @brief Unit tests for Blake2bHash.
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

#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
const std::string HASH_HEX(56, '0');

Blake2bHash
keyHash()
{
  return Blake2bHash::fromHex(HASH_HEX);
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(Blake2bHash, MatchesKnownDigest)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 'a', 'b', 'c' };

  // Act
  const Blake2bHash hash = Blake2bHash::compute(bytes);

  // Assert
  EXPECT_EQ(hash.toHex(), "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319");
}

TEST(Blake2bHash, SerializesToCbor)
{
  // Arrange
  const Blake2bHash hash = keyHash();

  // Act
  const std::vector<std::uint8_t> cbor    = hash.toCbor();
  const Blake2bHash               decoded = Blake2bHash::fromCbor(cbor);

  // Assert
  EXPECT_EQ(decoded, hash);
  ASSERT_EQ(cbor.size(), 30);
  EXPECT_EQ(cbor[0], 0x58);
  EXPECT_EQ(cbor[1], 28);
}

TEST(Blake2bHash, RejectsInvalidHexAndDigestSizes)
{
  // Act & Assert
  EXPECT_THROW(Blake2bHash::fromHex("0x"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromHex("abc"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::compute({}), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::compute({ 1 }, 0), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::compute({ 1 }, 65), std::invalid_argument);
}

TEST(Blake2bHash, RejectsMalformedCbor)
{
  // Arrange
  const std::vector<std::uint8_t> cbor { 0x01 };

  // Act & Assert
  EXPECT_THROW(Blake2bHash::fromCbor(cbor), Error);
}

TEST(Blake2bHash, SerializesToJson)
{
  // Arrange
  const Blake2bHash value = keyHash();

  // Act
  const std::string json = value.toJson();

  // Assert
  EXPECT_EQ(json, "\"" + HASH_HEX + "\"");
}

TEST(Blake2bHash, ParsesHexCbor)
{
  // Arrange
  const std::string hex      = "581c00000000000000000000000000000000000000000000000000000000";
  const Blake2bHash expected = keyHash();

  // Act
  const Blake2bHash decoded = Blake2bHash::fromCbor(hex);

  // Assert
  EXPECT_EQ(decoded, expected);
}

TEST(Blake2bHash, RejectsInvalidHexCbor)
{
  // Arrange
  const std::string trailing = "581c0000000000000000000000000000000000000000000000000000000000";

  // Act & Assert
  EXPECT_THROW(Blake2bHash::fromCbor("0"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromCbor("gg"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromCbor("0x40"), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromCbor(trailing), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromCbor("01"), Error);
  EXPECT_THROW(Blake2bHash::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Blake2bHash::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(Blake2bHash, ByteConstructorCopiesInput)
{
  // Arrange
  std::vector<std::uint8_t> bytes(28, 0);

  // Act
  const Blake2bHash value(bytes);
  bytes[0]                         = 1;
  std::vector<std::uint8_t> result = value.toBytes();
  result[0]                        = 2;

  // Assert
  EXPECT_EQ(value.toHex(), HASH_HEX);
  EXPECT_EQ(result[0], 2);
}

TEST(Blake2bHash, ComputesRequestedDigestSize)
{
  // Arrange
  const std::vector<std::uint8_t> input { 'a', 'b', 'c' };

  // Act
  const Blake2bHash digest = Blake2bHash::compute(input, 64);

  // Assert
  EXPECT_EQ(digest.toHex(), "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923");
}

TEST(Blake2bHash, ComparesValues)
{
  // Arrange
  const Blake2bHash first = keyHash();
  const Blake2bHash same(std::vector<std::uint8_t>(28, 0));
  const Blake2bHash second = Blake2bHash::fromHex(std::string(56, '1'));

  // Act & Assert
  EXPECT_EQ(first, same);
  EXPECT_NE(first, second);
  EXPECT_LT(first, second);
  EXPECT_FALSE(second < first);
}
