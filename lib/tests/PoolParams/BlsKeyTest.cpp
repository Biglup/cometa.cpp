/**
 * @file BlsKeyTest.cpp
 *
 * @brief Unit tests for BlsKey.
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

/* INTERNAL IMPLEMENTATION ***************************************************/

static const char* PUBLIC_KEY_HEX       = "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f";
static const char* POSSESSION_PROOF_HEX = "606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f";
static const char* CBOR                 = "825860000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f5830606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f";
static const char* CBOR_ONE_ELEMENT     = "815860000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f";
static const char* CBOR_SHORT_KEY       = "82582f0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f5830606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f808182838485868788898a8b8c8d8e8f";
static const char* CBOR_SHORT_PROOF     = "825860000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d4e4f505152535455565758595a5b5c5d5e5f5820606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f";
static const char* CBOR_KEY_NOT_BYTES   = "820105";

/* IMPLEMENTATION ************************************************************/

TEST(BlsKey, MatchesNativeSerializationVector)
{
  // Arrange
  CborReader encoded(CBOR);

  // Act
  const BlsKey key = BlsKey::fromHex(PUBLIC_KEY_HEX, POSSESSION_PROOF_HEX);

  // Assert
  EXPECT_EQ(key.publicKey().size(), 96U);
  EXPECT_EQ(key.publicKey().front(), 0U);
  EXPECT_EQ(key.publicKey().back(), 95U);
  EXPECT_EQ(key.possessionProof().size(), 48U);
  EXPECT_EQ(key.possessionProof().front(), 96U);
  EXPECT_EQ(key.possessionProof().back(), 143U);
  EXPECT_EQ(key.toCbor(), encoded.readEncodedValue());
  EXPECT_EQ(BlsKey::fromCbor(CBOR), key);
  EXPECT_EQ(BlsKey::fromCbor(key.toCbor()), key);
  EXPECT_EQ(key.toJson(), std::string("{\"public_key\":\"") + PUBLIC_KEY_HEX + "\",\"possession_proof\":\"" + POSSESSION_PROOF_HEX + "\"}");
}

TEST(BlsKey, CopiesInputsAndReturnedBytes)
{
  // Arrange
  std::vector<std::uint8_t> publicKey(96, 1);
  std::vector<std::uint8_t> proof(48, 2);
  const BlsKey              key(publicKey, proof);

  // Act
  publicKey.at(0)                    = 3;
  proof.at(0)                        = 4;
  std::vector<std::uint8_t> returned = key.publicKey();
  returned.at(0)                     = 5;

  // Assert
  EXPECT_EQ(key.publicKey().at(0), 1U);
  EXPECT_EQ(key.possessionProof().at(0), 2U);
  EXPECT_NE(returned, key.publicKey());
  EXPECT_NE(BlsKey(publicKey, proof), key);
}

TEST(BlsKey, RejectsInvalidKeysAndEncodings)
{
  // Act & Assert
  EXPECT_THROW((void)BlsKey(std::vector<std::uint8_t>(95), std::vector<std::uint8_t>(48)), Error);
  EXPECT_THROW((void)BlsKey(std::vector<std::uint8_t>(96), std::vector<std::uint8_t>(47)), Error);
  EXPECT_THROW(BlsKey::fromHex("", POSSESSION_PROOF_HEX), Error);
  EXPECT_THROW(BlsKey::fromHex("z", POSSESSION_PROOF_HEX), std::invalid_argument);
  EXPECT_THROW(BlsKey::fromCbor(CBOR_ONE_ELEMENT), Error);
  EXPECT_THROW(BlsKey::fromCbor(CBOR_SHORT_KEY), Error);
  EXPECT_THROW(BlsKey::fromCbor(CBOR_SHORT_PROOF), Error);
  EXPECT_THROW(BlsKey::fromCbor(CBOR_KEY_NOT_BYTES), Error);
  EXPECT_THROW(BlsKey::fromCbor(std::string(CBOR) + "00"), std::invalid_argument);
  EXPECT_THROW(BlsKey::fromCbor("z"), std::invalid_argument);
  EXPECT_THROW(BlsKey::fromCbor(std::vector<std::uint8_t> {}), std::invalid_argument);
}
