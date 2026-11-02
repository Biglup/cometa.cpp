/**
 * @file NewConstitutionActionTest.cpp
 *
 * @brief Unit tests for new constitution actions.
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

TEST(NewConstitutionAction, MatchesNativeCborWithoutPreviousAction)
{
  // Arrange
  const Constitution          constitution(Anchor("https://a", Blake2bHash::fromHex(std::string(64, '0'))));
  const NewConstitutionAction action(constitution);
  const std::string           cbor = "8305f682826968747470733a2f2f615820" + std::string(64, '0') + "f6";
  CborReader                  reader(cbor);

  // Act
  const NewConstitutionAction decoded = NewConstitutionAction::fromCbor(cbor);

  // Assert
  EXPECT_EQ(action.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, action);
  EXPECT_EQ(action.constitution(), constitution);
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_FALSE(action.toJson().empty());
}

TEST(NewConstitutionAction, CopiesAndSetsBothFieldsIndependently)
{
  // Arrange
  const Constitution          constitution(Anchor("https://a", Blake2bHash::fromHex(std::string(64, '0'))));
  const GovernanceActionId    previous(Blake2bHash::fromHex(std::string(64, '1')), 0);
  NewConstitutionAction       action(constitution, previous);
  const NewConstitutionAction copied(action);

  // Act
  action.setConstitution(Constitution(Anchor("https://b", Blake2bHash::fromHex(std::string(64, '2')))));
  action.setPreviousAction(std::nullopt);

  // Assert
  EXPECT_EQ(copied.constitution(), constitution);
  EXPECT_EQ(copied.previousAction(), previous);
  EXPECT_NE(action, copied);
  EXPECT_EQ(NewConstitutionAction::fromCbor(copied.toCbor()), copied);
}
