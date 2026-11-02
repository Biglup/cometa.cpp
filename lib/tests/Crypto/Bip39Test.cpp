/**
 * @file Bip39Test.cpp
 *
 * @brief Tests for Bip39.
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

TEST(Bip39, MatchesZeroEntropyMnemonic)
{
  // Arrange
  const std::vector<std::uint8_t> entropy(16, 0);
  std::vector<std::string>        expected(12, "abandon");
  expected.back() = "about";

  // Act
  const std::vector<std::string> words = entropyToMnemonic(entropy);

  // Assert
  EXPECT_EQ(words, expected);
  EXPECT_EQ(mnemonicToEntropy(words), entropy);
}

TEST(Bip39, SupportsEveryEntropyLength)
{
  // Act & Assert
  for (std::size_t size = 16; size <= 32; size += 4)
  {
    const std::vector<std::uint8_t> entropy(size, 0xff);
    const std::vector<std::string>  words = entropyToMnemonic(entropy);

    EXPECT_EQ(words.size(), size * 3 / 4);
    EXPECT_EQ(mnemonicToEntropy(words), entropy);
  }
}

TEST(Bip39, RejectsInvalidSizeWordsAndChecksum)
{
  // Arrange
  std::vector<std::string> words(12, "abandon");

  // Act & Assert
  EXPECT_THROW(entropyToMnemonic({}), Error);
  EXPECT_THROW(entropyToMnemonic(std::vector<std::uint8_t>(17, 0)), Error);
  EXPECT_THROW(mnemonicToEntropy({}), Error);
  EXPECT_THROW(mnemonicToEntropy(words), Error);

  words.back() = "About";
  EXPECT_THROW(mnemonicToEntropy(words), Error);

  words.back() = std::string("about\0suffix", 12);
  EXPECT_THROW(mnemonicToEntropy(words), std::invalid_argument);
}
