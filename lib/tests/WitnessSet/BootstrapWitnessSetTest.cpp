/**
 * @file BootstrapWitnessSetTest.cpp
 *
 * @brief Unit tests for bootstrap witness sets.
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

TEST(BootstrapWitnessSet, MatchesNativeCborAndSupportsSetTag)
{
  // Arrange
  const BootstrapWitness          witness(Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Ed25519Signature(std::vector<std::uint8_t>(64, 2)), std::vector<std::uint8_t>(32, 3), { 0xa0 });
  BootstrapWitnessSet             set;
  std::vector<std::uint8_t>       expected { 0xd9, 0x01, 0x02, 0x81 };
  const std::vector<std::uint8_t> witnessCbor = witness.toCbor();
  expected.insert(expected.end(), witnessCbor.begin(), witnessCbor.end());

  // Act
  set.add(witness);
  const BootstrapWitnessSet decoded = BootstrapWitnessSet::fromCbor(expected);

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

TEST(BootstrapWitnessSet, ReplacesSignatureForSameAddressRoot)
{
  // Arrange
  const Ed25519PublicKey          key(std::vector<std::uint8_t>(32, 1));
  const std::vector<std::uint8_t> chainCode(32, 3);
  const BootstrapWitness          original(key, Ed25519Signature(std::vector<std::uint8_t>(64, 2)), chainCode, { 0xa0 });
  const BootstrapWitness          replacement(key, Ed25519Signature(std::vector<std::uint8_t>(64, 4)), chainCode, { 0xa0 });
  BootstrapWitnessSet             set;
  set.add(original);
  const BootstrapWitnessSet copy(set);

  // Act
  set.add(replacement);

  // Assert
  EXPECT_EQ(set.size(), 1U);
  EXPECT_EQ(set.at(0), replacement);
  EXPECT_EQ(copy.at(0), original);
  EXPECT_THROW(set.at(1), std::out_of_range);
  EXPECT_EQ(BootstrapWitnessSet::fromCbor(set.toCbor()), set);
}
