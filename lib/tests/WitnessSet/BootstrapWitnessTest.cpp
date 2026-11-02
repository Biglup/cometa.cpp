/**
 * @file BootstrapWitnessTest.cpp
 *
 * @brief Unit tests for bootstrap witnesses.
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

TEST(BootstrapWitness, MatchesNativeCbor)
{
  // Arrange
  const Ed25519PublicKey          key(std::vector<std::uint8_t>(32, 1));
  const Ed25519Signature          signature(std::vector<std::uint8_t>(64, 2));
  const std::vector<std::uint8_t> chainCode(32, 3);
  const std::vector<std::uint8_t> attributes { 0xa0 };
  const BootstrapWitness          witness(key, signature, chainCode, attributes);
  std::vector<std::uint8_t>       expected { 0x84, 0x58, 0x20 };
  expected.insert(expected.end(), 32, 1);
  expected.push_back(0x58);
  expected.push_back(0x40);
  expected.insert(expected.end(), 64, 2);
  expected.push_back(0x58);
  expected.push_back(0x20);
  expected.insert(expected.end(), 32, 3);
  expected.push_back(0x41);
  expected.push_back(0xa0);

  // Act
  const BootstrapWitness decoded = BootstrapWitness::fromCbor(expected);

  // Assert
  EXPECT_EQ(witness.toCbor(), expected);
  EXPECT_EQ(decoded, witness);
  EXPECT_EQ(witness.publicKey(), key);
  EXPECT_EQ(witness.signature(), signature);
  EXPECT_EQ(witness.chainCode(), chainCode);
  EXPECT_EQ(witness.attributes(), attributes);
  EXPECT_FALSE(witness.toJson().empty());
}

TEST(BootstrapWitness, CopiesAndSetsFieldsIndependently)
{
  // Arrange
  BootstrapWitness       witness(Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Ed25519Signature(std::vector<std::uint8_t>(64, 2)), std::vector<std::uint8_t>(32, 3), { 0xa0 });
  const BootstrapWitness copy(witness);
  const Ed25519PublicKey key(std::vector<std::uint8_t>(32, 4));
  const Ed25519Signature signature(std::vector<std::uint8_t>(64, 5));

  // Act
  witness.setPublicKey(key);
  witness.setSignature(signature);
  witness.setChainCode(std::vector<std::uint8_t>(32, 6));
  witness.setAttributes({});

  // Assert
  EXPECT_EQ(copy.publicKey().toBytes().front(), 1U);
  EXPECT_EQ(copy.signature().toBytes().front(), 2U);
  EXPECT_EQ(copy.chainCode().front(), 3U);
  EXPECT_EQ(copy.attributes(), std::vector<std::uint8_t> { 0xa0 });
  EXPECT_EQ(witness.publicKey(), key);
  EXPECT_EQ(witness.signature(), signature);
  EXPECT_EQ(witness.chainCode(), std::vector<std::uint8_t>(32, 6));
  EXPECT_TRUE(witness.attributes().empty());
  EXPECT_NE(witness, copy);
  EXPECT_THROW(witness.setChainCode({ 1 }), std::invalid_argument);
  EXPECT_THROW(BootstrapWitness(key, signature, { 1 }, {}), std::invalid_argument);
  EXPECT_EQ(BootstrapWitness::fromCbor(witness.toCbor()), witness);
}

TEST(BootstrapWitness, AcceptsEmptyAttributes)
{
  // Arrange
  const BootstrapWitness witness(Ed25519PublicKey(std::vector<std::uint8_t>(32, 1)), Ed25519Signature(std::vector<std::uint8_t>(64, 2)), std::vector<std::uint8_t>(32, 3), {});

  // Act
  const BootstrapWitness decoded = BootstrapWitness::fromCbor(witness.toCbor());

  // Assert
  EXPECT_TRUE(witness.attributes().empty());
  EXPECT_EQ(decoded, witness);
}
