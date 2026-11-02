/**
 * @file Crc32Test.cpp
 *
 * @brief Tests for Crc32.
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

TEST(Crc32, MatchesStandardCheckValueAndEmptyInput)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { '1', '2', '3', '4', '5', '6', '7', '8', '9' };

  // Act & Assert
  EXPECT_EQ(crc32(bytes), 0xcbf43926U);
  EXPECT_EQ(crc32({}), 0U);
}
