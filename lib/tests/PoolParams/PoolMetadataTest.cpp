/**
 * @file PoolMetadataTest.cpp
 *
 * @brief Unit tests for PoolMetadata.
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

TEST(PoolMetadata, MatchesKnownCborAndInspection)
{
  // Arrange
  const Blake2bHash hash = Blake2bHash::fromHex(std::string(64, '1'));
  const std::string cbor = "82636162635820" + std::string(64, '1');
  CborReader        encoded(cbor);

  // Act
  const PoolMetadata metadata("abc", hash);

  // Assert
  EXPECT_EQ(metadata.url(), "abc");
  EXPECT_EQ(metadata.hash(), hash);
  EXPECT_EQ(metadata.toCbor(), encoded.readEncodedValue());
  EXPECT_EQ(PoolMetadata::fromCbor(cbor), metadata);
  EXPECT_EQ(PoolMetadata::fromCbor(metadata.toCbor()), metadata);
  EXPECT_EQ(metadata.toJson(), "{\"url\":\"abc\",\"hash\":\"" + std::string(64, '1') + "\"}");
}

TEST(PoolMetadata, ReplacesShorterAndEmptyUrls)
{
  // Arrange
  PoolMetadata       metadata(std::string(128, 'x'), Blake2bHash::fromHex(std::string(64, '1')));
  const PoolMetadata original(metadata);

  // Act
  metadata.setUrl("abc");

  // Assert
  EXPECT_EQ(metadata.url(), "abc");
  EXPECT_EQ(original.url(), std::string(128, 'x'));
  EXPECT_EQ(PoolMetadata::fromCbor(metadata.toCbor()).url(), "abc");

  // Act
  metadata.setUrl({});

  // Assert
  EXPECT_TRUE(metadata.url().empty());
  EXPECT_TRUE(PoolMetadata::fromCbor(metadata.toCbor()).url().empty());
}

TEST(PoolMetadata, CopiesHashInputsAndReturnsIndependentHash)
{
  // Arrange
  Blake2bHash        first  = Blake2bHash::fromHex(std::string(64, '1'));
  const Blake2bHash  second = Blake2bHash::fromHex(std::string(64, '2'));
  PoolMetadata       metadata("abc", first);
  const PoolMetadata original(metadata);

  // Act
  first = second;
  metadata.setHash(first);
  Blake2bHash returned = metadata.hash();
  EXPECT_EQ(returned, second);
  returned = original.hash();

  // Assert
  EXPECT_EQ(metadata.hash(), second);
  EXPECT_EQ(original.hash(), returned);
  EXPECT_NE(metadata, original);
}

TEST(PoolMetadata, RejectsInvalidChangesWithoutMutation)
{
  // Arrange
  const Blake2bHash  hash      = Blake2bHash::fromHex(std::string(64, '1'));
  const Blake2bHash  shortHash = Blake2bHash::fromHex("11");
  PoolMetadata       metadata("abc", hash);
  const PoolMetadata original(metadata);
  const std::string  embedded("a\0b", 3);

  // Act & Assert
  EXPECT_THROW((void)PoolMetadata(std::string(129, 'x'), hash), std::invalid_argument);
  EXPECT_THROW((void)PoolMetadata(embedded, hash), std::invalid_argument);
  EXPECT_THROW((void)PoolMetadata("abc", shortHash), std::invalid_argument);
  EXPECT_THROW(metadata.setUrl(std::string(129, 'x')), std::invalid_argument);
  EXPECT_THROW(metadata.setUrl(embedded), std::invalid_argument);
  EXPECT_THROW(metadata.setHash(shortHash), std::invalid_argument);
  EXPECT_EQ(metadata, original);
  EXPECT_THROW(PoolMetadata::fromCbor("82636162634111"), std::invalid_argument);
  EXPECT_THROW(PoolMetadata::fromCbor("80"), Error);
  EXPECT_THROW(PoolMetadata::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(PoolMetadata::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  std::vector<std::uint8_t> trailing = metadata.toCbor();
  trailing.push_back(0);
  EXPECT_THROW(PoolMetadata::fromCbor(trailing), std::invalid_argument);
}
