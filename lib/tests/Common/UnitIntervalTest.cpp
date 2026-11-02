/**
 * @file UnitIntervalTest.cpp
 *
 * @brief Tests for UnitInterval.
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

#include <Cometa/Common/UnitInterval.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(UnitInterval, MatchesKnownEncoding)
{
  // Arrange
  const UnitInterval              value(3, 4);
  const std::vector<std::uint8_t> expected { 0xd8, 0x1e, 0x82, 0x03, 0x04 };

  // Act
  const UnitInterval decoded = UnitInterval::fromCbor("d81e820304");

  // Assert
  EXPECT_EQ(decoded, value);
  EXPECT_EQ(value.toCbor(), expected);
  EXPECT_EQ(UnitInterval::fromCbor(expected), value);
  EXPECT_EQ(value.toJson(), "{\"numerator\":\"3\",\"denominator\":\"4\"}");
  EXPECT_DOUBLE_EQ(value.toDouble(), 0.75);
}

TEST(UnitInterval, PreservesComponentsWithoutReduction)
{
  // Arrange
  const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max();
  const UnitInterval  value(maximum, maximum);

  // Act
  const UnitInterval decoded = UnitInterval::fromCbor(value.toCbor());

  // Assert
  EXPECT_EQ(decoded.numerator(), maximum);
  EXPECT_EQ(decoded.denominator(), maximum);
  EXPECT_NE(decoded, UnitInterval(1, 1));
  EXPECT_DOUBLE_EQ(UnitInterval(3, 2).toDouble(), 1.5);
}

TEST(UnitInterval, MutatingCopyDoesNotChangeOriginal)
{
  // Arrange
  const UnitInterval original(3, 4);
  UnitInterval       copy = original;

  // Act
  copy.setNumerator(5);
  copy.setDenominator(6);

  // Assert
  EXPECT_EQ(original, UnitInterval(3, 4));
  EXPECT_EQ(copy, UnitInterval(5, 6));
}

TEST(UnitInterval, PreservesZeroDenominatorButRejectsDivision)
{
  // Arrange
  const UnitInterval value(1, 0);

  // Act
  const UnitInterval decoded = UnitInterval::fromCbor(value.toCbor());

  // Assert
  EXPECT_EQ(decoded, value);
  EXPECT_THROW(decoded.toDouble(), std::domain_error);
}

TEST(UnitInterval, ConvertsFiniteNonnegativeNumbers)
{
  // Act & Assert
  EXPECT_EQ(UnitInterval::fromDouble(0.0), UnitInterval(0, 1));
  EXPECT_EQ(UnitInterval::fromDouble(-0.0), UnitInterval(0, 1));
  EXPECT_EQ(UnitInterval::fromDouble(0.75), UnitInterval(3, 4));
  EXPECT_EQ(UnitInterval::fromDouble(2.5), UnitInterval(5, 2));
  EXPECT_EQ(UnitInterval::fromDouble(1000000000000.5), UnitInterval(2000000000001ULL, 2));
  EXPECT_DOUBLE_EQ(UnitInterval::fromDouble(std::nextafter(std::ldexp(1.0, 64), 0.0)).toDouble(), std::nextafter(std::ldexp(1.0, 64), 0.0));
}

TEST(UnitInterval, RejectsUnsafeFloatingPointConversions)
{
  // Act & Assert
  EXPECT_THROW(UnitInterval::fromDouble(-1.0), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromDouble(std::numeric_limits<double>::infinity()), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromDouble(std::numeric_limits<double>::quiet_NaN()), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromDouble(std::ldexp(1.0, 64)), std::overflow_error);
}

TEST(UnitInterval, RejectsMalformedInput)
{
  // Act & Assert
  EXPECT_THROW(UnitInterval::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromCbor("0"), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromCbor("zz"), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromCbor("d81e82030400"), std::invalid_argument);
  EXPECT_THROW(UnitInterval::fromCbor("d81e8100"), Error);
  EXPECT_THROW(UnitInterval::fromCbor("d81e822000"), Error);
  EXPECT_THROW(UnitInterval::fromCbor("d81e8200"), Error);
}

TEST(UnitInterval, MovedFromValuesRejectAccessAndMutation)
{
  // Arrange
  UnitInterval       original(3, 4);
  const UnitInterval moved = std::move(original);

  // Act & Assert
  EXPECT_THROW(original.numerator(), std::logic_error);
  EXPECT_THROW(original.denominator(), std::logic_error);
  EXPECT_THROW(original.setNumerator(0), std::logic_error);
  EXPECT_THROW(original.setDenominator(0), std::logic_error);
  EXPECT_THROW(original.toDouble(), std::logic_error);
  EXPECT_THROW(original.toJson(), std::logic_error);
  EXPECT_EQ(moved.numerator(), 3);
}
