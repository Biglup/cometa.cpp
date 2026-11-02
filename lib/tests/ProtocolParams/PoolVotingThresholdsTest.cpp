/**
 * @file PoolVotingThresholdsTest.cpp
 *
 * @brief Unit tests for governance voting thresholds.
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

TEST(PoolVotingThresholds, MatchesNativeCborFixture)
{
  // Arrange
  CborReader reader("85d81e820000d81e820101d81e820202d81e820303d81e820404");

  // Act
  const PoolVotingThresholds value(
    UnitInterval(0, 0),
    UnitInterval(1, 1),
    UnitInterval(2, 2),
    UnitInterval(3, 3),
    UnitInterval(4, 4));
  const PoolVotingThresholds decoded = PoolVotingThresholds::fromCbor("85d81e820000d81e820101d81e820202d81e820303d81e820404");

  // Assert
  EXPECT_EQ(value.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, value);
  EXPECT_FALSE(value.toJson().empty());
  EXPECT_EQ(value.motionNoConfidence(), UnitInterval(0, 0));
  EXPECT_EQ(value.committeeNormal(), UnitInterval(1, 1));
  EXPECT_EQ(value.committeeNoConfidence(), UnitInterval(2, 2));
  EXPECT_EQ(value.hardForkInitiation(), UnitInterval(3, 3));
  EXPECT_EQ(value.securityRelevantParam(), UnitInterval(4, 4));
}

TEST(PoolVotingThresholds, SettersAndCopiesAreIndependent)
{
  // Arrange
  PoolVotingThresholds       value = PoolVotingThresholds::fromCbor("85d81e820000d81e820101d81e820202d81e820303d81e820404");
  const PoolVotingThresholds copied(value);
  PoolVotingThresholds       assigned = value;

  // Act
  value.setMotionNoConfidence(UnitInterval(7, 8));
  value.setCommitteeNormal(UnitInterval(7, 8));
  value.setCommitteeNoConfidence(UnitInterval(7, 8));
  value.setHardForkInitiation(UnitInterval(7, 8));
  value.setSecurityRelevantParam(UnitInterval(7, 8));
  UnitInterval retrieved = value.motionNoConfidence();
  retrieved.setNumerator(9);

  // Assert
  EXPECT_EQ(value.motionNoConfidence(), UnitInterval(7, 8));
  EXPECT_EQ(value.committeeNormal(), UnitInterval(7, 8));
  EXPECT_EQ(value.committeeNoConfidence(), UnitInterval(7, 8));
  EXPECT_EQ(value.hardForkInitiation(), UnitInterval(7, 8));
  EXPECT_EQ(value.securityRelevantParam(), UnitInterval(7, 8));
  EXPECT_EQ(copied.motionNoConfidence(), UnitInterval(0, 0));
  EXPECT_EQ(assigned.motionNoConfidence(), UnitInterval(0, 0));
  EXPECT_EQ(retrieved, UnitInterval(9, 8));
}
