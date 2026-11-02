/**
 * @file Ed25519SignatureTest.cpp
 *
 * @brief Unit tests for Ed25519Signature.
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

#include <Cometa/Crypto/Ed25519Signature.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Ed25519SignatureTest, CopiesBytesAndRespectsHexadecimalViewBounds)
{
  // Arrange
  std::vector<std::uint8_t> bytes(64, 1);
  const Ed25519Signature    value  = Ed25519Signature(bytes);
  const std::string         hex    = value.toHex();
  const std::string         padded = hex + "trailing";

  // Act
  bytes[0]                           = 9;
  std::vector<std::uint8_t> exported = value.toBytes();
  exported[0]                        = 8;
  const Ed25519Signature parsed      = Ed25519Signature::fromHex(std::string_view(padded.data(), hex.size()));

  // Assert
  EXPECT_EQ(value.toBytes().front(), 1);
  EXPECT_EQ(exported.front(), 8);
  EXPECT_EQ(parsed.toHex(), hex);
  EXPECT_EQ(hex.size(), 128U);
}

TEST(Ed25519SignatureTest, SupportsIndependentCopiesMovesAndSwap)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(64, 1);
  Ed25519Signature                value = Ed25519Signature(bytes);
  Ed25519Signature                copy(value);
  Ed25519Signature                assigned = Ed25519Signature(std::vector<std::uint8_t>(64, 2));
  Ed25519Signature*               alias    = &value;

  // Act
  assigned = value;
  value    = *alias;
  value    = std::move(*alias);
  Ed25519Signature moved(std::move(value));
  swap(moved, assigned);
  value = std::move(moved);
  copy  = Ed25519Signature(std::vector<std::uint8_t>(64, 2));

  // Assert
  EXPECT_EQ(value.toBytes(), bytes);
  EXPECT_EQ(assigned.toBytes(), bytes);
  EXPECT_NE(copy.toBytes(), bytes);
  EXPECT_THROW(moved.toHex(), std::logic_error);
  EXPECT_THROW(Ed25519Signature invalid(moved), std::logic_error);
}

TEST(Ed25519SignatureTest, RejectsWrongSizesAndMalformedHex)
{
  // Arrange
  const std::vector<std::uint8_t> bytes(63, 0);

  // Act & Assert
  EXPECT_THROW((void)Ed25519Signature(bytes), Error);
  EXPECT_THROW(Ed25519Signature::fromHex("00"), Error);
  EXPECT_THROW(Ed25519Signature::fromHex("0"), std::invalid_argument);
  EXPECT_THROW(Ed25519Signature::fromHex(std::string(128, 'g')), std::invalid_argument);
}

TEST(Ed25519SignatureTest, ComparesEncodedValues)
{
  // Arrange
  const Ed25519Signature first(std::vector<std::uint8_t>(64, 1));
  const Ed25519Signature same = Ed25519Signature::fromHex(first.toHex());
  const Ed25519Signature different(std::vector<std::uint8_t>(64, 2));

  // Act & Assert
  EXPECT_TRUE(first == same);
  EXPECT_FALSE(first != same);
  EXPECT_TRUE(first != different);
}
