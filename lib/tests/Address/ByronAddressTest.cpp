/**
 * @file ByronAddressTest.cpp
 *
 * @brief ByronAddress tests.
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

#include "../../src/CardanoInterop.h"

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ByronAddress, EncodesAndDecodesValues)
{
  // Arrange
  const Credential   credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const ByronAddress address = ByronAddress(credential.hash(), {}, ByronAddressType::PublicKey);

  // Act
  const ByronAddress bytes(address.toBytes());
  const ByronAddress text = ByronAddress::fromBase58(address.toString());

  // Assert
  EXPECT_EQ(bytes, address);
  EXPECT_EQ(text, address);
  EXPECT_FALSE(bytes != text);
}

TEST(ByronAddress, SupportsIndependentCopiesMovesAndAssignments)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  ByronAddress     address = ByronAddress(credential.hash(), {}, ByronAddressType::PublicKey);
  ByronAddress     copy(address);
  ByronAddress     assigned(copy);
  ByronAddress*    alias = &address;

  // Act
  assigned = address;
  address  = *alias;
  address  = std::move(*alias);
  ByronAddress moved(std::move(address));
  swap(moved, assigned);
  address = std::move(moved);

  // Assert
  EXPECT_EQ(address, copy);
  EXPECT_EQ(assigned, copy);
  EXPECT_THROW(moved.toBytes(), std::logic_error);
  EXPECT_THROW(ByronAddress invalid(moved), std::logic_error);
}

TEST(ByronAddress, RejectsInvalidBytesAndText)
{
  // Act & Assert
  EXPECT_THROW((void)ByronAddress(std::vector<std::uint8_t> {}), Error);
  EXPECT_THROW(ByronAddress::fromBase58("invalid"), std::invalid_argument);
}

TEST(ByronAddress, MatchesKnownCip19Encoding)
{
  // Arrange
  const Blake2bHash root = Blake2bHash::fromHex("ba970ad36654d8dd8f74274b733452ddeab9a62a397746be3c42ccdd");

  // Act
  const ByronAddress address(root, {}, ByronAddressType::PublicKey);

  // Assert
  EXPECT_EQ(address.toString(), "Ae2tdPwUPEZFRbyhz3cpfC2CumGzNkFBN2L42rcUc2yjQpEkxDbkPodpMAi");
  EXPECT_EQ(address.root(), root);
  EXPECT_EQ(address.type(), ByronAddressType::PublicKey);
  EXPECT_EQ(address.networkId(), NetworkId::Mainnet);
  EXPECT_FALSE(address.attributes().magic.has_value());
}

TEST(ByronAddress, PreservesOwnedAttributesAndRejectsInvalidComponents)
{
  // Arrange
  const Blake2bHash      root(std::vector<std::uint8_t>(28, 1));
  ByronAddressAttributes attributes;
  attributes.derivationPath = { 1, 2, 3 };
  attributes.magic          = 42;
  const ByronAddress address(root, attributes, ByronAddressType::Script);

  // Act
  attributes.derivationPath[0]    = 9;
  ByronAddressAttributes returned = address.attributes();
  returned.derivationPath[0]      = 8;
  const ByronAddress decoded(address.toBytes());

  // Assert
  EXPECT_EQ(returned.derivationPath.front(), 8);
  EXPECT_EQ(decoded.attributes().derivationPath, (std::vector<std::uint8_t> { 1, 2, 3 }));
  EXPECT_EQ(decoded.attributes().magic, 42);
  EXPECT_EQ(decoded.networkId(), NetworkId::Testnet);
  EXPECT_EQ(decoded.type(), ByronAddressType::Script);
  EXPECT_THROW((void)ByronAddress(Blake2bHash(std::vector<std::uint8_t>(27, 1)), {}, ByronAddressType::PublicKey), std::invalid_argument);
  EXPECT_THROW((void)ByronAddress(root, { std::vector<std::uint8_t>(65, 1), {} }, ByronAddressType::PublicKey), std::invalid_argument);
  EXPECT_THROW((void)ByronAddress(root, { {}, -2 }, ByronAddressType::PublicKey), std::invalid_argument);
  EXPECT_THROW((void)ByronAddress(root, {}, static_cast<ByronAddressType>(3)), std::invalid_argument);
}
