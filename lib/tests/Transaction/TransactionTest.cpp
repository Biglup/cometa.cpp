/**
 * @file TransactionTest.cpp
 *
 * @brief Unit tests for top-level transactions.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

TEST(Transaction, MinimalValueRoundTripsThroughNativeCbor)
{
  // Arrange
  const TransactionBody body(TransactionInputSet(), TransactionOutputList(), 0);
  const WitnessSet      witnesses;
  const Transaction     transaction(body, witnesses);
  CborReader            reader("84a300d901028001800200a0f5f6");

  // Act
  const Transaction decoded = Transaction::fromCbor(transaction.toCbor());

  // Assert
  EXPECT_EQ(decoded, transaction);
  EXPECT_EQ(transaction.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(transaction.body(), body);
  EXPECT_EQ(transaction.witnessSet(), witnesses);
  EXPECT_EQ(transaction.id(), body.hash());
  EXPECT_TRUE(transaction.isValid());
  EXPECT_FALSE(transaction.hasScriptData());
  EXPECT_FALSE(transaction.auxiliaryData().has_value());
  EXPECT_FALSE(transaction.toJson().empty());
}

TEST(Transaction, ValidityAndBodyChangesRefreshCbor)
{
  // Arrange
  const TransactionBody body(TransactionInputSet(), TransactionOutputList(), 0);
  Transaction           transaction(body, WitnessSet());
  const Transaction     previous(transaction);
  TransactionBody       updated(body);
  updated.setFee(100);

  // Act
  transaction.setBody(updated);
  transaction.setValid(false);

  // Assert
  EXPECT_FALSE(transaction.isValid());
  EXPECT_NE(transaction.id(), previous.id());
  EXPECT_EQ(transaction.body(), updated);
  EXPECT_EQ(Transaction::fromCbor(transaction.toCbor()), transaction);
}

TEST(Transaction, AppliesVerificationKeyWitnesses)
{
  // Arrange
  const TransactionBody body(TransactionInputSet(), TransactionOutputList(), 0);
  Transaction           transaction(body, WitnessSet());
  const WitnessSet      source = WitnessSet::fromCbor("a100d90102818258203d4017c3e843895a92b70aa74d1b7ebc9c982ccf2ec4968cc0cd55f12af4660c58406291d657deec24024827e69c3abe01a30ce548a284743a445e3680d7db5ac3ac18ff9b538d16f290ae67f760984dc6594a7c15e9716ed28dc027beceea1ec40a");
  ASSERT_TRUE(source.vkeys().has_value());

  // Act
  transaction.applyVkeyWitnesses(*source.vkeys());

  // Assert
  ASSERT_TRUE(transaction.witnessSet().vkeys().has_value());
  EXPECT_EQ(transaction.witnessSet().vkeys()->size(), 1U);
  EXPECT_EQ(Transaction::fromCbor(transaction.toCbor()), transaction);
}

TEST(Transaction, ReturnsAbsentIndexesForMissingComponents)
{
  // Arrange
  const TransactionBody  body(TransactionInputSet(), TransactionOutputList(), 0);
  const Transaction      transaction(body, WitnessSet());
  const TransactionInput missing = TransactionInput::fromHex(std::string(64, '0'), 0);
  const Blake2bHash      policy  = Blake2bHash::fromHex(std::string(56, '1'));

  // Act and assert
  EXPECT_FALSE(transaction.findInputIndex(missing).has_value());
  EXPECT_FALSE(transaction.findReferenceInputIndex(missing).has_value());
  EXPECT_FALSE(transaction.findMintPolicyIndex(policy).has_value());
  EXPECT_FALSE(transaction.findRedeemerIndex(RedeemerTag::Spend, 0).has_value());
  EXPECT_TRUE(transaction.uniqueSigners().empty());
}

TEST(Transaction, FindsInputAndOutputAndResolvedSigner)
{
  // Arrange
  const TransactionInput  input      = TransactionInput::fromHex(std::string(64, '0'), 1);
  const Credential        credential = Credential::fromCbor("8200581c00112233445566778899aabbccddeeff00112233445566778899aabb");
  const EnterpriseAddress enterprise(NetworkId::Testnet, credential);
  const Address           address = Address::fromString(enterprise.toString());
  const TransactionOutput output(address, Value(2'000'000));
  TransactionInputSet     inputs;
  TransactionOutputList   outputs;
  inputs.insert(input);
  outputs.pushBack(output);
  const Transaction transaction(TransactionBody(inputs, outputs, 100), WitnessSet());
  const UtxoList    resolvedInputs { Utxo(input, output) };

  // Act
  const Blake2bHashSet signers = transaction.uniqueSigners(resolvedInputs);

  // Assert
  EXPECT_EQ(transaction.findInputIndex(input), 0U);
  EXPECT_EQ(transaction.findOutputIndex(address, 1'000'000), 0U);
  EXPECT_FALSE(transaction.findOutputIndex(address, 3'000'000).has_value());
  EXPECT_TRUE(signers.contains(credential.hash()));
}

TEST(Transaction, PreservesDijkstraSubTransactions)
{
  // Arrange
  TransactionBody         body(TransactionInputSet(), TransactionOutputList(), 0);
  const SubTransactionSet subTransactions = SubTransactionSet::fromCbor("d901028183a200d90102800180a0f6");
  body.setSubTransactions(subTransactions);
  const Transaction transaction(body, WitnessSet());

  // Act
  const Transaction decoded = Transaction::fromCbor(transaction.toCbor());

  // Assert
  ASSERT_TRUE(decoded.body().subTransactions().has_value());
  EXPECT_EQ(decoded.body().subTransactions(), subTransactions);
  EXPECT_EQ(decoded.id(), transaction.id());
}

TEST(Transaction, PreservesBlockLayoutWhenDecodedAndCopied)
{
  // Arrange
  const std::string encoded = "84a300d901028001800200a0f6f4";
  CborReader        reader(encoded);

  // Act
  const Transaction transaction = Transaction::fromCbor(encoded);
  const Transaction copy(transaction);

  // Assert
  EXPECT_FALSE(transaction.isValid());
  EXPECT_FALSE(copy.isValid());
  EXPECT_EQ(transaction.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(copy.toCbor(), transaction.toCbor());
  EXPECT_EQ(copy.id(), transaction.id());
}
