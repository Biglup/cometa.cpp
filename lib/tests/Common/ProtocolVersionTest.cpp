/**
 * @file ProtocolVersionTest.cpp
 *
 * @brief Tests for ProtocolVersion.
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

#include <Cometa/Common/ProtocolVersion.h>
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

TEST(ProtocolVersion, MatchesKnownEncoding)
{
  // Arrange
  const ProtocolVersion value(12ULL, 0ULL);
  const std::string     cbor = "820c00";

  // Act
  const ProtocolVersion decoded = ProtocolVersion::fromCbor(cbor);
  const std::string     json    = value.toJson();

  // Assert
  EXPECT_EQ(decoded, value);
  EXPECT_EQ(decoded.major(), 12ULL);
  EXPECT_EQ(decoded.minor(), 0ULL);
  EXPECT_EQ(decoded.toCbor(), value.toCbor());
  EXPECT_EQ(json, "{\"major\":12,\"minor\":0}");
}

TEST(ProtocolVersion, PreservesProtocolIntegerRanges)
{
  // Arrange
  const std::uint64_t   maximum = std::numeric_limits<std::uint64_t>::max();
  const ProtocolVersion original(maximum, std::numeric_limits<std::uint32_t>::max());

  // Act
  const std::vector<std::uint8_t> bytes   = original.toCbor();
  const ProtocolVersion           decoded = ProtocolVersion::fromCbor(bytes);

  // Assert
  EXPECT_EQ(decoded.major(), maximum);
  EXPECT_EQ(decoded.minor(), std::numeric_limits<std::uint32_t>::max());
  EXPECT_NE(decoded.toJson().find("18446744073709551615"), std::string::npos);
  EXPECT_EQ(bytes, (std::vector<std::uint8_t> { 0x82, 0x1b, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0xff, 0xff, 0xff, 0xff }));
}

TEST(ProtocolVersion, MutatingCopyDoesNotChangeOriginal)
{
  // Arrange
  const ProtocolVersion original(1, 2);
  ProtocolVersion       copy = original;

  // Act
  copy.setMajor(3);
  copy.setMinor(4);

  // Assert
  EXPECT_EQ(original.major(), 1);
  EXPECT_EQ(original.minor(), 2);
  EXPECT_EQ(copy.major(), 3);
  EXPECT_EQ(copy.minor(), 4);
  EXPECT_NE(copy, original);
}

TEST(ProtocolVersion, RejectsMalformedInput)
{
  // Act & Assert
  EXPECT_THROW(ProtocolVersion::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(ProtocolVersion::fromCbor("0"), std::invalid_argument);
  EXPECT_THROW(ProtocolVersion::fromCbor("zz"), std::invalid_argument);
  EXPECT_THROW(ProtocolVersion::fromCbor("820c0000"), std::invalid_argument);
  EXPECT_THROW(ProtocolVersion::fromCbor("8100"), Error);
  EXPECT_THROW(ProtocolVersion::fromCbor("822000"), Error);
  EXPECT_THROW(ProtocolVersion::fromCbor("8200"), Error);
}

TEST(ProtocolVersion, MovedFromValuesRejectAccessAndMutation)
{
  // Arrange
  ProtocolVersion       original(1, 2);
  const ProtocolVersion moved = std::move(original);

  // Act & Assert
  EXPECT_THROW(original.major(), std::logic_error);
  EXPECT_THROW(original.minor(), std::logic_error);
  EXPECT_THROW(original.setMajor(0), std::logic_error);
  EXPECT_THROW(original.setMinor(0), std::logic_error);
  EXPECT_THROW(original.toJson(), std::logic_error);
  EXPECT_EQ(moved.major(), 1);
}

TEST(ProtocolVersion, RejectsMinorVersionOverflowWithoutChangingValue)
{
  // Arrange
  ProtocolVersion     value(12, 0);
  const std::uint64_t invalid = static_cast<std::uint64_t>(std::numeric_limits<std::uint32_t>::max()) + 1;

  // Act & Assert
  EXPECT_THROW(ProtocolVersion(12, invalid), std::invalid_argument);
  EXPECT_THROW(value.setMinor(invalid), std::invalid_argument);
  EXPECT_EQ(value.minor(), 0);
  EXPECT_THROW(ProtocolVersion::fromCbor("820c1b0000000100000000"), Error);
}
