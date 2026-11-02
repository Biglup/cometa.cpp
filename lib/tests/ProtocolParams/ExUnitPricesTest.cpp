/**
 * @file ExUnitPricesTest.cpp
 *
 * @brief Unit tests for Plutus execution unit prices.
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

TEST(ExUnitPrices, MatchesNativeCborFixture)
{
  // Arrange
  const UnitInterval memory(1, 2);
  const UnitInterval steps(1, 3);
  CborReader         reader("82d81e820102d81e820103");

  // Act
  const ExUnitPrices prices(memory, steps);
  const ExUnitPrices decoded = ExUnitPrices::fromCbor("82d81e820102d81e820103");

  // Assert
  EXPECT_EQ(prices.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(prices.memoryPrice(), memory);
  EXPECT_EQ(prices.stepsPrice(), steps);
  EXPECT_EQ(decoded, prices);
  EXPECT_FALSE(prices.toJson().empty());
}

TEST(ExUnitPrices, SettersAndCopiesAreIndependent)
{
  // Arrange
  ExUnitPrices       prices(UnitInterval(1, 2), UnitInterval(1, 3));
  const ExUnitPrices copy(prices);
  ExUnitPrices       assigned(UnitInterval(0, 1), UnitInterval(0, 1));

  // Act
  assigned = prices;
  prices.setMemoryPrice(UnitInterval(2, 3));
  prices.setStepsPrice(UnitInterval(3, 4));
  UnitInterval retrieved = prices.memoryPrice();
  retrieved.setNumerator(9);

  // Assert
  EXPECT_EQ(copy.memoryPrice(), UnitInterval(1, 2));
  EXPECT_EQ(assigned.stepsPrice(), UnitInterval(1, 3));
  EXPECT_EQ(prices.memoryPrice(), UnitInterval(2, 3));
  EXPECT_EQ(prices.stepsPrice(), UnitInterval(3, 4));
}
