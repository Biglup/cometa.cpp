/**
 * @file UpdateCommitteeActionTest.cpp
 *
 * @brief Unit tests for committee update actions.
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

TEST(UpdateCommitteeAction, MatchesNativeCborWithoutPreviousAction)
{
  // Arrange
  const CredentialSet         removed;
  const CommitteeMembersMap   added;
  const UnitInterval          quorum(1, 2);
  const UpdateCommitteeAction action(removed, added, quorum);
  CborReader                  reader("8504f6d9010280a0d81e820102");

  // Act
  const UpdateCommitteeAction decoded = UpdateCommitteeAction::fromCbor("8504f6d9010280a0d81e820102");

  // Assert
  EXPECT_EQ(action.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, action);
  EXPECT_EQ(action.removedMembers(), removed);
  EXPECT_EQ(action.addedMembers(), added);
  EXPECT_EQ(action.quorum(), quorum);
  EXPECT_FALSE(action.previousAction().has_value());
  EXPECT_FALSE(action.toJson().empty());
}

TEST(UpdateCommitteeAction, CopiesAllFieldsIndependently)
{
  // Arrange
  const Credential    credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  const CredentialSet removed { credential };
  CommitteeMembersMap added;
  added.insert(credential, 200);
  const GovernanceActionId    previous(Blake2bHash::fromHex(std::string(64, '1')), 0);
  UpdateCommitteeAction       action(removed, added, UnitInterval(1, 2), previous);
  const UpdateCommitteeAction copied(action);

  // Act
  action.setRemovedMembers(CredentialSet());
  action.setAddedMembers(CommitteeMembersMap());
  action.setQuorum(UnitInterval(2, 3));
  action.setPreviousAction(std::nullopt);

  // Assert
  EXPECT_EQ(copied.removedMembers(), removed);
  EXPECT_EQ(copied.addedMembers(), added);
  EXPECT_EQ(copied.quorum(), UnitInterval(1, 2));
  EXPECT_EQ(copied.previousAction(), previous);
  EXPECT_NE(action, copied);
  EXPECT_EQ(UpdateCommitteeAction::fromCbor(copied.toCbor()), copied);
}
