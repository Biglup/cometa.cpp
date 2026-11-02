/**
 * @file RelayTest.cpp
 *
 * @brief Unit tests for Relay.
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

TEST(Relay, ExposesAllVariantsAndRejectsWrongAccessors)
{
  // Arrange
  const SingleHostAddrRelay address(3001, Ipv4::fromString("127.0.0.1"));
  const SingleHostNameRelay name("example.com", 3001);
  const MultiHostNameRelay  multiple("example.com");

  // Act
  const Relay first(address);
  const Relay second(name);
  const Relay third(multiple);

  // Assert
  EXPECT_EQ(first.type(), RelayType::SingleHostAddress);
  EXPECT_EQ(second.type(), RelayType::SingleHostName);
  EXPECT_EQ(third.type(), RelayType::MultiHostName);
  EXPECT_EQ(first.asSingleHostAddr(), address);
  EXPECT_EQ(second.asSingleHostName(), name);
  EXPECT_EQ(third.asMultiHostName(), multiple);
  EXPECT_EQ(first.toCbor(), address.toCbor());
  EXPECT_EQ(second.toJson(), name.toJson());
  EXPECT_EQ(third.toJson(), multiple.toJson());
  EXPECT_THROW(first.asSingleHostName(), Error);
  EXPECT_THROW(first.asMultiHostName(), Error);
  EXPECT_THROW(second.asSingleHostAddr(), Error);
  EXPECT_THROW(third.asSingleHostName(), Error);
  EXPECT_NE(first, second);
}

TEST(Relay, CopiesInputAndReturnedVariants)
{
  // Arrange
  SingleHostNameRelay source("example.com");
  const Relay         relay(source);
  SingleHostNameRelay child = relay.asSingleHostName();

  // Act
  source.setDns("a.b");
  child.setDns("c.d");

  // Assert
  EXPECT_EQ(relay.asSingleHostName().dns(), "example.com");
  EXPECT_EQ(source.dns(), "a.b");
  EXPECT_EQ(child.dns(), "c.d");
}

TEST(Relay, DecodesAllVariantsAndRejectsMalformedValues)
{
  // Arrange
  const std::vector<Relay> relays { Relay(SingleHostAddrRelay()), Relay(SingleHostNameRelay("example.com")), Relay(MultiHostNameRelay("example.com")) };

  // Act & Assert
  for (const Relay& relay: relays)
  {
    EXPECT_EQ(Relay::fromCbor(relay.toCbor()), relay);
  }
  EXPECT_EQ(Relay::fromCbor("8400f6f6f6"), relays.at(0));
  EXPECT_THROW(Relay::fromCbor("820360"), Error);
  EXPECT_THROW(Relay::fromCbor("84001a00010000f6f6"), Error);
  EXPECT_THROW(Relay::fromCbor("8400f6f6f600"), std::invalid_argument);
  EXPECT_THROW(Relay::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(Relay::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}
