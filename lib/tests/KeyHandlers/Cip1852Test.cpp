/**
 * @file Cip1852Test.cpp
 *
 * @brief Unit tests for Cardano derivation constants.
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

#include <cardano/key_handlers/cip_1852_constants.h>

#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(Cip1852, ConstantsMatchNativeValues)
{
  // Arrange
  const std::uint32_t standard = Cometa::Cip1852::StandardPurpose;

  // Act
  const std::uint32_t coinType = Cometa::Cip1852::CoinType;

  // Assert
  EXPECT_EQ(standard, CARDANO_CIP_1852_PURPOSE_STANDARD);
  EXPECT_EQ(Cometa::Cip1852::MultisigPurpose, CARDANO_CIP_1852_PURPOSE_MULTI_SIG);
  EXPECT_EQ(coinType, CARDANO_CIP_1852_COIN_TYPE);
}
