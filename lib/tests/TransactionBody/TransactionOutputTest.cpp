/**
 * @file TransactionOutputTest.cpp
 *
 * @brief Unit tests for TransactionOutput.
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

TEST(TransactionOutput, CopiesMandatoryFieldsAndOptionalValues)
{
  // Arrange
  const Address     address = Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6");
  Value             amount(10);
  TransactionOutput output(address, amount);
  const Datum       datum(PlutusData(std::int64_t { 42 }));
  const Script      script(PlutusV4Script::fromHex("450100002499"));

  // Act
  amount.setCoin(11);
  output.setDatum(datum);
  output.setScriptRef(script);
  Value returned = output.amount();
  returned.setCoin(12);

  // Assert
  EXPECT_EQ(amount.coin(), 11);
  EXPECT_EQ(returned.coin(), 12);
  EXPECT_EQ(output.address(), address);
  EXPECT_EQ(output.amount().coin(), 10);
  ASSERT_TRUE(output.datum());
  EXPECT_EQ(*output.datum(), datum);
  ASSERT_TRUE(output.scriptRef());
  EXPECT_EQ(*output.scriptRef(), script);
  EXPECT_EQ(TransactionOutput::fromCbor(output.toCbor()), output);
}

TEST(TransactionOutput, RemovesOptionalFieldsAndReplacesCopiedFields)
{
  // Arrange
  TransactionOutput output = TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  output.setDatum(Datum(PlutusData(std::int64_t { 1 })));
  output.setScriptRef(Script(PlutusV1Script::fromHex("01")));
  const TransactionOutput original(output);

  // Act
  output.setDatum(std::nullopt);
  output.setScriptRef(std::nullopt);
  output.setAmount(Value(20));
  output.setAddress(Address::fromString("addr1vyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygatvcjl"));

  // Assert
  EXPECT_FALSE(output.datum());
  EXPECT_FALSE(output.scriptRef());
  EXPECT_EQ(output.amount().coin(), 20);
  EXPECT_NE(output.address(), original.address());
  EXPECT_TRUE(original.datum());
  EXPECT_TRUE(original.scriptRef());
  EXPECT_EQ(original.amount().coin(), 10);
}

TEST(TransactionOutput, RejectsNegativeLovelace)
{
  // Arrange
  const Address     address = Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6");
  TransactionOutput output(address, Value(10));

  // Act & Assert
  EXPECT_THROW(TransactionOutput(address, Value(-1)), std::invalid_argument);
  EXPECT_THROW(output.setAmount(Value(-1)), std::invalid_argument);
  EXPECT_EQ(output.amount().coin(), 10);
}

TEST(TransactionOutput, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const TransactionOutput   value = TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  std::vector<std::uint8_t> bytes = value.toCbor();
  EXPECT_EQ(TransactionOutput::fromCbor(bytes), value);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(TransactionOutput::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(TransactionOutput::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(TransactionOutput::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(TransactionOutput::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(TransactionOutput::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(TransactionOutput::fromCbor("f5"), Error);
}

TEST(TransactionOutput, MatchesSharedSerializationAndInspectionFixtures)
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
    if (item.at("type").asString() != "transaction_output")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    const TransactionOutput value = TransactionOutput::fromCbor(item.at("input").at("cbor").asString());
    CborReader              expected(item.at("expected").at("cbor").asString());

    EXPECT_EQ(value.toCbor(), expected.readEncodedValue());
    EXPECT_EQ(value.toJson(), item.at("expected").at("inspection").toJson());
    ++checked;
  }

  EXPECT_EQ(checked, 7U);
}

TEST(TransactionOutput, RejectsSharedInvalidFixtures)
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
    if (item.at("type").asString() != "transaction_output")
    {
      continue;
    }

    SCOPED_TRACE(item.at("id").asString());
    EXPECT_THROW(TransactionOutput::fromCbor(item.at("input").at("cbor").asString()), Error);
    ++checked;
  }

  EXPECT_EQ(checked, 2U);
}
