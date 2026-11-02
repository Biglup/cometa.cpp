/**
 * @file Emip3Test.cpp
 *
 * @brief Tests for Emip3.
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

#include <limits>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Emip3, DecryptsNativeKnownVector)
{
  // Arrange
  const std::vector<std::uint8_t> encrypted = decodeHex("00000000000000000000000000000000000000000000000000000000000000000000000000000000000000009ce1d7784a05efd109ad89c29fea0775bf085ac03988089b3a93");

  // Act
  const std::vector<std::uint8_t> plaintext = decryptEmip3(encrypted, "password");

  // Assert
  EXPECT_EQ(plaintext, decodeHex("00010203040506070809"));
}

TEST(Emip3, EncryptsWithFreshRandomSaltAndNonce)
{
  // Arrange
  const std::vector<std::uint8_t> plaintext { 1, 2, 3, 0, 4 };
  const std::string               password("a\0b", 3);

  // Act
  const std::vector<std::uint8_t> first  = encryptEmip3(plaintext, password);
  const std::vector<std::uint8_t> second = encryptEmip3(plaintext, password);

  // Assert
  EXPECT_EQ(first.size(), plaintext.size() + 60);
  EXPECT_NE(first, second);
  EXPECT_EQ(decryptEmip3(first, password), plaintext);
  EXPECT_EQ(decryptEmip3(second, password), plaintext);
  EXPECT_THROW(decryptEmip3(first, std::string_view(password.data(), 1)), Error);
}

TEST(Emip3, SupportsEmptyPlaintextAndPassword)
{
  // Act
  const std::vector<std::uint8_t> encrypted = encryptEmip3({}, "");

  // Assert
  EXPECT_EQ(encrypted.size(), 60U);
  EXPECT_TRUE(decryptEmip3(encrypted, "").empty());
}

TEST(Emip3, RejectsTruncationAndAlteredAuthenticationTag)
{
  // Arrange
  std::vector<std::uint8_t> encrypted = encryptEmip3({ 1, 2, 3 }, "password");
  encrypted[44]                       ^= 1;

  // Act & Assert
  EXPECT_THROW(decryptEmip3({}, "password"), Error);
  EXPECT_THROW(decryptEmip3(std::vector<std::uint8_t>(59, 0), "password"), Error);
  EXPECT_THROW(decryptEmip3(encrypted, "password"), Error);
}
