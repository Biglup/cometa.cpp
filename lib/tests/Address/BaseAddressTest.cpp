/**
 * @file BaseAddressTest.cpp
 *
 * @brief BaseAddress tests.
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

TEST(BaseAddress, EncodesAndDecodesValues)
{
  // Arrange
  const Credential  credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const BaseAddress address = BaseAddress(NetworkId::Testnet, credential, credential);

  // Act
  const BaseAddress bytes(address.toBytes());
  const BaseAddress text = BaseAddress::fromBech32(address.toString());

  // Assert
  EXPECT_EQ(bytes, address);
  EXPECT_EQ(text, address);
  EXPECT_FALSE(bytes != text);
}

TEST(BaseAddress, SupportsIndependentCopiesMovesAndAssignments)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  BaseAddress      address = BaseAddress(NetworkId::Testnet, credential, credential);
  BaseAddress      copy(address);
  BaseAddress      assigned(copy);
  BaseAddress*     alias = &address;

  // Act
  assigned = address;
  address  = *alias;
  address  = std::move(*alias);
  BaseAddress moved(std::move(address));
  swap(moved, assigned);
  address = std::move(moved);

  // Assert
  EXPECT_EQ(address, copy);
  EXPECT_EQ(assigned, copy);
  EXPECT_THROW(moved.toBytes(), std::logic_error);
  EXPECT_THROW(BaseAddress invalid(moved), std::logic_error);
}

TEST(BaseAddress, RejectsInvalidBytesAndText)
{
  // Act & Assert
  EXPECT_THROW((void)BaseAddress(std::vector<std::uint8_t> {}), Error);
  EXPECT_THROW(BaseAddress::fromBech32("invalid"), std::invalid_argument);
}

TEST(BaseAddress, CredentialsAreIndependentFromInputsAndReturnedValues)
{
  // Arrange
  Credential        credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Credential  original(credential);
  const BaseAddress address = BaseAddress(NetworkId::Testnet, credential, credential);
  const std::string before  = address.toString();

  // Act
  credential.setType(CredentialType::ScriptHash);
  Credential returned = address.paymentCredential();
  returned.setHash(Blake2bHash(std::vector<std::uint8_t>(28, 2)));

  // Assert
  EXPECT_EQ(address.paymentCredential(), original);
  EXPECT_EQ(address.toString(), before);
  EXPECT_EQ(address.toAddress().networkId(), NetworkId::Testnet);
}

TEST(BaseAddress, ReturnsIndependentStakeCredentialAndRejectsWrongType)
{
  // Arrange
  const Credential        credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const BaseAddress       address(NetworkId::Mainnet, credential, credential);
  const EnterpriseAddress enterprise(NetworkId::Mainnet, credential);

  // Act
  Credential stake = address.stakeCredential();
  stake.setType(CredentialType::ScriptHash);

  // Assert
  EXPECT_EQ(address.stakeCredential(), credential);
  EXPECT_THROW(BaseAddress::fromAddress(enterprise.toAddress()), Error);
  EXPECT_THROW((void)BaseAddress(enterprise.toBytes()), Error);
  EXPECT_THROW((void)BaseAddress(static_cast<NetworkId>(16), credential, credential), std::invalid_argument);
}
