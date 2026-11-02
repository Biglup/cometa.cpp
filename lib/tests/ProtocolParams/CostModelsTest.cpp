/**
 * @file CostModelsTest.cpp
 *
 * @brief Unit tests for Plutus cost-model collections.
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

TEST(CostModels, MatchesNativeCborMap)
{
  // Arrange
  CostModels      models;
  const CostModel v1(PlutusLanguageVersion::V1, { 1, 2, 3 });
  CborReader      reader("a10083010203");

  // Act
  models.insert(v1);

  // Assert
  EXPECT_EQ(models.toCbor(), reader.readEncodedValue());
  ASSERT_TRUE(models.get(PlutusLanguageVersion::V1).has_value());
  EXPECT_EQ(models.get(PlutusLanguageVersion::V1)->costs(), v1.costs());
  EXPECT_TRUE(models.contains(PlutusLanguageVersion::V1));
  EXPECT_FALSE(models.contains(PlutusLanguageVersion::V2));
  EXPECT_FALSE(models.get(PlutusLanguageVersion::V2).has_value());
  EXPECT_EQ(CostModels::fromCbor("a10083010203"), models);
  EXPECT_FALSE(models.languageViews().empty());
  EXPECT_FALSE(models.toJson().empty());
}

TEST(CostModels, KeepsUnknownLanguagesAndCopiesIndependent)
{
  // Arrange
  CostModels       models = CostModels::fromCbor("a10483010203");
  const CostModels copy(models);
  CostModels       assigned;

  // Act
  assigned = models;
  models.insert(CostModel(PlutusLanguageVersion::V1, { 4, 5, 6 }));
  std::optional<CostModel> retrieved = models.get(4);
  ASSERT_TRUE(retrieved.has_value());
  retrieved->setCost(0, 9);

  // Assert
  EXPECT_EQ(copy.toCbor(), CostModels::fromCbor("a10483010203").toCbor());
  EXPECT_EQ(assigned.get(4)->cost(0), 1);
  EXPECT_EQ(models.get(4)->cost(0), 1);
  EXPECT_EQ(retrieved->cost(0), 9);
  EXPECT_TRUE(models.get(PlutusLanguageVersion::V1).has_value());
}
