/**
 * @file SingleHostAddrRelayTest.cpp
 *
 * @brief Unit tests for SingleHostAddrRelay.
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

TEST(SingleHostAddrRelay, MatchesKnownCborAndInspection)
{
  // Arrange
  const Ipv4 address = Ipv4::fromString("127.0.0.1");
  CborReader expected("8400190bb9447f000001f6");

  // Act
  const SingleHostAddrRelay relay(3001, address);

  // Assert
  EXPECT_EQ(relay.port(), 3001U);
  EXPECT_EQ(relay.ipv4(), address);
  EXPECT_FALSE(relay.ipv6());
  EXPECT_EQ(relay.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(SingleHostAddrRelay::fromCbor("8400190bb9447f000001f6"), relay);
  EXPECT_EQ(SingleHostAddrRelay::fromCbor(relay.toCbor()), relay);
  EXPECT_EQ(relay.toJson(), R"({"tag":"single_host_addr","port":3001,"ipv4":"127.0.0.1","ipv6":null})");
}

TEST(SingleHostAddrRelay, ReplacesAndRemovesOptionalFields)
{
  // Arrange
  SingleHostAddrRelay       relay;
  const Ipv4                v4 = Ipv4::fromString("127.0.0.1");
  const Ipv6                v6 = Ipv6::fromString("0000:0000:0000:0000:0000:0000:0000:0001");
  const SingleHostAddrRelay empty(relay);

  // Act
  relay.setPort(65535);
  relay.setIpv4(v4);
  relay.setIpv6(v6);
  SingleHostAddrRelay copy(relay);
  relay.setIpv4(std::nullopt);
  relay.setIpv6(std::nullopt);
  relay.setPort(std::nullopt);

  // Assert
  EXPECT_EQ(relay, empty);
  EXPECT_EQ(copy.port(), 65535U);
  EXPECT_EQ(copy.ipv4(), v4);
  EXPECT_EQ(copy.ipv6(), v6);
  copy.setPort(0);
  EXPECT_EQ(copy.port(), 0U);
  EXPECT_NE(copy, relay);
}

TEST(SingleHostAddrRelay, ReturnsIndependentAddresses)
{
  // Arrange
  Ipv4                      address = Ipv4::fromString("127.0.0.1");
  const SingleHostAddrRelay relay(0, address);
  std::optional<Ipv4>       result = relay.ipv4();

  // Act
  address = Ipv4::fromString("192.0.2.1");
  result  = address;

  // Assert
  EXPECT_EQ(relay.ipv4()->toString(), "127.0.0.1");
  EXPECT_EQ(result->toString(), "192.0.2.1");
}

TEST(SingleHostAddrRelay, RejectsInvalidCborAndPortOverflow)
{
  // Act & Assert
  EXPECT_THROW(SingleHostAddrRelay::fromCbor("84001a00010000f6f6"), Error);
  EXPECT_THROW(SingleHostAddrRelay::fromCbor("8400f64100f6"), Error);
  EXPECT_THROW(SingleHostAddrRelay::fromCbor("8400f6f6"), Error);
  EXPECT_THROW(SingleHostAddrRelay::fromCbor("8400f6f6f600"), std::invalid_argument);
  EXPECT_THROW(SingleHostAddrRelay::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(SingleHostAddrRelay::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}
