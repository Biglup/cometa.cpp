/**
 * @file VkeyWitnessSetTest.cpp
 *
 * @brief Unit tests for verification-key witness sets.
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

TEST(VkeyWitnessSet, MatchesNativeCborAndSupportsSetTag)
{
  // Arrange
  const VkeyWitness               witness(Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Ed25519Signature(std::vector<std::uint8_t>(64, 2)));
  VkeyWitnessSet                  set;
  std::vector<std::uint8_t>       expected { 0xd9, 0x01, 0x02, 0x81 };
  const std::vector<std::uint8_t> witnessCbor = witness.toCbor();
  expected.insert(expected.end(), witnessCbor.begin(), witnessCbor.end());

  // Act
  set.add(witness);
  const VkeyWitnessSet decoded = VkeyWitnessSet::fromCbor(expected);

  // Assert
  EXPECT_EQ(set.size(), 1U);
  EXPECT_EQ(set.toCbor(), expected);
  EXPECT_EQ(decoded, set);
  EXPECT_EQ(set.at(0), witness);
  EXPECT_TRUE(set.useTag());
  EXPECT_FALSE(set.toJson().empty());

  // Act
  set.setUseTag(false);

  // Assert
  EXPECT_FALSE(set.useTag());
  EXPECT_EQ(set.toCbor().front(), 0x81);
}

TEST(VkeyWitnessSet, ReplacesMatchingKeyAndAppliesIndependently)
{
  // Arrange
  const Ed25519PublicKey key(std::vector<std::uint8_t>(32, 1));
  const VkeyWitness      original(key, Ed25519Signature(std::vector<std::uint8_t>(64, 2)));
  const VkeyWitness      replacement(key, Ed25519Signature(std::vector<std::uint8_t>(64, 3)));
  VkeyWitnessSet         set;
  set.add(original);
  const VkeyWitnessSet copy(set);
  VkeyWitnessSet       updates;
  updates.add(replacement);

  // Act
  set.apply(updates);

  // Assert
  EXPECT_EQ(set.size(), 1U);
  EXPECT_EQ(set.at(0), replacement);
  EXPECT_EQ(copy.at(0), original);
  EXPECT_EQ(updates.at(0), replacement);
  EXPECT_THROW(set.at(1), std::out_of_range);
  EXPECT_EQ(VkeyWitnessSet::fromCbor(set.toCbor()), set);
}

TEST(VkeyWitnessSet, StandardInsertRetainsExistingSignature)
{
  // Arrange
  const Ed25519PublicKey key(std::vector<std::uint8_t>(32, 1));
  const VkeyWitness      first(key, Ed25519Signature(std::vector<std::uint8_t>(64, 2)));
  const VkeyWitness      replacement(key, Ed25519Signature(std::vector<std::uint8_t>(64, 3)));
  VkeyWitnessSet         witnesses;
  witnesses.add(first);

  // Act
  const std::pair<VkeyWitnessSet::iterator, bool> result = witnesses.insert(replacement);

  // Assert
  EXPECT_FALSE(result.second);
  EXPECT_EQ(*result.first, first);
  EXPECT_EQ(witnesses.size(), 1U);

  // Act
  witnesses.add(replacement);

  // Assert
  EXPECT_EQ(*witnesses.begin(), replacement);
}
