/**
 * @file CostModelTest.cpp
 *
 * @brief Unit tests for Plutus cost models.
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

TEST(CostModel, MatchesNativeCborFixture)
{
  // Arrange
  const std::vector<std::int64_t> costs = { 1, 2, 3 };
  CborReader                      reader("0083010203");
  std::vector<std::uint8_t>       expected     = reader.readEncodedValue();
  const std::vector<std::uint8_t> encodedCosts = reader.readEncodedValue();
  expected.insert(expected.end(), encodedCosts.begin(), encodedCosts.end());

  // Act
  const CostModel model(PlutusLanguageVersion::V1, costs);
  const CostModel decoded = CostModel::fromCbor("0083010203");

  // Assert
  EXPECT_EQ(model.toCbor(), expected);
  EXPECT_EQ(model.languageId(), 0);
  EXPECT_EQ(model.language(), PlutusLanguageVersion::V1);
  EXPECT_EQ(model.costs(), costs);
  EXPECT_EQ(model.size(), 3);
  EXPECT_EQ(model.cost(1), 2);
  EXPECT_EQ(decoded, model);
  EXPECT_FALSE(model.toJson().empty());
}

TEST(CostModel, PreservesUnknownLanguageId)
{
  // Arrange
  const std::string               cbor = "0483010203";
  CborReader                      reader(cbor);
  std::vector<std::uint8_t>       expected     = reader.readEncodedValue();
  const std::vector<std::uint8_t> encodedCosts = reader.readEncodedValue();
  expected.insert(expected.end(), encodedCosts.begin(), encodedCosts.end());

  // Act
  const CostModel model = CostModel::fromCbor(cbor);

  // Assert
  EXPECT_EQ(model.languageId(), 4);
  EXPECT_FALSE(model.language().has_value());
  EXPECT_EQ(model.toCbor(), expected);
  EXPECT_EQ(model.costs(), (std::vector<std::int64_t> { 1, 2, 3 }));
}

TEST(CostModel, CopiesAndSettersAreIndependent)
{
  // Arrange
  CostModel       model(PlutusLanguageVersion::V2, { 1, 2, 3 });
  const CostModel copy(model);
  CostModel       assigned(PlutusLanguageVersion::V1, { 0 });

  // Act
  assigned = model;
  model.setCost(1, 7);
  std::vector<std::int64_t> values = model.costs();
  values[1]                        = 9;

  // Assert
  EXPECT_EQ(model.cost(1), 7);
  EXPECT_EQ(values[1], 9);
  EXPECT_EQ(copy.cost(1), 2);
  EXPECT_EQ(assigned.cost(1), 2);
  EXPECT_THROW(model.cost(3), std::out_of_range);
  EXPECT_THROW(model.setCost(3, 0), std::out_of_range);
}
