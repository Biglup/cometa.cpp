/**
 * @file TransactionInputTest.cpp
 *
 * @brief Unit tests for TransactionInput.
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

TEST(TransactionInput, PreservesTransactionIdAndFullWidthIndex)
{
  // Arrange
  const Blake2bHash id = Blake2bHash::fromHex("0101010101010101010101010101010101010101010101010101010101010101");
  TransactionInput  value(id, 5);
  CborReader        expected("825820010101010101010101010101010101010101010101010101010101010101010105");

  // Act & Assert
  EXPECT_EQ(value.id(), id);
  EXPECT_EQ(value.index(), 5U);
  EXPECT_EQ(value.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(value.toJson(), R"({"transaction_id":"0101010101010101010101010101010101010101010101010101010101010101","index":5})");

  // Act
  value.setIndex(std::numeric_limits<std::uint64_t>::max());

  // Assert
  EXPECT_EQ(value.index(), std::numeric_limits<std::uint64_t>::max());
  EXPECT_EQ(TransactionInput::fromCbor(value.toCbor()), value);
}

TEST(TransactionInput, CopiesIdsAndAssignmentsIndependently)
{
  // Arrange
  TransactionInput  original = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  TransactionInput  copy(original);
  const Blake2bHash replacement = Blake2bHash::fromHex(std::string(64, '2'));

  // Act
  copy.setId(replacement);
  copy.setIndex(8);

  // Assert
  EXPECT_EQ(original.id().toHex(), "0101010101010101010101010101010101010101010101010101010101010101");
  EXPECT_EQ(original.index(), 5U);
  EXPECT_EQ(copy.id(), replacement);
  EXPECT_EQ(copy.index(), 8U);
}

TEST(TransactionInput, OrdersByIdThenUnsignedIndex)
{
  // Arrange
  const TransactionInput first = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  const TransactionInput next(first.id(), std::numeric_limits<std::uint64_t>::max());
  const TransactionInput last(Blake2bHash::fromHex(std::string(64, '2')), 0);

  // Act & Assert
  EXPECT_LT(first, next);
  EXPECT_LT(next, last);
  EXPECT_GT(last, first);
  EXPECT_LE(first, first);
  EXPECT_GE(first, first);
  EXPECT_NE(first, next);
}

TEST(TransactionInput, RejectsInvalidIdsWithoutMutation)
{
  // Arrange
  TransactionInput       value = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  const TransactionInput before(value);
  const Blake2bHash      shortId = Blake2bHash::fromHex("01");

  // Act & Assert
  EXPECT_THROW(TransactionInput(shortId, 0), Error);
  EXPECT_THROW(value.setId(shortId), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromHex("xz", 0), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromCbor("82410100"), Error);
  EXPECT_EQ(value, before);
}

TEST(TransactionInput, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const TransactionInput    value = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  std::vector<std::uint8_t> bytes = value.toCbor();
  EXPECT_EQ(TransactionInput::fromCbor(bytes), value);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(TransactionInput::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(TransactionInput::fromCbor("f5"), Error);
}

TEST(TransactionInput, MatchesSharedSerializationAndInspectionFixtures)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/serialization.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0U;

  // Act & Assert
  for (std::size_t index = 0U; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);
    if (item.at("type").asString() != "transaction_input")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const TransactionInput value = TransactionInput::fromCbor(item.at("input").at("cbor").asString());
    CborReader             expected(item.at("expected").at("cbor").asString());

    EXPECT_EQ(value.toCbor(), expected.readEncodedValue());
    EXPECT_EQ(value.toJson(), item.at("expected").at("inspection").toJson());
    ++checked;
  }

  EXPECT_EQ(checked, 1U);
}

TEST(TransactionInput, RejectsSharedInvalidFixtures)
{
  // Arrange
  std::ifstream input(std::string(COMETA_TEST_VECTOR_DIR) + "/invalid.json", std::ios::binary);
  ASSERT_TRUE(input);
  const std::string text((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
  const JsonValue   cases   = JsonValue::parse(text).at("cases");
  std::size_t       checked = 0U;

  // Act & Assert
  for (std::size_t index = 0U; index < cases.size(); ++index)
  {
    const JsonValue item = cases.at(index);
    if (item.at("type").asString() != "transaction_input")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    EXPECT_THROW(TransactionInput::fromCbor(item.at("input").at("cbor").asString()), Error);
    ++checked;
  }

  EXPECT_EQ(checked, 2U);
}
