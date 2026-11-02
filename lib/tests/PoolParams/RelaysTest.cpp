/**
 * @file RelaysTest.cpp
 *
 * @brief Unit tests for Relays.
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

TEST(Relays, PreservesOrderAndDuplicates)
{
  // Arrange
  const Relay first { SingleHostAddrRelay() };
  const Relay second(MultiHostNameRelay("example.com"));
  Relays      relays { first, second };
  CborReader  expected("838400f6f6f682026b6578616d706c652e636f6d8400f6f6f6");

  // Act
  relays.pushBack(first);

  // Assert
  EXPECT_EQ(relays.size(), 3U);
  EXPECT_FALSE(relays.empty());
  EXPECT_EQ(relays.at(0), first);
  EXPECT_EQ(relays.at(1), second);
  EXPECT_EQ(relays.at(2), first);
  EXPECT_EQ(relays.values(), (std::vector<Relay> { first, second, first }));
  EXPECT_EQ(relays.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(Relays::fromCbor(relays.toCbor()), relays);
  EXPECT_EQ(Relays::fromCbor("80"), Relays());
  EXPECT_EQ(JsonValue::parse(relays.toJson()).size(), 3U);
}

TEST(Relays, CopiesInsertedAndReturnedRelays)
{
  // Arrange
  Relay        source(SingleHostNameRelay("example.com"));
  Relays       relays { source };
  const Relays original(relays);
  Relay        returned = relays.at(0);
  EXPECT_EQ(returned, source);

  // Act
  source   = Relay(MultiHostNameRelay("a.b"));
  returned = source;
  relays.pushBack(source);

  // Assert
  EXPECT_EQ(original.size(), 1U);
  EXPECT_EQ(original.at(0).asSingleHostName().dns(), "example.com");
  EXPECT_EQ(relays.at(0), original.at(0));
  EXPECT_EQ(relays.at(1), returned);
}

TEST(Relays, RejectsOutOfRangeAndMalformedValues)
{
  // Arrange
  const Relays empty;

  // Act & Assert
  EXPECT_TRUE(empty.empty());
  EXPECT_THROW(empty.at(0), std::out_of_range);
  EXPECT_THROW(Relays::fromCbor("8100"), Error);
  EXPECT_THROW(Relays::fromCbor("8000"), std::invalid_argument);
  EXPECT_THROW(Relays::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(Relays::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}
