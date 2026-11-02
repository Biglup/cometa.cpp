/**
 * @file PointerAddressTest.cpp
 *
 * @brief PointerAddress tests.
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

TEST(PointerAddress, EncodesAndDecodesValues)
{
  // Arrange
  const Credential     credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const PointerAddress address = PointerAddress(NetworkId::Testnet, credential, { 42, 1, 2 });

  // Act
  const PointerAddress bytes(address.toBytes());
  const PointerAddress text = PointerAddress::fromBech32(address.toString());

  // Assert
  EXPECT_EQ(bytes, address);
  EXPECT_EQ(text, address);
  EXPECT_FALSE(bytes != text);
}

TEST(PointerAddress, SupportsIndependentCopiesMovesAndAssignments)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  PointerAddress   address = PointerAddress(NetworkId::Testnet, credential, { 42, 1, 2 });
  PointerAddress   copy(address);
  PointerAddress   assigned(copy);
  PointerAddress*  alias = &address;

  // Act
  assigned = address;
  address  = *alias;
  address  = std::move(*alias);
  PointerAddress moved(std::move(address));
  swap(moved, assigned);
  address = std::move(moved);

  // Assert
  EXPECT_EQ(address, copy);
  EXPECT_EQ(assigned, copy);
  EXPECT_THROW(moved.toBytes(), std::logic_error);
  EXPECT_THROW(PointerAddress invalid(moved), std::logic_error);
}

TEST(PointerAddress, RejectsInvalidBytesAndText)
{
  // Act & Assert
  EXPECT_THROW((void)PointerAddress(std::vector<std::uint8_t> {}), Error);
  EXPECT_THROW(PointerAddress::fromBech32("invalid"), std::invalid_argument);
}

TEST(PointerAddress, CredentialsAreIndependentFromInputsAndReturnedValues)
{
  // Arrange
  Credential           credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Credential     original(credential);
  const PointerAddress address = PointerAddress(NetworkId::Testnet, credential, { 42, 1, 2 });
  const std::string    before  = address.toString();

  // Act
  credential.setType(CredentialType::ScriptHash);
  Credential returned = address.paymentCredential();
  returned.setHash(Blake2bHash(std::vector<std::uint8_t>(28, 2)));

  // Assert
  EXPECT_EQ(address.paymentCredential(), original);
  EXPECT_EQ(address.toString(), before);
  EXPECT_EQ(address.toAddress().networkId(), NetworkId::Testnet);
}

TEST(PointerAddress, PreservesMaximumSupportedFieldsAndRejectsOverflow)
{
  // Arrange
  const Credential   credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const StakePointer pointer { 0xffffffffULL, 0xffffffffULL, 0xffffffffULL };

  // Act
  const PointerAddress address(NetworkId::Testnet, credential, pointer);
  const StakePointer   decoded = PointerAddress(address.toBytes()).stakePointer();

  // Assert
  EXPECT_EQ(decoded.slot, pointer.slot);
  EXPECT_EQ(decoded.transactionIndex, pointer.transactionIndex);
  EXPECT_EQ(decoded.certificateIndex, pointer.certificateIndex);
  EXPECT_THROW((void)PointerAddress(NetworkId::Testnet, credential, { 0x100000000ULL, 0, 0 }), std::invalid_argument);
  EXPECT_THROW((void)PointerAddress(NetworkId::Testnet, credential, { 0, 0x100000000ULL, 0 }), std::invalid_argument);
  EXPECT_THROW((void)PointerAddress(NetworkId::Testnet, credential, { 0, 0, 0x100000000ULL }), std::invalid_argument);
}

TEST(PointerAddress, MatchesKnownCip19Encoding)
{
  // Arrange
  const std::string encoded = "addr1gx2fxv2umyhttkxyxp8x0dlpdt3k6cwng5pxj3jhsydzer5pnz75xxcrzqf96k";

  // Act
  const PointerAddress address = PointerAddress::fromBech32(encoded);

  // Assert
  EXPECT_EQ(address.toString(), encoded);
  EXPECT_EQ(address.toAddress().type(), AddressType::PointerKey);
}
