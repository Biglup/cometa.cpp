/**
 * @file HardForkInitiationActionTest.cpp
 *
 * @brief Unit tests for hard fork initiation actions.
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

TEST(HardForkInitiationAction, MatchesNativeCborWithoutPreviousAction)
{
  // Arrange
  const HardForkInitiationAction action(ProtocolVersion(12, 0));
  CborReader                     reader("8301f6820c00");

  // Act
  const HardForkInitiationAction decoded = HardForkInitiationAction::fromCbor("8301f6820c00");

  // Assert
  EXPECT_EQ(action.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, action);
  EXPECT_EQ(action.version(), ProtocolVersion(12, 0));
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_FALSE(action.toJson().empty());
}

TEST(HardForkInitiationAction, CopiesAndUpdatesBothFields)
{
  // Arrange
  const GovernanceActionId       previous(Blake2bHash::fromHex(std::string(64, '0')), 0);
  const std::string              cbor = "8301825820" + std::string(64, '0') + "00820c00";
  HardForkInitiationAction       action(ProtocolVersion(12, 0), previous);
  const HardForkInitiationAction copied(action);

  // Act
  action.setVersion(ProtocolVersion(13, 0));
  action.setPreviousAction(std::nullopt);

  // Assert
  EXPECT_EQ(copied.version(), ProtocolVersion(12, 0));
  EXPECT_EQ(copied.previousAction(), previous);
  EXPECT_EQ(HardForkInitiationAction::fromCbor(cbor), copied);
  EXPECT_EQ(action.version(), ProtocolVersion(13, 0));
  EXPECT_FALSE(action.previousAction().has_value());
}
