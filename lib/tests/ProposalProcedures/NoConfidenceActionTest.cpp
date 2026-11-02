/**
 * @file NoConfidenceActionTest.cpp
 *
 * @brief Unit tests for no confidence governance actions.
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

TEST(NoConfidenceAction, MatchesNativeCborWithAbsentPreviousAction)
{
  // Arrange
  const NoConfidenceAction action;
  CborReader               reader("8203f6");

  // Act
  const NoConfidenceAction decoded = NoConfidenceAction::fromCbor("8203f6");

  // Assert
  EXPECT_EQ(action.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, action);
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_EQ(action.toJson(), "{\"tag\":\"no_confidence\"}");
}

TEST(NoConfidenceAction, CopiesAndChangesPreviousActionIndependently)
{
  // Arrange
  const GovernanceActionId previous(Blake2bHash::fromHex(std::string(64, '0')), 0);
  const std::string        cbor = "8203825820" + std::string(64, '0') + "00";
  NoConfidenceAction       action(previous);
  const NoConfidenceAction copied(action);

  // Act
  action.setPreviousAction(std::nullopt);

  // Assert
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_EQ(copied.previousAction(), previous);
  EXPECT_EQ(NoConfidenceAction::fromCbor(cbor), copied);
}
