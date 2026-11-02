/**
 * @file DRepVotingThresholdsTest.cpp
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

TEST(DRepVotingThresholds, MatchesNativeCborFixture)
{
  // Arrange
  CborReader reader("8ad81e820000d81e820101d81e820202d81e820303d81e820404d81e820505d81e820606d81e820707d81e820808d81e820909");

  // Act
  const DRepVotingThresholds value(
    UnitInterval(0, 0),
    UnitInterval(1, 1),
    UnitInterval(2, 2),
    UnitInterval(3, 3),
    UnitInterval(4, 4),
    UnitInterval(5, 5),
    UnitInterval(6, 6),
    UnitInterval(7, 7),
    UnitInterval(8, 8),
    UnitInterval(9, 9));
  const DRepVotingThresholds decoded = DRepVotingThresholds::fromCbor("8ad81e820000d81e820101d81e820202d81e820303d81e820404d81e820505d81e820606d81e820707d81e820808d81e820909");

  // Assert
  EXPECT_EQ(value.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, value);
  EXPECT_FALSE(value.toJson().empty());
  EXPECT_EQ(value.motionNoConfidence(), UnitInterval(0, 0));
  EXPECT_EQ(value.committeeNormal(), UnitInterval(1, 1));
  EXPECT_EQ(value.committeeNoConfidence(), UnitInterval(2, 2));
  EXPECT_EQ(value.updateConstitution(), UnitInterval(3, 3));
  EXPECT_EQ(value.hardForkInitiation(), UnitInterval(4, 4));
  EXPECT_EQ(value.ppNetworkGroup(), UnitInterval(5, 5));
  EXPECT_EQ(value.ppEconomicGroup(), UnitInterval(6, 6));
  EXPECT_EQ(value.ppTechnicalGroup(), UnitInterval(7, 7));
  EXPECT_EQ(value.ppGovernanceGroup(), UnitInterval(8, 8));
  EXPECT_EQ(value.treasuryWithdrawal(), UnitInterval(9, 9));
}

TEST(DRepVotingThresholds, SettersAndCopiesAreIndependent)
{
  // Arrange
  DRepVotingThresholds       value = DRepVotingThresholds::fromCbor("8ad81e820000d81e820101d81e820202d81e820303d81e820404d81e820505d81e820606d81e820707d81e820808d81e820909");
  const DRepVotingThresholds copied(value);
  DRepVotingThresholds       assigned = value;

  // Act
  value.setMotionNoConfidence(UnitInterval(7, 8));
  value.setCommitteeNormal(UnitInterval(7, 8));
  value.setCommitteeNoConfidence(UnitInterval(7, 8));
  value.setUpdateConstitution(UnitInterval(7, 8));
  value.setHardForkInitiation(UnitInterval(7, 8));
  value.setPpNetworkGroup(UnitInterval(7, 8));
  value.setPpEconomicGroup(UnitInterval(7, 8));
  value.setPpTechnicalGroup(UnitInterval(7, 8));
  value.setPpGovernanceGroup(UnitInterval(7, 8));
  value.setTreasuryWithdrawal(UnitInterval(7, 8));
  UnitInterval retrieved = value.motionNoConfidence();
  retrieved.setNumerator(9);

  // Assert
  EXPECT_EQ(value.motionNoConfidence(), UnitInterval(7, 8));
  EXPECT_EQ(value.committeeNormal(), UnitInterval(7, 8));
  EXPECT_EQ(value.committeeNoConfidence(), UnitInterval(7, 8));
  EXPECT_EQ(value.updateConstitution(), UnitInterval(7, 8));
  EXPECT_EQ(value.hardForkInitiation(), UnitInterval(7, 8));
  EXPECT_EQ(value.ppNetworkGroup(), UnitInterval(7, 8));
  EXPECT_EQ(value.ppEconomicGroup(), UnitInterval(7, 8));
  EXPECT_EQ(value.ppTechnicalGroup(), UnitInterval(7, 8));
  EXPECT_EQ(value.ppGovernanceGroup(), UnitInterval(7, 8));
  EXPECT_EQ(value.treasuryWithdrawal(), UnitInterval(7, 8));
  EXPECT_EQ(copied.motionNoConfidence(), UnitInterval(0, 0));
  EXPECT_EQ(assigned.motionNoConfidence(), UnitInterval(0, 0));
  EXPECT_EQ(retrieved, UnitInterval(9, 8));
}
