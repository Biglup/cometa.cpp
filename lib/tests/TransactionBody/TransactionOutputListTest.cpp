/**
 * @file TransactionOutputListTest.cpp
 *
 * @brief Unit tests for TransactionOutputList.
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

TEST(TransactionOutputList, PreservesOrderAndIndependentOutputs)
{
  // Arrange
  TransactionOutput first = TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  TransactionOutput second(first);
  second.setAmount(Value(20));
  TransactionOutputList values { first, second };

  // Act
  first.setAmount(Value(30));
  TransactionOutput returned = values.at(0);
  returned.setAmount(Value(40));
  values.pushBack(first);

  // Assert
  EXPECT_EQ(values.size(), 3U);
  EXPECT_FALSE(values.empty());
  EXPECT_EQ(values.at(0).amount().coin(), 10);
  EXPECT_EQ(values.at(1).amount().coin(), 20);
  EXPECT_EQ(values.at(2).amount().coin(), 30);
  EXPECT_EQ(returned.amount().coin(), 40);
  EXPECT_THROW(values.at(3), std::out_of_range);
  EXPECT_EQ(TransactionOutputList::fromCbor(values.toCbor()), values);
}

TEST(TransactionOutputList, NormalizesEmptyIndefiniteArrays)
{
  // Arrange
  const TransactionOutputList values = TransactionOutputList::fromCbor("9fff");
  CborReader                  expected("80");

  // Act & Assert
  EXPECT_TRUE(values.empty());
  EXPECT_EQ(values.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(values.toJson(), "[]");
}

TEST(TransactionOutputList, RejectsMalformedCborAndTrailingBytes)
{
  // Arrange
  const TransactionOutputList value = TransactionOutputList { TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10)) };
  std::vector<std::uint8_t>   bytes = value.toCbor();
  EXPECT_EQ(TransactionOutputList::fromCbor(bytes), value);
  bytes.push_back(0);

  // Act & Assert
  EXPECT_THROW(TransactionOutputList::fromCbor(bytes), std::invalid_argument);
  EXPECT_THROW(TransactionOutputList::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(TransactionOutputList::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
  EXPECT_THROW(TransactionOutputList::fromCbor("f"), std::invalid_argument);
  EXPECT_THROW(TransactionOutputList::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(TransactionOutputList::fromCbor("f5"), Error);
}

TEST(TransactionOutputList, IteratorEditsReachParentsAndSurviveSerialization)
{
  // Arrange
  const TransactionOutput     output(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  TransactionOutputList       outputs { output };
  TransactionOutput&          element = *outputs.begin();
  const TransactionOutputList copy(outputs);

  // Act
  element.setAmount(Value(20));
  outputs.toCbor();
  element.setAmount(Value(30));
  TransactionBody body(TransactionInputSet(), outputs, 0);
  element.setAmount(Value(40));

  // Assert
  EXPECT_EQ(TransactionOutputList::fromCbor(outputs.toCbor()).front().amount().coin(), 40);
  EXPECT_EQ(body.outputs().front().amount().coin(), 30);
  EXPECT_EQ(copy.front().amount().coin(), 10);
  EXPECT_EQ(std::addressof(outputs.front()), std::addressof(element));
}
