/**
 * @file UtxoListTest.cpp
 *
 * @brief Unit tests for UtxoList.
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

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
bool
lessByInput(const Utxo& lhs, const Utxo& rhs)
{
  return lhs.input() < rhs.input();
}

bool
isFirstOutput(const Utxo& value)
{
  return value.input().index() == 5U;
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(UtxoList, SupportsStandardAlgorithmsAndIndependentCopies)
{
  // Arrange
  const TransactionInput  input  = TransactionInput::fromHex("0101010101010101010101010101010101010101010101010101010101010101", 5);
  const TransactionOutput output = TransactionOutput(Address::fromString("addr_test1vqg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygxrcya6"), Value(10));
  UtxoList                values { Utxo(TransactionInput(input.id(), 8), output), Utxo(input, output) };
  UtxoList                copy(values);

  // Act
  std::sort(values.begin(), values.end(), lessByInput);
  UtxoList filtered;
  std::copy_if(values.begin(), values.end(), std::back_inserter(filtered), isFirstOutput);
  copy.at(0).setOutput(TransactionOutput(output.address(), Value(20)));

  // Assert
  EXPECT_EQ(values.at(0).input().index(), 5U);
  ASSERT_EQ(filtered.size(), 1U);
  EXPECT_EQ(filtered.at(0).input(), input);
  EXPECT_EQ(copy.at(0).output().amount().coin(), 20);
  EXPECT_EQ(values.at(1).output().amount().coin(), 10);
  EXPECT_NE(std::find_if(values.begin(), values.end(), isFirstOutput), values.end());

  // Act
  copy.insert(copy.end(), values.begin(), values.end());
  copy.erase(copy.begin());

  // Assert
  EXPECT_EQ(copy.size(), 3U);
}
