/**
 * @file AddressTypeTest.cpp
 *
 * @brief Address tests.
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

#include <Cometa/Address/AddressType.h>

#include <cardano/address/address_type.h>
#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(AddressType, MatchesNativeVariants)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::BasePaymentKeyStakeKey), CARDANO_ADDRESS_TYPE_BASE_PAYMENT_KEY_STAKE_KEY);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::BasePaymentScriptStakeKey), CARDANO_ADDRESS_TYPE_BASE_PAYMENT_SCRIPT_STAKE_KEY);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::BasePaymentKeyStakeScript), CARDANO_ADDRESS_TYPE_BASE_PAYMENT_KEY_STAKE_SCRIPT);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::BasePaymentScriptStakeScript), CARDANO_ADDRESS_TYPE_BASE_PAYMENT_SCRIPT_STAKE_SCRIPT);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::PointerKey), CARDANO_ADDRESS_TYPE_POINTER_KEY);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::PointerScript), CARDANO_ADDRESS_TYPE_POINTER_SCRIPT);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::EnterpriseKey), CARDANO_ADDRESS_TYPE_ENTERPRISE_KEY);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::EnterpriseScript), CARDANO_ADDRESS_TYPE_ENTERPRISE_SCRIPT);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::Byron), CARDANO_ADDRESS_TYPE_BYRON);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::RewardKey), CARDANO_ADDRESS_TYPE_REWARD_KEY);
  EXPECT_EQ(static_cast<int>(Cometa::AddressType::RewardScript), CARDANO_ADDRESS_TYPE_REWARD_SCRIPT);
}
