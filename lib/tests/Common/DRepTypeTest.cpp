/**
 * @file DRepTypeTest.cpp
 *
 * @brief Unit tests for delegated representative kinds.
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

#include <Cometa/Common/DRepType.h>

#include <cardano/common/drep_type.h>

#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(DRepType, MatchesNativeDiscriminators)
{
  EXPECT_EQ(static_cast<int>(Cometa::DRepType::KeyHash), CARDANO_DREP_TYPE_KEY_HASH);
  EXPECT_EQ(static_cast<int>(Cometa::DRepType::ScriptHash), CARDANO_DREP_TYPE_SCRIPT_HASH);
  EXPECT_EQ(static_cast<int>(Cometa::DRepType::Abstain), CARDANO_DREP_TYPE_ABSTAIN);
  EXPECT_EQ(static_cast<int>(Cometa::DRepType::NoConfidence), CARDANO_DREP_TYPE_NO_CONFIDENCE);
}
