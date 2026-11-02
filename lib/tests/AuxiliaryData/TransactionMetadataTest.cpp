/**
 * @file TransactionMetadataTest.cpp
 *
 * @brief Unit tests for TransactionMetadata.
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

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(TransactionMetadata, ReplacesLabelsAndSortsEntries)
{
  // Arrange
  TransactionMetadata metadata;
  metadata.set(2, Metadatum("two"));
  metadata.set(1, Metadatum("old"));
  CborReader expected("a201636f6e65026374776f");

  // Act
  metadata.set(1, Metadatum("one"));

  // Assert
  EXPECT_EQ(metadata.size(), 2U);
  EXPECT_FALSE(metadata.empty());
  EXPECT_EQ(metadata.keys(), (MetadatumLabelList { 1, 2 }));
  EXPECT_EQ(metadata.at(1).asText(), "one");
  EXPECT_EQ(metadata.values().at(1).asText(), "two");
  EXPECT_EQ(metadata.entryAt(0).first, 1U);
  EXPECT_EQ(metadata.entries().size(), 2U);
  EXPECT_FALSE(metadata.get(3).has_value());
  EXPECT_THROW(metadata.at(3), std::out_of_range);
  EXPECT_THROW(metadata.entryAt(2), std::out_of_range);
  EXPECT_EQ(metadata.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(TransactionMetadata::fromCbor(metadata.toCbor()), metadata);
  EXPECT_EQ(metadata.toJson(), R"([{"key":"1","value":{"tag":"string","value":"one"}},{"key":"2","value":{"tag":"string","value":"two"}}])");
}

TEST(TransactionMetadata, PreservesFullUnsignedLabelRange)
{
  // Arrange
  TransactionMetadata metadata;
  metadata.set(UINT64_MAX, Metadatum(std::int64_t { 42 }));
  CborReader expected("a11bffffffffffffffff182a");

  // Act & Assert
  EXPECT_EQ(metadata.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(TransactionMetadata::fromCbor("a11bffffffffffffffff182a"), metadata);
  EXPECT_EQ(metadata.get(UINT64_MAX)->asInteger(), BigInt(42));
}

TEST(TransactionMetadata, RejectsDuplicateLabelsAndMalformedCbor)
{
  // Act & Assert
  EXPECT_THROW(TransactionMetadata::fromCbor("a200000001"), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor("bf00000001ff"), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor("a000"), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor("xx"), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(TransactionMetadata::fromCbor("80"), Error);
  EXPECT_THROW(TransactionMetadata::fromCbor("a12000"), Error);
  EXPECT_TRUE(TransactionMetadata::fromCbor("bfff").empty());
  EXPECT_EQ(TransactionMetadata().toJson(), "[]");
}
