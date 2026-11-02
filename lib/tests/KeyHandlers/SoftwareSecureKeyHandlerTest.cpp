/**
 * @file SoftwareSecureKeyHandlerTest.cpp
 *
 * @brief Encrypted software signer tests.
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

/* USINGS ********************************************************************/

using namespace Cometa;

/* STATIC FUNCTIONS **********************************************************/

static Transaction
sampleTransaction()
{
  return Transaction(TransactionBody(TransactionInputSet(), TransactionOutputList(), 10), WitnessSet());
}

static SubTransaction
sampleSubTransaction()
{
  return SubTransaction(SubTransactionBody(TransactionInputSet(), TransactionOutputList()), WitnessSet());
}

static std::vector<std::uint8_t>
passphrase()
{
  return { 's', 'e', 'c', 'r', 'e', 't' };
}

static std::vector<std::uint8_t>
wrongPassphrase()
{
  return { 'w', 'r', 'o', 'n', 'g' };
}

/* IMPLEMENTATION ************************************************************/

TEST(SoftwareSecureKeyHandler, SignsAndRestoresBip32Keys)
{
  // Arrange
  const std::vector<std::uint8_t> entropy(16, 0);
  const SecureKeyHandler          signer = SecureKeyHandler::softwareBip32(entropy, passphrase(), &passphrase);
  const DerivationPath            path { 1852, 1815, 0, DerivationRole::External, 0 };
  const SecureKeyHandler          restored = SecureKeyHandler::deserializeSoftware(signer.serialize(), &passphrase);

  // Act
  const VkeyWitnessSet witnesses    = restored.signTransaction(sampleTransaction(), { path });
  const VkeyWitnessSet subWitnesses = restored.signSubTransaction(sampleSubTransaction(), { path });
  const Bip32PublicKey publicKey    = restored.accountPublicKey({ 1852, 1815, 0 });

  // Assert
  EXPECT_EQ(restored.type(), SecureKeyHandlerType::Bip32);
  EXPECT_EQ(witnesses.size(), 1U);
  EXPECT_EQ(subWitnesses.size(), 1U);
  EXPECT_EQ(publicKey, Bip32PrivateKey::fromBip39Entropy(entropy).derive({ hardenIndex(1852), hardenIndex(1815), hardenIndex(0) }).publicKey());
}

TEST(SoftwareSecureKeyHandler, SignsAndRestoresEd25519Keys)
{
  // Arrange
  const Ed25519PrivateKey key    = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 7));
  const SecureKeyHandler  signer = SecureKeyHandler::softwareEd25519(key, passphrase(), &passphrase);
  const SecureKeyHandler  copy(signer);
  const SecureKeyHandler  restored = SecureKeyHandler::deserializeSoftware(copy.serialize(), &passphrase);

  // Act
  const VkeyWitnessSet witnesses    = restored.signTransaction(sampleTransaction());
  const VkeyWitnessSet subWitnesses = restored.signSubTransaction(sampleSubTransaction());

  // Assert
  EXPECT_EQ(restored.type(), SecureKeyHandlerType::Ed25519);
  EXPECT_EQ(witnesses.size(), 1U);
  EXPECT_EQ(subWitnesses.size(), 1U);
  EXPECT_EQ(restored.publicKey(), key.publicKey());
}

TEST(SoftwareSecureKeyHandler, WrongPassphraseFailsWithoutCrossingHandlers)
{
  // Arrange
  const Ed25519PrivateKey key     = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 7));
  const SecureKeyHandler  valid   = SecureKeyHandler::softwareEd25519(key, passphrase(), &passphrase);
  const SecureKeyHandler  invalid = SecureKeyHandler::deserializeSoftware(valid.serialize(), &wrongPassphrase);

  // Act and assert
  EXPECT_THROW(invalid.signTransaction(sampleTransaction()), Error);
  EXPECT_EQ(valid.signTransaction(sampleTransaction()).size(), 1U);
  EXPECT_THROW(SecureKeyHandler::softwareBip32(std::vector<std::uint8_t>(16, 0), passphrase(), {}), std::invalid_argument);
}
