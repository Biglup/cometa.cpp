/**
 * @file SingleHostNameRelayTest.cpp
 *
 * @brief Unit tests for SingleHostNameRelay.
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

TEST(SingleHostNameRelay, MatchesKnownCborAndInspection)
{
  // Arrange
  CborReader expected("8301190bb96b6578616d706c652e636f6d");

  // Act
  const SingleHostNameRelay relay("example.com", 3001);

  // Assert
  EXPECT_EQ(relay.dns(), "example.com");
  EXPECT_EQ(relay.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(SingleHostNameRelay::fromCbor("8301190bb96b6578616d706c652e636f6d"), relay);
  EXPECT_EQ(SingleHostNameRelay::fromCbor(relay.toCbor()), relay);
  EXPECT_EQ(relay.toJson(), R"({"tag":"single_host_name","port":3001,"dns_name":"example.com"})");
}

TEST(SingleHostNameRelay, ReplacesShorterNamesWithoutChangingCopies)
{
  // Arrange
  SingleHostNameRelay       relay(std::string(64, 'x'));
  const SingleHostNameRelay original(relay);

  // Act
  relay.setDns("a.b");

  // Assert
  EXPECT_EQ(relay.dns(), "a.b");
  EXPECT_EQ(original.dns(), std::string(64, 'x'));
  EXPECT_EQ(SingleHostNameRelay::fromCbor(relay.toCbor()).dns(), "a.b");
}

TEST(SingleHostNameRelay, RejectsInvalidNamesAndCbor)
{
  // Arrange
  SingleHostNameRelay       relay("example.com");
  const SingleHostNameRelay original(relay);
  const std::string         embedded("a\0b", 3);

  // Act & Assert
  EXPECT_THROW((void)SingleHostNameRelay(""), std::invalid_argument);
  EXPECT_THROW((void)SingleHostNameRelay(std::string(65, 'x')), std::invalid_argument);
  EXPECT_THROW((void)SingleHostNameRelay(embedded), std::invalid_argument);
  EXPECT_THROW(relay.setDns(""), std::invalid_argument);
  EXPECT_THROW(relay.setDns(std::string(65, 'x')), std::invalid_argument);
  EXPECT_THROW(relay.setDns(embedded), std::invalid_argument);
  EXPECT_EQ(relay, original);
  EXPECT_THROW(SingleHostNameRelay::fromCbor("80"), Error);
  EXPECT_THROW(SingleHostNameRelay::fromCbor("8301190bb96b6578616d706c652e636f6d00"), std::invalid_argument);
  EXPECT_THROW(SingleHostNameRelay::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(SingleHostNameRelay::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}

TEST(SingleHostNameRelay, DistinguishesZeroAndAbsentPort)
{
  // Arrange
  SingleHostNameRelay relay("example.com");
  EXPECT_FALSE(relay.port());

  // Act
  relay.setPort(0);
  const SingleHostNameRelay withZero(relay);
  relay.setPort(65535);
  EXPECT_EQ(relay.port(), 65535U);
  relay.setPort(std::nullopt);

  // Assert
  EXPECT_FALSE(relay.port());
  EXPECT_EQ(withZero.port(), 0U);
  EXPECT_NE(relay, withZero);
  EXPECT_THROW(SingleHostNameRelay::fromCbor("83011a000100006161"), Error);
}
