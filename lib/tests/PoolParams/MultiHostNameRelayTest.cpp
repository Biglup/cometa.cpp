/**
 * @file MultiHostNameRelayTest.cpp
 *
 * @brief Unit tests for MultiHostNameRelay.
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

TEST(MultiHostNameRelay, MatchesKnownCborAndInspection)
{
  // Arrange
  CborReader expected("82026b6578616d706c652e636f6d");

  // Act
  const MultiHostNameRelay relay("example.com");

  // Assert
  EXPECT_EQ(relay.dns(), "example.com");
  EXPECT_EQ(relay.toCbor(), expected.readEncodedValue());
  EXPECT_EQ(MultiHostNameRelay::fromCbor("82026b6578616d706c652e636f6d"), relay);
  EXPECT_EQ(MultiHostNameRelay::fromCbor(relay.toCbor()), relay);
  EXPECT_EQ(relay.toJson(), R"({"tag":"multi_host_name","dns_name":"example.com"})");
}

TEST(MultiHostNameRelay, ReplacesShorterNamesWithoutChangingCopies)
{
  // Arrange
  MultiHostNameRelay       relay(std::string(64, 'x'));
  const MultiHostNameRelay original(relay);

  // Act
  relay.setDns("a.b");

  // Assert
  EXPECT_EQ(relay.dns(), "a.b");
  EXPECT_EQ(original.dns(), std::string(64, 'x'));
  EXPECT_EQ(MultiHostNameRelay::fromCbor(relay.toCbor()).dns(), "a.b");
}

TEST(MultiHostNameRelay, RejectsInvalidNamesAndCbor)
{
  // Arrange
  MultiHostNameRelay       relay("example.com");
  const MultiHostNameRelay original(relay);
  const std::string        embedded("a\0b", 3);

  // Act & Assert
  EXPECT_THROW((void)MultiHostNameRelay(""), std::invalid_argument);
  EXPECT_THROW((void)MultiHostNameRelay(std::string(65, 'x')), std::invalid_argument);
  EXPECT_THROW((void)MultiHostNameRelay(embedded), std::invalid_argument);
  EXPECT_THROW(relay.setDns(""), std::invalid_argument);
  EXPECT_THROW(relay.setDns(std::string(65, 'x')), std::invalid_argument);
  EXPECT_THROW(relay.setDns(embedded), std::invalid_argument);
  EXPECT_EQ(relay, original);
  EXPECT_THROW(MultiHostNameRelay::fromCbor("80"), Error);
  EXPECT_THROW(MultiHostNameRelay::fromCbor("82026b6578616d706c652e636f6d00"), std::invalid_argument);
  EXPECT_THROW(MultiHostNameRelay::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(MultiHostNameRelay::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}
