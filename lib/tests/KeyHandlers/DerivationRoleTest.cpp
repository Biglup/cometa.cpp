/**
 * @file DerivationRoleTest.cpp
 *
 * @brief Unit tests for Cardano key roles.
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

TEST(DerivationRole, ValuesMatchNativeRoles)
{
  // Arrange
  const Cometa::DerivationRole role = Cometa::DerivationRole::External;

  // Act
  const std::uint32_t external = static_cast<std::uint32_t>(role);

  // Assert
  EXPECT_EQ(external, CARDANO_CIP_1852_ROLE_EXTERNAL);
  EXPECT_EQ(static_cast<std::uint32_t>(Cometa::DerivationRole::Internal), CARDANO_CIP_1852_ROLE_INTERNAL);
  EXPECT_EQ(static_cast<std::uint32_t>(Cometa::DerivationRole::Staking), CARDANO_CIP_1852_ROLE_STAKING);
  EXPECT_EQ(static_cast<std::uint32_t>(Cometa::DerivationRole::DRep), CARDANO_CIP_1852_ROLE_DREP);
  EXPECT_EQ(static_cast<std::uint32_t>(Cometa::DerivationRole::CommitteeCold), CARDANO_CIP_1852_ROLE_CC_COLD);
  EXPECT_EQ(static_cast<std::uint32_t>(Cometa::DerivationRole::CommitteeHot), CARDANO_CIP_1852_ROLE_CC_HOT);
}
