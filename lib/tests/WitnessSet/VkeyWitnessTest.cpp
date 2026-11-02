/**
 * @file VkeyWitnessTest.cpp
 *
 * @brief Unit tests for verification-key witnesses.
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

TEST(VkeyWitness, MatchesNativeCbor)
{
  // Arrange
  const Ed25519PublicKey    key(std::vector<std::uint8_t>(32, 1));
  const Ed25519Signature    signature(std::vector<std::uint8_t>(64, 2));
  const VkeyWitness         witness(key, signature);
  std::vector<std::uint8_t> expected { 0x82, 0x58, 0x20 };
  expected.insert(expected.end(), 32, 1);
  expected.push_back(0x58);
  expected.push_back(0x40);
  expected.insert(expected.end(), 64, 2);

  // Act
  const VkeyWitness decoded = VkeyWitness::fromCbor(expected);

  // Assert
  EXPECT_EQ(witness.toCbor(), expected);
  EXPECT_EQ(decoded, witness);
  EXPECT_EQ(witness.publicKey(), key);
  EXPECT_EQ(witness.signature(), signature);
  EXPECT_TRUE(witness.hasPublicKey(key));
  EXPECT_FALSE(witness.toJson().empty());
}

TEST(VkeyWitness, CopiesAndSetsFieldsIndependently)
{
  // Arrange
  VkeyWitness            witness(Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Ed25519Signature(std::vector<std::uint8_t>(64, 2)));
  const VkeyWitness      copy(witness);
  const Ed25519PublicKey replacementKey(std::vector<std::uint8_t>(32, 3));
  const Ed25519Signature replacementSignature(std::vector<std::uint8_t>(64, 4));

  // Act
  witness.setPublicKey(replacementKey);
  witness.setSignature(replacementSignature);

  // Assert
  EXPECT_EQ(copy.publicKey().toBytes().front(), 1U);
  EXPECT_EQ(copy.signature().toBytes().front(), 2U);
  EXPECT_EQ(witness.publicKey(), replacementKey);
  EXPECT_EQ(witness.signature(), replacementSignature);
  EXPECT_TRUE(witness.hasPublicKey(replacementKey));
  EXPECT_NE(witness, copy);
  EXPECT_EQ(VkeyWitness::fromCbor(witness.toCbor()), witness);
}
