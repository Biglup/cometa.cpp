/**
 * @file AssetNameListTest.cpp
 *
 * @brief Unit tests for AssetNameList.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/Assets/AssetNameList.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(AssetNameList, CopiesElementsIndependently)
{
  // Arrange
  const AssetNameList source { AssetName::fromString("first") };
  AssetNameList       copy(source);
  const std::string   before = source.front().toHex();

  // Act
  copy.front() = AssetName::fromString("second");
  copy.push_back(source.front());

  // Assert
  EXPECT_EQ(source.size(), 1U);
  EXPECT_EQ(source.front().toHex(), before);
  EXPECT_NE(copy.front(), source.front());
  EXPECT_EQ(copy.back(), source.front());
}
