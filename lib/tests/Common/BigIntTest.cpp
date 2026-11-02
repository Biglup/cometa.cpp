/**
 * @file BigIntTest.cpp
 *
 * @brief Tests for BigInt.
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

#include <Cometa/Common/BigInt.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(BigInt, PreservesSignedAndUnsignedLimits)
{
  // Arrange
  const std::int64_t  minimum         = std::numeric_limits<std::int64_t>::min();
  const std::int64_t  maximum         = std::numeric_limits<std::int64_t>::max();
  const std::uint64_t unsignedMaximum = std::numeric_limits<std::uint64_t>::max();

  // Act & Assert
  EXPECT_EQ(BigInt(minimum).toString(), "-9223372036854775808");
  EXPECT_EQ(BigInt(minimum).toInt64(), minimum);
  EXPECT_EQ(BigInt(maximum).toInt64(), maximum);
  EXPECT_EQ(BigInt::fromUnsigned(unsignedMaximum).toUInt64(), unsignedMaximum);
  EXPECT_THROW(BigInt::fromUnsigned(unsignedMaximum).toInt64(), std::overflow_error);
  EXPECT_THROW(BigInt(-1).toUInt64(), std::overflow_error);
  EXPECT_THROW(BigInt::fromString("18446744073709551616").toUInt64(), std::overflow_error);
  EXPECT_THROW(BigInt::fromString("-9223372036854775809").toInt64(), std::overflow_error);
}

TEST(BigInt, ParsesBoundedStringsAndExplicitBases)
{
  // Arrange
  const std::string digits = "123456ignored";

  // Act & Assert
  EXPECT_EQ(BigInt::fromString(std::string_view(digits.data(), 6)), BigInt(123456));
  EXPECT_EQ(BigInt::fromString("+123"), BigInt(123));
  EXPECT_EQ(BigInt::fromString("-FF", 16), BigInt(-255));
  EXPECT_EQ(BigInt::fromString("z", 36), BigInt(35));
  EXPECT_EQ(BigInt::fromString("Z", 62), BigInt(35));
  EXPECT_EQ(BigInt::fromString("z", 62), BigInt(61));
  EXPECT_EQ(BigInt(-255).toString(16), "-ff");
  EXPECT_EQ(BigInt(61).toString(62), "z");
  EXPECT_EQ(BigInt().toString(), "0");
}

TEST(BigInt, RejectsInvalidDigitsAndBases)
{
  // Act & Assert
  EXPECT_THROW(BigInt::fromString(""), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("-"), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("+"), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString(" 1"), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("1 2"), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("0xff", 16), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("2", 2), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString(std::string("1\0", 2)), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("1", 0), std::invalid_argument);
  EXPECT_THROW(BigInt::fromString("1", 63), std::invalid_argument);
  EXPECT_THROW(BigInt(1).toString(1), std::invalid_argument);
}

TEST(BigInt, ImportsAndExportsUnsignedMagnitude)
{
  // Arrange
  const std::vector<std::uint8_t> bigEndian { 1, 2, 3 };
  const std::vector<std::uint8_t> littleEndian { 3, 2, 1 };

  // Act
  const BigInt value = BigInt::fromBytes(bigEndian);

  // Assert
  EXPECT_EQ(value, BigInt(66051));
  EXPECT_EQ(value.toBytes(), bigEndian);
  EXPECT_EQ(value.toBytes(ByteOrder::LittleEndian), littleEndian);
  EXPECT_EQ(BigInt::fromBytes(littleEndian, ByteOrder::LittleEndian), value);
  EXPECT_EQ((-value).toBytes(), bigEndian);
  EXPECT_EQ(BigInt::fromBytes({}), BigInt(0));
  EXPECT_EQ(BigInt(0).toBytes(), (std::vector<std::uint8_t> { 0 }));
  EXPECT_EQ(BigInt::fromBytes({ 0, 0, 1 }).toBytes(), (std::vector<std::uint8_t> { 1 }));
  EXPECT_THROW(value.toBytes(static_cast<ByteOrder>(2)), std::invalid_argument);
  EXPECT_THROW(BigInt::fromBytes({}, static_cast<ByteOrder>(2)), std::invalid_argument);
}

TEST(BigInt, ComputesBeyondMachineWordSize)
{
  // Arrange
  const BigInt value = BigInt::fromString("18446744073709551616");

  // Act
  const BigInt squared = value * value;

  // Assert
  EXPECT_EQ(squared.toString(), "340282366920938463463374607431768211456");
  EXPECT_EQ(squared / value, value);
  EXPECT_EQ((value + BigInt(1)).toString(), "18446744073709551617");
  EXPECT_EQ((value - BigInt(1)).toString(), "18446744073709551615");
  EXPECT_EQ(value, BigInt(1) << 64);
}

TEST(BigInt, DivisionTruncatesTowardZero)
{
  // Arrange
  const BigInt dividend(-17);
  const BigInt divisor(5);

  // Act
  const std::pair<BigInt, BigInt> divided = dividend.divRem(divisor);

  // Assert
  EXPECT_EQ(divided.first, BigInt(-3));
  EXPECT_EQ(divided.second, BigInt(-2));
  EXPECT_EQ(dividend / divisor, divided.first);
  EXPECT_EQ(dividend % divisor, divided.second);
  EXPECT_EQ(divided.first * divisor + divided.second, dividend);
  EXPECT_EQ(BigInt(17) / BigInt(-5), BigInt(-3));
  EXPECT_EQ(BigInt(17) % BigInt(-5), BigInt(2));
}

TEST(BigInt, RejectsZeroDivisorsWithoutMutation)
{
  // Arrange
  BigInt value(17);

  // Act & Assert
  EXPECT_THROW(value / BigInt(), std::domain_error);
  EXPECT_THROW(value % BigInt(), std::domain_error);
  EXPECT_THROW(value.divRem(BigInt()), std::domain_error);
  EXPECT_THROW(value /= BigInt(), std::domain_error);
  EXPECT_THROW(value %= BigInt(), std::domain_error);
  EXPECT_EQ(value, BigInt(17));
}

TEST(BigInt, ComputesPowersAndGreatestCommonDivisors)
{
  // Act & Assert
  EXPECT_EQ(BigInt(2).pow(128).toString(), "340282366920938463463374607431768211456");
  EXPECT_EQ(BigInt(0).pow(0), BigInt(1));
  EXPECT_EQ(BigInt(-2).pow(3), BigInt(-8));
  EXPECT_EQ(BigInt(-1).pow(4294967296ULL), BigInt(1));
  EXPECT_EQ(BigInt(-1).pow(4294967297ULL), BigInt(-1));
  EXPECT_EQ(BigInt(-48).gcd(BigInt(18)), BigInt(6));
  EXPECT_EQ(BigInt().gcd(BigInt()), BigInt());
  EXPECT_EQ(BigInt(-12).abs(), BigInt(12));
}

TEST(BigInt, ComputesModularPowersAndInverses)
{
  // Act & Assert
  EXPECT_EQ(BigInt(4).modPow(BigInt(13), BigInt(497)), BigInt(445));
  EXPECT_EQ(BigInt(3).modInverse(BigInt(11)), BigInt(4));
  EXPECT_EQ(BigInt(3).modPow(BigInt(-1), BigInt(11)), BigInt(4));
  EXPECT_EQ(BigInt(-3).modInverse(BigInt(11)), BigInt(7));
  EXPECT_EQ(BigInt(3).modInverse(BigInt(1)), BigInt(0));
  EXPECT_EQ(BigInt(0).modPow(BigInt(-1), BigInt(1)), BigInt(0));
  EXPECT_THROW(BigInt(2).modInverse(BigInt(4)), std::domain_error);
  EXPECT_THROW(BigInt(2).modInverse(BigInt(0)), std::domain_error);
  EXPECT_THROW(BigInt(2).modPow(BigInt(-1), BigInt(4)), std::domain_error);
  EXPECT_THROW(BigInt(2).modPow(BigInt(3), BigInt(0)), std::domain_error);
  EXPECT_THROW(BigInt(2).modPow(BigInt(3), BigInt(-5)), std::domain_error);
}

TEST(BigInt, UsesTwosComplementBitwiseSemantics)
{
  // Act & Assert
  EXPECT_EQ(BigInt(10) & BigInt(12), BigInt(8));
  EXPECT_EQ(BigInt(10) | BigInt(12), BigInt(14));
  EXPECT_EQ(BigInt(10) ^ BigInt(12), BigInt(6));
  EXPECT_EQ(~BigInt(10), BigInt(-11));
  EXPECT_EQ(BigInt(-1) & BigInt(12), BigInt(12));
  EXPECT_EQ(BigInt(-9) >> 1, BigInt(-5));
  EXPECT_TRUE(BigInt(-1).testBit(100));
  EXPECT_EQ(BigInt(-15).bitCount(), 4);
  EXPECT_EQ(BigInt(-15).bitLength(), 4);
  EXPECT_EQ(BigInt(0).bitLength(), 1);
  EXPECT_EQ(BigInt(0).bitCount(), 0);
}

TEST(BigInt, MutatesBitsIndependently)
{
  // Arrange
  const BigInt original(1);
  BigInt       copy = original;

  // Act
  copy.setBit(100);
  copy.clearBit(0);
  copy.flipBit(1);

  // Assert
  EXPECT_TRUE(copy.testBit(100));
  EXPECT_FALSE(copy.testBit(0));
  EXPECT_TRUE(copy.testBit(1));
  EXPECT_EQ(original, BigInt(1));
}

TEST(BigInt, CompoundArithmeticSupportsAliasing)
{
  // Arrange
  BigInt value(3);

  // Act
  value += value;
  value *= value;
  value -= BigInt(1);
  value /= BigInt(2);
  value %= BigInt(10);

  // Assert
  EXPECT_EQ(value, BigInt(7));
}

TEST(BigInt, CompoundBitwiseOperationsSupportAliasing)
{
  // Arrange
  BigInt value(10);

  // Act
  value |= BigInt(4);
  value &= BigInt(12);
  value ^= BigInt(6);
  value <<= 2;
  value >>= 1;

  // Assert
  EXPECT_EQ(value, BigInt(20));
}

TEST(BigInt, IncrementAndDecrementReturnExpectedValues)
{
  // Arrange
  BigInt value(-1);

  // Act & Assert
  EXPECT_EQ(value++, BigInt(-1));
  EXPECT_EQ(++value, BigInt(1));
  EXPECT_EQ(value--, BigInt(1));
  EXPECT_EQ(--value, BigInt(-1));
}

TEST(BigInt, ComparesSignedValues)
{
  // Act & Assert
  EXPECT_EQ(BigInt(1), BigInt(1));
  EXPECT_NE(BigInt(-1), BigInt(1));
  EXPECT_LT(BigInt(-1), BigInt(0));
  EXPECT_LE(BigInt(1), BigInt(1));
  EXPECT_GT(BigInt(1), BigInt(-1));
  EXPECT_GE(BigInt(1), BigInt(1));
  EXPECT_EQ(BigInt(-100).sign(), -1);
  EXPECT_EQ(BigInt(0).sign(), 0);
  EXPECT_EQ(BigInt(100).sign(), 1);
  EXPECT_TRUE(BigInt(0).isZero());
  EXPECT_FALSE(BigInt(1).isZero());
}

TEST(BigInt, SupportsIndependentCopyAndMove)
{
  // Arrange
  BigInt original = BigInt(1) << 100;
  BigInt copy     = original;

  // Act
  copy         += BigInt(1);
  BigInt moved = std::move(original);
  original     = copy;
  swap(original, moved);

  // Assert
  EXPECT_EQ(original, BigInt(1) << 100);
  EXPECT_EQ(moved, copy);
  EXPECT_NE(original, copy);
}

TEST(BigInt, SupportsSelfAssignmentAndMovedFromReassignment)
{
  // Arrange
  BigInt  value(123);
  BigInt* alias = &value;

  // Act
  value        = *alias;
  value        = std::move(*alias);
  BigInt moved = std::move(value);
  value        = moved;
  moved        = std::move(value);
  swap(value, moved);

  // Assert
  EXPECT_EQ(value, BigInt(123));
  EXPECT_THROW(moved.toString(), std::logic_error);
}

TEST(BigInt, MovedFromValuesRejectOperations)
{
  // Arrange
  BigInt       original(1);
  const BigInt moved = std::move(original);

  // Act & Assert
  EXPECT_THROW(original.toString(), std::logic_error);
  EXPECT_THROW(original.toInt64(), std::logic_error);
  EXPECT_THROW(original.toUInt64(), std::logic_error);
  EXPECT_THROW(original.toBytes(), std::logic_error);
  EXPECT_THROW(original.sign(), std::logic_error);
  EXPECT_THROW(original.isZero(), std::logic_error);
  EXPECT_THROW(original.setBit(0), std::logic_error);
  EXPECT_THROW(original + moved, std::logic_error);
  EXPECT_THROW(original.pow(0), std::logic_error);
  EXPECT_EQ(moved, BigInt(1));
}
