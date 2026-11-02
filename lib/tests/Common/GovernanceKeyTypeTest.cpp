/**
 * @file GovernanceKeyTypeTest.cpp
 *
 * @brief Tests for GovernanceKeyType.
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

#include <Cometa/Common/GovernanceKeyType.h>

#include <cardano/common/governance_key_type.h>

#include <gtest/gtest.h>

#include <cstdint>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(GovernanceKeyType, MatchesCardanoValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(GovernanceKeyType::CommitteeHot), static_cast<std::uint32_t>(CARDANO_GOVERNANCE_KEY_TYPE_CC_HOT));
  EXPECT_EQ(static_cast<std::uint32_t>(GovernanceKeyType::CommitteeCold), static_cast<std::uint32_t>(CARDANO_GOVERNANCE_KEY_TYPE_CC_COLD));
  EXPECT_EQ(static_cast<std::uint32_t>(GovernanceKeyType::DRep), static_cast<std::uint32_t>(CARDANO_GOVERNANCE_KEY_TYPE_DREP));
}
