/**
 * @file TransactionBodyTest.cpp
 *
 * @brief Unit tests for top-level transaction bodies.
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

TEST(TransactionBody, MinimalBodyMatchesNativeEncoding)
{
  // Arrange
  const TransactionInputSet   inputs;
  const TransactionOutputList outputs;
  const TransactionBody       body(inputs, outputs, 172497);
  CborReader                  reader("a300d90102800180021a0002a1d1");

  // Act
  const TransactionBody decoded = TransactionBody::fromCbor(body.toCbor());

  // Assert
  EXPECT_EQ(body.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(body.fee(), 172497U);
  EXPECT_EQ(body.inputs(), inputs);
  EXPECT_EQ(body.outputs(), outputs);
  EXPECT_EQ(decoded, body);
  EXPECT_EQ(decoded.hash(), body.hash());
  EXPECT_FALSE(body.toJson().empty());
}

TEST(TransactionBody, UpdatesFeeAndOptionalSlots)
{
  // Arrange
  TransactionBody   body         = TransactionBody::fromCbor("a300d901028001800200");
  const Blake2bHash previousHash = body.hash();

  // Act
  body.setFee(10);
  body.setInvalidAfter(100);
  body.setInvalidBefore(5);
  const TransactionBody copy(body);
  body.setInvalidBefore(std::nullopt);

  // Assert
  EXPECT_EQ(body.fee(), 10U);
  EXPECT_EQ(body.invalidAfter(), 100U);
  EXPECT_FALSE(body.invalidBefore().has_value());
  EXPECT_EQ(copy.invalidBefore(), 5U);
  EXPECT_NE(body.hash(), previousHash);
  EXPECT_EQ(TransactionBody::fromCbor(body.toCbor()), body);
}

TEST(TransactionBody, RetainsDijkstraSubTransactions)
{
  // Arrange
  TransactionBody         body(TransactionInputSet(), TransactionOutputList(), 0);
  const SubTransactionSet subTransactions = SubTransactionSet::fromCbor("d901028183a200d90102800180a0f6");

  // Act
  body.setSubTransactions(subTransactions);
  const TransactionBody copy(body);
  body.setSubTransactions(std::nullopt);

  // Assert
  ASSERT_TRUE(copy.subTransactions().has_value());
  EXPECT_EQ(copy.subTransactions(), subTransactions);
  EXPECT_FALSE(body.subTransactions().has_value());
  EXPECT_EQ(TransactionBody::fromCbor(copy.toCbor()), copy);
}

TEST(TransactionBody, RetainsAdditionalOptionalFields)
{
  // Arrange
  TransactionBody                  body(TransactionInputSet(), TransactionOutputList(), 0);
  const Blake2bHash                signer = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHashSet             signers { signer };
  const AccountBalanceIntervalsMap intervals;

  // Act
  body.setRequiredSigners(signers);
  body.setTotalCollateral(1000);
  body.setStartingAccountBalanceIntervals(intervals);

  // Assert
  ASSERT_TRUE(body.requiredSigners().has_value());
  EXPECT_EQ(body.requiredSigners(), signers);
  EXPECT_EQ(body.totalCollateral(), 1000U);
  ASSERT_TRUE(body.startingAccountBalanceIntervals().has_value());
  EXPECT_EQ(body.startingAccountBalanceIntervals(), intervals);
}

TEST(TransactionBody, ReplacesCollateralAndProtocolUpdate)
{
  // Arrange
  TransactionBody           body(TransactionInputSet(), TransactionOutputList(), 0);
  const TransactionInputSet collateral;
  const Update              update(10, ProposedParamUpdates());
  const Credential          credential = Credential::fromCbor("8200581c00112233445566778899aabbccddeeff00112233445566778899aabb");
  const EnterpriseAddress   enterprise(NetworkId::Testnet, credential);
  const TransactionOutput   collateralReturn(Address::fromString(enterprise.toString()), Value(1'000'000));

  // Act
  body.setCollateral(collateral);
  body.setCollateralReturn(collateralReturn);
  body.setUpdate(update);
  const TransactionBody copy(body);
  body.setCollateral(std::nullopt);
  body.setCollateralReturn(std::nullopt);
  body.setUpdate(std::nullopt);

  // Assert
  EXPECT_EQ(copy.collateral(), collateral);
  EXPECT_EQ(copy.collateralReturn(), collateralReturn);
  EXPECT_EQ(copy.update(), update);
  EXPECT_FALSE(body.collateral().has_value());
  EXPECT_FALSE(body.collateralReturn().has_value());
  EXPECT_FALSE(body.update().has_value());
}
