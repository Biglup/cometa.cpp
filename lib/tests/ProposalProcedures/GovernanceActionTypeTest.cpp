/**
 * @file GovernanceActionTypeTest.cpp
 *
 * @brief Tests for governance action discriminators.
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

#include <cardano/proposal_procedures/governance_action_type.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(GovernanceActionType, MatchesNativeDiscriminatorsAndNames)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(GovernanceActionType::ParameterChange), CARDANO_GOVERNANCE_ACTION_TYPE_PARAMETER_CHANGE);
  EXPECT_EQ(toString(GovernanceActionType::ParameterChange), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_PARAMETER_CHANGE));
  EXPECT_EQ(static_cast<int>(GovernanceActionType::HardForkInitiation), CARDANO_GOVERNANCE_ACTION_TYPE_HARD_FORK_INITIATION);
  EXPECT_EQ(toString(GovernanceActionType::HardForkInitiation), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_HARD_FORK_INITIATION));
  EXPECT_EQ(static_cast<int>(GovernanceActionType::TreasuryWithdrawals), CARDANO_GOVERNANCE_ACTION_TYPE_TREASURY_WITHDRAWALS);
  EXPECT_EQ(toString(GovernanceActionType::TreasuryWithdrawals), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_TREASURY_WITHDRAWALS));
  EXPECT_EQ(static_cast<int>(GovernanceActionType::NoConfidence), CARDANO_GOVERNANCE_ACTION_TYPE_NO_CONFIDENCE);
  EXPECT_EQ(toString(GovernanceActionType::NoConfidence), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_NO_CONFIDENCE));
  EXPECT_EQ(static_cast<int>(GovernanceActionType::UpdateCommittee), CARDANO_GOVERNANCE_ACTION_TYPE_UPDATE_COMMITTEE);
  EXPECT_EQ(toString(GovernanceActionType::UpdateCommittee), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_UPDATE_COMMITTEE));
  EXPECT_EQ(static_cast<int>(GovernanceActionType::NewConstitution), CARDANO_GOVERNANCE_ACTION_TYPE_NEW_CONSTITUTION);
  EXPECT_EQ(toString(GovernanceActionType::NewConstitution), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_NEW_CONSTITUTION));
  EXPECT_EQ(static_cast<int>(GovernanceActionType::Info), CARDANO_GOVERNANCE_ACTION_TYPE_INFO);
  EXPECT_EQ(toString(GovernanceActionType::Info), cardano_governance_action_type_to_string(CARDANO_GOVERNANCE_ACTION_TYPE_INFO));
}
