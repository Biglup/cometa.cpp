/**
 * @file Pbkdf2Test.cpp
 *
 * @brief Tests for Pbkdf2.
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

TEST(Pbkdf2, MatchesKnownVectorsAcrossMultipleBlocks)
{
  // Arrange
  const std::vector<std::uint8_t> salt { 's', 'a', 'l', 't' };
  const std::vector<std::uint8_t> expected = decodeHex("867f70cf1ade02cff3752599a3a53dc4af34c7a669815ae5d513554e1c8cf252c02d470a285a0501bad999bfe943c08f050235d7d68b1da55e63f73b60a57fce7b532e206c2967d4c7d2ffa460539fc4d4e5eec70125d74c6c7cf86d25284f297907fcea");

  // Act
  const std::vector<std::uint8_t> output = pbkdf2HmacSha512("password", salt, 1, 100);

  // Assert
  EXPECT_EQ(output, expected);
  EXPECT_EQ(pbkdf2HmacSha512("password", salt, 2, 32), decodeHex("e1d9c16aa681708a45f5c7c4e215ceb66e011a2e9f0040713f18aefdb866d53c"));
  EXPECT_EQ(pbkdf2HmacSha512("", salt, 1, 32), decodeHex("00ef42cdbfc98d29db20976608e455567fdddf141f6eb03b5a85addd25974f5d"));
}

TEST(Pbkdf2, PreservesPasswordViewBoundsAndNullBytes)
{
  // Arrange
  const std::string               password("pass\0word", 9);
  const std::vector<std::uint8_t> salt { 's', 'a', 0, 'l', 't' };

  // Act
  const std::vector<std::uint8_t> output = pbkdf2HmacSha512(password, salt, 2, 16);

  // Assert
  EXPECT_NE(output, pbkdf2HmacSha512(std::string_view(password.data(), 4), salt, 2, 16));
  EXPECT_EQ(pbkdf2HmacSha512(std::string_view(password.data(), 4), salt, 2, 16), pbkdf2HmacSha512("pass", salt, 2, 16));
}

TEST(Pbkdf2, RejectsInvalidParametersBeforeAllocating)
{
  // Act & Assert
  EXPECT_THROW(pbkdf2HmacSha512("password", {}, 1, 32), std::invalid_argument);
  EXPECT_THROW(pbkdf2HmacSha512("password", { 1 }, 0, 32), std::invalid_argument);
  EXPECT_THROW(pbkdf2HmacSha512("password", { 1 }, 1, 0), std::invalid_argument);

  if (sizeof(std::size_t) > 4)
  {
    EXPECT_THROW(pbkdf2HmacSha512("password", { 1 }, 1, std::numeric_limits<std::size_t>::max()), std::length_error);
  }
}
