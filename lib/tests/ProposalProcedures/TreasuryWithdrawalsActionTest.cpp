/**
 * @file TreasuryWithdrawalsActionTest.cpp
 *
 * @brief Unit tests for treasury withdrawal actions.
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

TEST(TreasuryWithdrawalsAction, MatchesNativeCborWithoutPolicyHash)
{
  // Arrange
  const WithdrawalMap             withdrawals;
  const TreasuryWithdrawalsAction action(withdrawals);
  CborReader                      reader("8302a0f6");

  // Act
  const TreasuryWithdrawalsAction decoded = TreasuryWithdrawalsAction::fromCbor("8302a0f6");

  // Assert
  EXPECT_EQ(action.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, action);
  EXPECT_EQ(action.withdrawals(), withdrawals);
  EXPECT_FALSE(action.policyHash().has_value());
  EXPECT_FALSE(action.toJson().empty());
}

TEST(TreasuryWithdrawalsAction, CopiesPolicyHashIndependently)
{
  // Arrange
  const WithdrawalMap             withdrawals;
  const Blake2bHash               policy = Blake2bHash::fromHex(std::string(56, '1'));
  TreasuryWithdrawalsAction       action(withdrawals, policy);
  const TreasuryWithdrawalsAction copied(action);

  // Act
  action.setPolicyHash(std::nullopt);
  action.setWithdrawals(WithdrawalMap());

  // Assert
  EXPECT_EQ(copied.policyHash(), policy);
  EXPECT_FALSE(action.policyHash().has_value());
  EXPECT_EQ(TreasuryWithdrawalsAction::fromCbor(copied.toCbor()), copied);
}
