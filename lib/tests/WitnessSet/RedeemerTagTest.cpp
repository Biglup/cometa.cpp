/**
 * @file RedeemerTagTest.cpp
 *
 * @brief Unit tests for redeemer purposes.
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

#include <cardano/witness_set/redeemer_tag.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(RedeemerTag, MirrorsNativePurposes)
{
  // Arrange
  const RedeemerTag            purposes[] = { RedeemerTag::Spend, RedeemerTag::Mint, RedeemerTag::Certifying, RedeemerTag::Reward, RedeemerTag::Voting, RedeemerTag::Proposing, RedeemerTag::Guarding };
  const cardano_redeemer_tag_t native[]   = { CARDANO_REDEEMER_TAG_SPEND, CARDANO_REDEEMER_TAG_MINT, CARDANO_REDEEMER_TAG_CERTIFYING, CARDANO_REDEEMER_TAG_REWARD, CARDANO_REDEEMER_TAG_VOTING, CARDANO_REDEEMER_TAG_PROPOSING, CARDANO_REDEEMER_TAG_GUARDING };

  // Act & Assert
  for (std::size_t index = 0; index < 7U; ++index)
  {
    EXPECT_EQ(static_cast<int>(purposes[index]), static_cast<int>(native[index]));
    EXPECT_FALSE(toString(purposes[index]).empty());
  }
}
