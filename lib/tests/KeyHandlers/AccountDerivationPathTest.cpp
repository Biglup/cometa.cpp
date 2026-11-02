/**
 * @file AccountDerivationPathTest.cpp
 *
 * @brief Unit tests for Cardano account paths.
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

TEST(AccountDerivationPath, DefaultsToStandardCardanoAccount)
{
  // Arrange
  const Cometa::AccountDerivationPath path;

  // Act
  const Cometa::AccountDerivationPath copy(path);

  // Assert
  EXPECT_EQ(path.purpose, 1852U);
  EXPECT_EQ(path.coinType, 1815U);
  EXPECT_EQ(path.account, 0U);
  EXPECT_EQ(copy, path);
}

TEST(AccountDerivationPath, ComparesAllSegments)
{
  // Arrange
  const Cometa::AccountDerivationPath first { 1852, 1815, 3 };
  const Cometa::AccountDerivationPath second { 1854, 1815, 3 };

  // Act
  const bool different = first != second;

  // Assert
  EXPECT_TRUE(different);
  EXPECT_FALSE(first == second);
}
