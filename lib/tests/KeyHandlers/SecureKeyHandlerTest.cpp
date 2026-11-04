/**
 * @file SecureKeyHandlerTest.cpp
 *
 * @brief Unit tests for injected secure signing operations.
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

static VkeyWitnessSet
signBip32Transaction(const Transaction& transaction, const std::vector<DerivationPath>& paths)
{
  if (transaction.body().fee() != 10 || paths.size() != 1 || paths.front().role != DerivationRole::DRep)
  {
    throw std::invalid_argument("Unexpected BIP32 signing request");
  }

  return VkeyWitnessSet();
}

static VkeyWitnessSet
signBip32SubTransaction(const SubTransaction& transaction, const std::vector<DerivationPath>& paths)
{
  if (transaction.body().inputs().size() != 0 || paths.size() != 1)
  {
    throw std::invalid_argument("Unexpected BIP32 sub-transaction request");
  }

  return VkeyWitnessSet();
}

static Bip32PublicKey
accountPublicKey(const AccountDerivationPath& path)
{
  if (path.account != 2)
  {
    throw std::invalid_argument("Unexpected account path");
  }

  const Bip32PrivateKey root = Bip32PrivateKey::fromBip39Entropy(std::vector<std::uint8_t>(16, 0));
  return root.publicKey();
}

static VkeyWitnessSet
signEd25519Transaction(const Transaction& transaction)
{
  if (transaction.body().fee() != 10)
  {
    throw std::invalid_argument("Unexpected Ed25519 signing request");
  }

  return VkeyWitnessSet();
}

static VkeyWitnessSet
signEd25519SubTransaction(const SubTransaction& transaction)
{
  if (transaction.body().outputs().size() != 0)
  {
    throw std::invalid_argument("Unexpected Ed25519 sub-transaction request");
  }

  return VkeyWitnessSet();
}

static Ed25519PublicKey
ed25519PublicKey()
{
  const Ed25519PrivateKey key = Ed25519PrivateKey::fromNormalBytes(std::vector<std::uint8_t>(32, 7));
  return key.publicKey();
}

static std::vector<std::uint8_t>
serializedHandler()
{
  return { 1, 2, 3 };
}

static VkeyWitnessSet
failSigning(const Transaction&)
{
  throw std::runtime_error("Signer rejected transaction");
}

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

/* IMPLEMENTATION ************************************************************/

TEST(SecureKeyHandler, InjectedBip32CallbacksReceiveTypedPaths)
{
  // Arrange
  Bip32KeyCallbacks callbacks;
  callbacks.signTransaction      = &signBip32Transaction;
  callbacks.signSubTransaction   = &signBip32SubTransaction;
  callbacks.accountPublicKey     = &accountPublicKey;
  callbacks.serialize            = &serializedHandler;
  const SecureKeyHandler handler = SecureKeyHandler::bip32("test-bip32", callbacks);
  const DerivationPath   path { 1852, 1815, 2, DerivationRole::DRep, 0 };

  // Act
  const VkeyWitnessSet transactionWitnesses    = handler.signTransaction(sampleTransaction(), { path });
  const VkeyWitnessSet subTransactionWitnesses = handler.signSubTransaction(sampleSubTransaction(), { path });

  // Assert
  EXPECT_EQ(handler.name(), "test-bip32");
  EXPECT_EQ(handler.type(), SecureKeyHandlerType::Bip32);
  EXPECT_TRUE(transactionWitnesses.size() == 0);
  EXPECT_TRUE(subTransactionWitnesses.size() == 0);
  EXPECT_EQ(handler.accountPublicKey({ 1852, 1815, 2 }), accountPublicKey({ 1852, 1815, 2 }));
  EXPECT_EQ(handler.serialize(), (std::vector<std::uint8_t> { 1, 2, 3 }));
  EXPECT_THROW(handler.signTransaction(sampleTransaction(), {}), std::invalid_argument);
}

TEST(SecureKeyHandler, InjectedEd25519CallbacksAndCopiesRemainUsable)
{
  // Arrange
  Ed25519KeyCallbacks callbacks;
  callbacks.signTransaction      = &signEd25519Transaction;
  callbacks.signSubTransaction   = &signEd25519SubTransaction;
  callbacks.publicKey            = &ed25519PublicKey;
  SecureKeyHandler       handler = SecureKeyHandler::ed25519("test-ed25519", callbacks);
  const SecureKeyHandler copy(handler);

  // Act
  const VkeyWitnessSet transactionWitnesses    = copy.signTransaction(sampleTransaction());
  const VkeyWitnessSet subTransactionWitnesses = copy.signSubTransaction(sampleSubTransaction());

  // Assert
  EXPECT_EQ(copy.name(), "test-ed25519");
  EXPECT_EQ(copy.type(), SecureKeyHandlerType::Ed25519);
  EXPECT_TRUE(transactionWitnesses.size() == 0);
  EXPECT_TRUE(subTransactionWitnesses.size() == 0);
  EXPECT_EQ(copy.publicKey(), ed25519PublicKey());
  EXPECT_THROW(copy.signTransaction(sampleTransaction(), { DerivationPath() }), std::logic_error);
}

TEST(SecureKeyHandler, CallbackErrorsRemainAvailable)
{
  // Arrange
  Ed25519KeyCallbacks callbacks;
  callbacks.signTransaction      = &failSigning;
  callbacks.signSubTransaction   = &signEd25519SubTransaction;
  callbacks.publicKey            = &ed25519PublicKey;
  const SecureKeyHandler handler = SecureKeyHandler::ed25519("failing", callbacks);

  // Act
  EXPECT_THROW(handler.signTransaction(sampleTransaction()), Error);

  // Assert
  EXPECT_EQ(handler.lastError(), "Signer rejected transaction");
}

TEST(SecureKeyHandler, MissingRequiredCallbacksAreRejected)
{
  // Arrange
  Bip32KeyCallbacks   bip32;
  Ed25519KeyCallbacks ed25519;

  // Act and assert
  EXPECT_THROW(SecureKeyHandler::bip32("missing", bip32), std::invalid_argument);
  EXPECT_THROW(SecureKeyHandler::ed25519("missing", ed25519), std::invalid_argument);
}

TEST(SecureKeyHandler, BorrowsNativeHandleWhileRetainingCallbacks)
{
  // Arrange
  Ed25519KeyCallbacks callbacks;
  callbacks.signTransaction       = &signEd25519Transaction;
  callbacks.signSubTransaction    = &signEd25519SubTransaction;
  callbacks.publicKey             = &ed25519PublicKey;
  SecureKeyHandler        handler = SecureKeyHandler::ed25519("interop", callbacks);
  const SecureKeyHandler& view    = handler;

  // Act & Assert
  EXPECT_NE(handler.nativeHandle(), nullptr);
  EXPECT_EQ(handler.nativeHandle(), view.nativeHandle());
  EXPECT_EQ(handler.publicKey(), ed25519PublicKey());
}
