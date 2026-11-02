/**
 * @file SecureKeyHandlerTypeTest.cpp
 *
 * @brief Unit tests for secure key handler types.
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

#include <Cometa/KeyHandlers/SecureKeyHandlerType.h>

#include <cardano/key_handlers/secure_key_handler_type.h>

#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(SecureKeyHandlerType, ValuesMatchNativeTypes)
{
  // Arrange
  const Cometa::SecureKeyHandlerType ed25519 = Cometa::SecureKeyHandlerType::Ed25519;

  // Act
  const Cometa::SecureKeyHandlerType bip32 = Cometa::SecureKeyHandlerType::Bip32;

  // Assert
  EXPECT_EQ(static_cast<int>(ed25519), CARDANO_SECURE_KEY_HANDLER_TYPE_ED25519);
  EXPECT_EQ(static_cast<int>(bip32), CARDANO_SECURE_KEY_HANDLER_TYPE_BIP32);
}
