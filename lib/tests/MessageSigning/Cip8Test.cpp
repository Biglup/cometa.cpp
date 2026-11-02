/**
 * @file Cip8Test.cpp
 *
 * @brief CIP-8 message signing tests.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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

#include <algorithm>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Cip8, SignsMessageBoundToAddress)
{
  // Arrange
  const Ed25519PrivateKey         key = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 7));
  const Credential                credential(CredentialType::KeyHash, key.publicKey().hash());
  const Address                   address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  const std::vector<std::uint8_t> message { 'h', 'e', 'l', 'l', 'o' };

  // Act
  const Cip8Signature                signedMessage = signCip8(message, address, key);
  CborReader                         reader(signedMessage.coseSign1);
  const std::optional<std::uint64_t> itemCount       = reader.readStartArray();
  const std::vector<std::uint8_t>    protectedHeader = reader.readBytes();

  reader.skipValue();

  const std::vector<std::uint8_t> payload      = reader.readBytes();
  const std::vector<std::uint8_t> signature    = reader.readBytes();
  const std::vector<std::uint8_t> addressBytes = address.toBytes();

  reader.readEndArray();

  CborWriter signedPayload;
  signedPayload.writeStartArray(4);
  signedPayload.writeText("Signature1");
  signedPayload.writeBytes(protectedHeader);
  signedPayload.writeBytes({});
  signedPayload.writeBytes(message);

  // Assert
  EXPECT_EQ(itemCount, 4U);
  EXPECT_EQ(payload, message);
  EXPECT_EQ(signature.size(), 64U);
  EXPECT_TRUE(key.publicKey().verify(Ed25519Signature(signature), signedPayload.toBytes()));
  EXPECT_EQ(reader.bytesRemaining(), 0U);
  EXPECT_FALSE(signedMessage.coseKey.empty());
  EXPECT_NE(std::search(protectedHeader.begin(), protectedHeader.end(), addressBytes.begin(), addressBytes.end()), protectedHeader.end());
}

TEST(Cip8, SignsMessageBoundToKeyHash)
{
  // Arrange
  const Ed25519PrivateKey         key  = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 7));
  const Blake2bHash               hash = key.publicKey().hash();
  const std::vector<std::uint8_t> message { 1, 2, 3 };

  // Act
  const Cip8Signature                signedMessage = signCip8(message, hash, key);
  CborReader                         reader(signedMessage.coseSign1);
  const std::optional<std::uint64_t> itemCount       = reader.readStartArray();
  const std::vector<std::uint8_t>    protectedHeader = reader.readBytes();
  const std::vector<std::uint8_t>    hashBytes       = hash.toBytes();

  // Assert
  EXPECT_EQ(itemCount, 4U);
  EXPECT_FALSE(signedMessage.coseKey.empty());
  EXPECT_NE(std::search(protectedHeader.begin(), protectedHeader.end(), hashBytes.begin(), hashBytes.end()), protectedHeader.end());
}
