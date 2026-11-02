/**
 * @file DerivationPathTest.cpp
 *
 * @brief Unit tests for Cardano key paths.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

/* IMPLEMENTATION ************************************************************/

TEST(DerivationPath, DefaultsToFirstExternalKey)
{
  // Arrange
  const Cometa::DerivationPath path;

  // Act
  const Cometa::AccountDerivationPath account = path.accountPath();

  // Assert
  EXPECT_EQ(account, (Cometa::AccountDerivationPath { 1852, 1815, 0 }));
  EXPECT_EQ(path.role, Cometa::DerivationRole::External);
  EXPECT_EQ(path.index, 0U);
}

TEST(DerivationPath, ComparesEverySegment)
{
  // Arrange
  const Cometa::DerivationPath first { 1852, 1815, 2, Cometa::DerivationRole::DRep, 0 };
  const Cometa::DerivationPath second { 1852, 1815, 2, Cometa::DerivationRole::DRep, 1 };

  // Act
  const bool different = first != second;

  // Assert
  EXPECT_TRUE(different);
  EXPECT_FALSE(first == second);
  EXPECT_EQ(first.accountPath(), second.accountPath());
}
