/**
 * @file ParameterChangeActionTest.cpp
 *
 * @brief Unit tests for parameter change actions.
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

TEST(ParameterChangeAction, MatchesNativeCborWithoutOptionalFields)
{
  // Arrange
  ProtocolParamUpdate update;
  update.setMinFeeA(std::uint64_t(44));
  const ParameterChangeAction action(update);
  CborReader                  reader("8400f6a100182cf6");

  // Act
  const ParameterChangeAction decoded = ParameterChangeAction::fromCbor("8400f6a100182cf6");

  // Assert
  EXPECT_EQ(action.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, action);
  EXPECT_EQ(action.update(), update);
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_FALSE(action.policyHash().has_value());
  EXPECT_FALSE(action.toJson().empty());
}

TEST(ParameterChangeAction, CopiesAllFieldsIndependently)
{
  // Arrange
  ProtocolParamUpdate update;
  update.setMinFeeA(std::uint64_t(44));
  const GovernanceActionId    previous(Blake2bHash::fromHex(std::string(64, '0')), 0);
  const Blake2bHash           policy = Blake2bHash::fromHex(std::string(56, '1'));
  ParameterChangeAction       action(update, previous, policy);
  const ParameterChangeAction copied(action);

  // Act
  update.setMinFeeA(std::uint64_t(99));
  action.setUpdate(update);
  action.setPreviousAction(std::nullopt);
  action.setPolicyHash(std::nullopt);

  // Assert
  EXPECT_EQ(copied.update().minFeeA(), std::optional<std::uint64_t>(44));
  EXPECT_EQ(copied.previousAction(), previous);
  EXPECT_EQ(copied.policyHash(), policy);
  EXPECT_EQ(action.update().minFeeA(), std::optional<std::uint64_t>(99));
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_FALSE(action.policyHash().has_value());
  EXPECT_EQ(ParameterChangeAction::fromCbor(copied.toCbor()), copied);
}
