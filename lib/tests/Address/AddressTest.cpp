/**
 * @file AddressTest.cpp
 *
 * @brief Address tests.
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

#include <algorithm>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/**
 * @brief Converts an ASCII letter to uppercase for address case tests.
 * @param character The character to convert.
 * @return The uppercase letter or original nonletter.
 */
char
toUpperAscii(char character)
{
  return character >= 'a' && character <= 'z' ? static_cast<char>(character - ('a' - 'A')) : character;
}
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(Address, EncodesAndDecodesValues)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address    address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();

  // Act
  const Address bytes(address.toBytes());
  const Address text = Address::fromString(address.toString());

  // Assert
  EXPECT_EQ(bytes, address);
  EXPECT_EQ(text, address);
  EXPECT_FALSE(bytes != text);
}

TEST(Address, SupportsIndependentCopiesMovesAndAssignments)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  Address          address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  Address          copy(address);
  Address          assigned(copy);
  Address*         alias = &address;

  // Act
  assigned = address;
  address  = *alias;
  address  = std::move(*alias);
  Address moved(std::move(address));
  swap(moved, assigned);
  address = std::move(moved);

  // Assert
  EXPECT_EQ(address, copy);
  EXPECT_EQ(assigned, copy);
  EXPECT_THROW(moved.toBytes(), std::logic_error);
  EXPECT_THROW(Address invalid(moved), std::logic_error);
}

TEST(Address, RejectsInvalidBytesAndText)
{
  // Act & Assert
  EXPECT_THROW((void)Address(std::vector<std::uint8_t> {}), Error);
  EXPECT_THROW(Address::fromString("invalid"), std::invalid_argument);
}

TEST(Address, ConvertsEveryAddressVariantAndReturnsNulloptForWrongTypes)
{
  // Arrange
  const Credential        credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const BaseAddress       base(NetworkId::Testnet, credential, credential);
  const EnterpriseAddress enterprise(NetworkId::Testnet, credential);
  const PointerAddress    pointer(NetworkId::Testnet, credential, { 1, 2, 3 });
  const RewardAddress     reward(NetworkId::Testnet, credential);
  const ByronAddress      byron(credential.hash(), {}, ByronAddressType::Redeem);

  // Act & Assert
  EXPECT_EQ(base.toAddress().asBase(), base);
  EXPECT_EQ(enterprise.toAddress().asEnterprise(), enterprise);
  EXPECT_EQ(pointer.toAddress().asPointer(), pointer);
  EXPECT_EQ(reward.toAddress().asReward(), reward);
  EXPECT_EQ(byron.toAddress().asByron(), byron);
  EXPECT_FALSE(enterprise.toAddress().asBase());
  EXPECT_FALSE(base.toAddress().asEnterprise());
  EXPECT_FALSE(base.toAddress().asPointer());
  EXPECT_FALSE(base.toAddress().asReward());
  EXPECT_FALSE(base.toAddress().asByron());
}

TEST(Address, ValidatesPrefixesCaseAndStringViewBounds)
{
  // Arrange
  const Credential  credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  const Address     address = EnterpriseAddress(NetworkId::Testnet, credential).toAddress();
  const std::string text    = address.toString();
  const std::string padded  = text + "suffix";
  std::string       upper   = text;

  std::transform(upper.begin(), upper.end(), upper.begin(), toUpperAscii);

  // Act & Assert
  EXPECT_EQ(Address::fromString(std::string_view(padded.data(), text.size())), address);
  EXPECT_EQ(Address::fromString(upper), address);
  EXPECT_TRUE(Address::isValid(text));
  EXPECT_TRUE(Address::isValidBech32(text));
  EXPECT_FALSE(Address::isValidByron(text));
  EXPECT_FALSE(Address::isValid(""));
  EXPECT_FALSE(Address::isValid("invalid"));
  EXPECT_THROW(Address::fromString(encodeBech32("addr", address.toBytes())), std::invalid_argument);
  EXPECT_THROW(Address::fromString(encodeBech32("stake_test", address.toBytes())), std::invalid_argument);
  EXPECT_THROW(Address::fromString(encodeBase58(address.toBytes())), std::invalid_argument);

  upper[0] = 'a';
  EXPECT_FALSE(Address::isValid(upper));
}

TEST(Address, RejectsTrailingAndOverflowingPointerBytes)
{
  // Arrange
  const Credential          credential(CredentialType::KeyHash, Blake2bHash(std::vector<std::uint8_t>(28, 1)));
  std::vector<std::uint8_t> bytes = EnterpriseAddress(NetworkId::Testnet, credential).toBytes();
  bytes.push_back(0);
  std::vector<std::uint8_t> pointer(29, 0);
  pointer[0] = 0x40;
  const std::vector<std::uint8_t> indices { 0x90, 0x80, 0x80, 0x80, 0, 0, 0 };
  pointer.insert(pointer.end(), indices.begin(), indices.end());

  // Act & Assert
  EXPECT_THROW((void)Address(bytes), std::invalid_argument);
  EXPECT_THROW((void)Address(pointer), std::invalid_argument);
}
