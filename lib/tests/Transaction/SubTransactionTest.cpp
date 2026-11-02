/**
 * @file SubTransactionTest.cpp
 *
 * @brief Unit tests for Dijkstra sub-transactions.
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

TEST(SubTransaction, MinimalValueMatchesNativeCborAndId)
{
  // Arrange
  const SubTransactionBody body { TransactionInputSet(), TransactionOutputList() };
  const WitnessSet         witnesses;
  const SubTransaction     transaction(body, witnesses);
  CborReader               reader("83a200d90102800180a0f6");

  // Act
  const SubTransaction decoded = SubTransaction::fromCbor(transaction.toCbor());

  // Assert
  EXPECT_EQ(transaction.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(transaction.id().toHex(), "da4603f4488d798af667794ab542f130a4bd1c20c7ed950c4648aaa95d0be4f4");
  EXPECT_EQ(decoded, transaction);
  EXPECT_EQ(transaction.body(), body);
  EXPECT_EQ(transaction.witnessSet(), witnesses);
  EXPECT_FALSE(transaction.auxiliaryData().has_value());
  EXPECT_FALSE(transaction.toJson().empty());
}

TEST(SubTransaction, AuxiliaryDataCanBeReplacedAndCleared)
{
  // Arrange
  const SubTransactionBody body { TransactionInputSet(), TransactionOutputList() };
  const WitnessSet         witnesses;
  SubTransaction           transaction(body, witnesses);
  const AuxiliaryData      auxiliary = AuxiliaryData::fromCbor("a1016474657374");

  // Act
  transaction.setAuxiliaryData(auxiliary);
  const SubTransaction copy(transaction);
  transaction.setAuxiliaryData(std::nullopt);

  // Assert
  ASSERT_TRUE(copy.auxiliaryData().has_value());
  EXPECT_EQ(copy.auxiliaryData(), auxiliary);
  EXPECT_FALSE(transaction.auxiliaryData().has_value());
  EXPECT_EQ(SubTransaction::fromCbor(copy.toCbor()), copy);
  EXPECT_NE(transaction, copy);
}

TEST(SubTransaction, AppliesVerificationKeyWitnesses)
{
  // Arrange
  const SubTransactionBody body { TransactionInputSet(), TransactionOutputList() };
  const WitnessSet         witnesses;
  SubTransaction           transaction(body, witnesses);
  const WitnessSet         source = WitnessSet::fromCbor("a100d90102818258203d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c58406291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a");
  ASSERT_TRUE(source.vkeys().has_value());
  CborReader reader("83a200d90102800180a100d90102818258203d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c58406291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40af6");

  // Act
  transaction.applyVkeyWitnesses(*source.vkeys());

  // Assert
  ASSERT_TRUE(transaction.witnessSet().vkeys().has_value());
  EXPECT_EQ(transaction.witnessSet().vkeys()->size(), 1U);
  EXPECT_EQ(transaction.toCbor(), reader.readEncodedValue());
}
