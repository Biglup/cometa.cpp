/**
 * @file AnchorTest.cpp
 *
 * @brief Tests for Anchor.
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

#include <Cometa/Common/Anchor.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Anchor, MatchesKnownEncoding)
{
  // Arrange
  const Blake2bHash hash = Blake2bHash::fromHex(std::string(64, '0'));
  const Anchor      value("https://example.com", hash);
  const std::string cbor = "827368747470733a2f2f6578616d706c652e636f6d5820" + std::string(64, '0');

  // Act
  const Anchor decoded = Anchor::fromCbor(cbor);

  // Assert
  EXPECT_EQ(decoded, value);
  EXPECT_EQ(decoded.url(), "https://example.com");
  EXPECT_EQ(decoded.hash(), hash);
  EXPECT_EQ(decoded.toCbor(), value.toCbor());
  EXPECT_EQ(Anchor::fromCbor(value.toCbor()), value);
  EXPECT_EQ(value.toJson(), "{\"url\":\"https://example.com\",\"data_hash\":\"" + std::string(64, '0') + "\"}");
}

TEST(Anchor, MutationsPreserveIndependentCopies)
{
  // Arrange
  const Anchor original("a", Blake2bHash::fromHex(std::string(64, '0')));
  Anchor       copy = original;
  Blake2bHash  hash = original.hash();

  // Act
  copy.setUrl(std::string(128, 'b'));
  copy.setHash(Blake2bHash::fromHex(std::string(64, '1')));
  hash = copy.hash();

  // Assert
  EXPECT_EQ(original.url(), "a");
  EXPECT_EQ(original.hash(), Blake2bHash::fromHex(std::string(64, '0')));
  EXPECT_EQ(copy.url(), std::string(128, 'b'));
  EXPECT_EQ(copy.hash(), hash);
  EXPECT_EQ(Anchor::fromCbor(copy.toCbor()), copy);
}

TEST(Anchor, RejectsInvalidUrlAndHashWithoutChangingValue)
{
  // Arrange
  const Blake2bHash hash = Blake2bHash::fromHex(std::string(64, '0'));
  Anchor            value("a", hash);

  // Act & Assert
  EXPECT_THROW(Anchor("", hash), std::invalid_argument);
  EXPECT_THROW(Anchor(std::string(129, 'a'), hash), std::invalid_argument);
  EXPECT_THROW(Anchor(std::string("a\0b", 3), hash), std::invalid_argument);
  EXPECT_THROW(value.setUrl(""), std::invalid_argument);
  EXPECT_THROW(value.setHash(Blake2bHash::fromHex(std::string(56, '0'))), std::invalid_argument);
  EXPECT_EQ(value, Anchor("a", hash));
}

TEST(Anchor, CopiesBoundedStringViews)
{
  // Arrange
  const std::string text = "https://example.com/ignored";
  const Blake2bHash hash = Blake2bHash::fromHex(std::string(64, '0'));

  // Act
  const Anchor value(std::string_view(text.data(), 19), hash);

  // Assert
  EXPECT_EQ(value.url(), "https://example.com");
}

TEST(Anchor, RejectsMalformedCbor)
{
  // Act & Assert
  EXPECT_THROW(Anchor::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Anchor::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(Anchor::fromCbor("8100"), Error);
  EXPECT_THROW(Anchor::fromCbor("8261615820" + std::string(64, '0') + "00"), std::invalid_argument);
}

TEST(Anchor, MovedFromValuesRejectAccessAndMutation)
{
  // Arrange
  Anchor       original("a", Blake2bHash::fromHex(std::string(64, '0')));
  const Anchor moved = std::move(original);

  // Act & Assert
  EXPECT_THROW(original.url(), std::logic_error);
  EXPECT_THROW(original.hash(), std::logic_error);
  EXPECT_THROW(original.setUrl("b"), std::logic_error);
  EXPECT_THROW(original.setHash(moved.hash()), std::logic_error);
  EXPECT_THROW(original.toJson(), std::logic_error);
}
