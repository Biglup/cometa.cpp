/**
 * @file MetadatumLabelListTest.cpp
 *
 * @brief Unit tests for MetadatumLabelList.
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

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(MetadatumLabelList, SupportsStandardVectorOperations)
{
  // Arrange
  MetadatumLabelList       labels { 2, 1 };
  const MetadatumLabelList original(labels);

  // Act
  labels.push_back(UINT64_MAX);
  std::sort(labels.begin(), labels.end());

  // Assert
  EXPECT_EQ(labels, (MetadatumLabelList { 1, 2, UINT64_MAX }));
  EXPECT_EQ(original, (MetadatumLabelList { 2, 1 }));
  EXPECT_THROW(labels.at(3), std::out_of_range);
}
