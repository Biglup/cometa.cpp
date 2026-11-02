/**
 * @file UtxoTest.cpp
 *
 * @brief Unit tests for Utxo.
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

TEST(Utxo, CopiesAndReplacesComponentsIndependently)
{
  // Arrange
  TransactionInput  input  = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  TransactionOutput output = TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  Utxo              utxo(input, output);
  const Utxo        original(utxo);

  // Act
  input.setIndex(8);
  output.setAmount(Value(20));
  utxo.setInput(input);
  utxo.setOutput(output);
  TransactionOutput returned = original.output();
  returned.setAmount(Value(30));

  // Assert
  EXPECT_EQ(original.input().index(), 5U);
  EXPECT_EQ(original.output().amount().coin(), 10);
  EXPECT_EQ(utxo.input().index(), 8U);
  EXPECT_EQ(utxo.output().amount().coin(), 20);
  EXPECT_EQ(returned.amount().coin(), 30);
  EXPECT_EQ(Utxo::fromCbor(utxo.toCbor()), utxo);
  EXPECT_EQ(JsonValue::parse(utxo.toJson()).type(), JsonType::Object);
}

TEST(Utxo, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const Utxo                value = Utxo(TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5), TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10)));
  std::vector<std::uint8_t> bytes = value.toCbor();
  EXPECT_EQ(Utxo::fromCbor(bytes), value);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(Utxo::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(Utxo::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(Utxo::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(Utxo::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(Utxo::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(Utxo::fromCbor("f5"), Error);
}
