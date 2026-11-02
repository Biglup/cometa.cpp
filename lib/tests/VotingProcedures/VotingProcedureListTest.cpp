/**
 * @file VotingProcedureListTest.cpp
 *
 * @brief Unit tests for VotingProcedureList.
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

TEST(VotingProcedureList, SupportsIndependentStandardSequences)
{
  // Arrange
  const VotingProcedure value = VotingProcedure(Vote::Yes);
  VotingProcedureList   values { value };
  VotingProcedureList   copy(values);

  // Act
  values.clear();
  copy.push_back(value);

  // Assert
  EXPECT_TRUE(values.empty());
  ASSERT_EQ(copy.size(), 2U);
  EXPECT_EQ(copy.at(0), value);
  EXPECT_EQ(copy.at(1), value);
}
