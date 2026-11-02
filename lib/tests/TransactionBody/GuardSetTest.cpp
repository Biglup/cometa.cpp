/**
 * @file GuardSetTest.cpp
 *
 * @brief Unit tests for ordered Dijkstra transaction guards.
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

TEST(GuardSet, PreservesKeyHashFormAndSetTag)
{
  // Arrange
  const Credential key = Credential::fromCbor("8200581c6199186adb51974690d7247d2646097d2c62763b16fb7ed3f9f55d39");
  GuardSet         guards;
  CborReader       reader("d9010281581c6199186adb51974690d7247d2646097d2c62763b16fb7ed3f9f55d39");

  // Act
  guards.add(key);
  const GuardSet decoded = GuardSet::fromCbor(guards.toCbor());

  // Assert
  EXPECT_EQ(guards.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, guards);
  EXPECT_EQ(guards.at(0), key);
  EXPECT_TRUE(guards.useTag());
  EXPECT_FALSE(guards.toJson().empty());

  // Act
  guards.setUseTag(false);

  // Assert
  EXPECT_FALSE(guards.useTag());
  EXPECT_EQ(guards.toCbor().front(), 0x81);
}

TEST(GuardSet, PreservesCredentialOrderAndRejectsDuplicates)
{
  // Arrange
  const Credential key    = Credential::fromCbor("8200581c6199186adb51974690d7247d2646097d2c62763b16fb7ed3f9f55d39");
  const Credential script = Credential::fromCbor("8201581c966e394a544f242081e41d1965137b1bb412ac230d40ed5407821c37");
  GuardSet         guards;
  guards.add(script);
  const GuardSet copy(guards);

  // Act
  guards.add(key);

  // Assert
  EXPECT_EQ(copy.size(), 1U);
  EXPECT_EQ(guards.size(), 2U);
  EXPECT_EQ(guards.at(0), script);
  EXPECT_EQ(guards.at(1), key);
  EXPECT_EQ(GuardSet::fromCbor(guards.toCbor()), guards);
  EXPECT_THROW(guards.add(script), Error);
  EXPECT_THROW(guards.at(2), std::out_of_range);
  EXPECT_NE(guards.toCbor(), GuardSet::fromCbor("d90102828200581c6199186adb51974690d7247d2646097d2c62763b16fb7ed3f9f55d398201581c966e394a544f242081e41d1965137b1bb412ac230d40ed5407821c37").toCbor());
}
