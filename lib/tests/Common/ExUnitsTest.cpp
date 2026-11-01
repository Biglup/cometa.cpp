/**
 * @file ExUnitsTest.cpp
 *
 * @brief Tests for ExUnits.
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

#include <Cometa/Common/ExUnits.h>
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

TEST(ExUnits, MatchesKnownEncoding)
{
  // Arrange
  const ExUnits     value(147852369874563ULL, 369852147852369ULL);
  const std::string cbor = "821b000086788ffc4e831b00015060e9e46451";

  // Act
  const ExUnits     decoded = ExUnits::fromCbor(cbor);
  const std::string json    = value.toJson();

  // Assert
  EXPECT_EQ(decoded, value);
  EXPECT_EQ(decoded.memory(), 147852369874563ULL);
  EXPECT_EQ(decoded.cpuSteps(), 369852147852369ULL);
  EXPECT_EQ(decoded.toCbor(), value.toCbor());
  EXPECT_EQ(json, "{\"mem\":\"147852369874563\",\"steps\":\"369852147852369\"}");
}

TEST(ExUnits, PreservesFullUnsignedRange)
{
  // Arrange
  const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max();
  const ExUnits       original(maximum, maximum);

  // Act
  const std::vector<std::uint8_t> bytes   = original.toCbor();
  const ExUnits                   decoded = ExUnits::fromCbor(bytes);

  // Assert
  EXPECT_EQ(decoded.memory(), maximum);
  EXPECT_EQ(decoded.cpuSteps(), maximum);
  EXPECT_NE(decoded.toJson().find("18446744073709551615"), std::string::npos);
  EXPECT_EQ(bytes, (std::vector<std::uint8_t> { 0x82, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }));
}

TEST(ExUnits, MutatingCopyDoesNotChangeOriginal)
{
  // Arrange
  const ExUnits original(1, 2);
  ExUnits       copy = original;

  // Act
  copy.setMemory(3);
  copy.setCpuSteps(4);

  // Assert
  EXPECT_EQ(original.memory(), 1);
  EXPECT_EQ(original.cpuSteps(), 2);
  EXPECT_EQ(copy.memory(), 3);
  EXPECT_EQ(copy.cpuSteps(), 4);
  EXPECT_NE(copy, original);
}

TEST(ExUnits, RejectsMalformedInput)
{
  // Act & Assert
  EXPECT_THROW(ExUnits::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ExUnits::fromCbor("0"), std::invalid_argument);
  EXPECT_THROW(ExUnits::fromCbor("zz"), std::invalid_argument);
  EXPECT_THROW(ExUnits::fromCbor("821b000086788ffc4e831b00015060e9e4645100"), std::invalid_argument);
  EXPECT_THROW(ExUnits::fromCbor("8100"), Error);
  EXPECT_THROW(ExUnits::fromCbor("822000"), Error);
  EXPECT_THROW(ExUnits::fromCbor("8200"), Error);
}

TEST(ExUnits, MovedFromValuesRejectAccessAndMutation)
{
  // Arrange
  ExUnits       original(1, 2);
  const ExUnits moved = std::move(original);

  // Act & Assert
  EXPECT_THROW(original.memory(), std::logic_error);
  EXPECT_THROW(original.cpuSteps(), std::logic_error);
  EXPECT_THROW(original.setMemory(0), std::logic_error);
  EXPECT_THROW(original.setCpuSteps(0), std::logic_error);
  EXPECT_THROW(original.toJson(), std::logic_error);
  EXPECT_EQ(moved.memory(), 1);
}
