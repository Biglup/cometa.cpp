/**
 * @file ByronAddressTypeTest.cpp
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

#include <Cometa/Address/ByronAddressType.h>

#include <cardano/address/byron_address_type.h>
#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(ByronAddressType, MatchesNativeVariants)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Cometa::ByronAddressType::PublicKey), CARDANO_BYRON_ADDRESS_TYPE_PUBKEY);
  EXPECT_EQ(static_cast<int>(Cometa::ByronAddressType::Script), CARDANO_BYRON_ADDRESS_TYPE_SCRIPT);
  EXPECT_EQ(static_cast<int>(Cometa::ByronAddressType::Redeem), CARDANO_BYRON_ADDRESS_TYPE_REDEEM);
}
