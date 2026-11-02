/**
 * @file Blake2bHashSizeTest.cpp
 *
 * @brief BLAKE2b digest size tests.
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

#include <Cometa/Crypto/Blake2bHash.h>

#include <cardano/crypto/blake2b_hash_size.h>
#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Blake2bHashSize, MatchesNativeDigestLengths)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Blake2bHashSize::Bits224), CARDANO_BLAKE2B_HASH_SIZE_224);
  EXPECT_EQ(static_cast<int>(Blake2bHashSize::Bits256), CARDANO_BLAKE2B_HASH_SIZE_256);
  EXPECT_EQ(static_cast<int>(Blake2bHashSize::Bits512), CARDANO_BLAKE2B_HASH_SIZE_512);
}

TEST(Blake2bHashSize, ComputesNamedLengthsAndRejectsUnknownValues)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 1, 2, 3 };

  // Act & Assert
  EXPECT_EQ(Blake2bHash::compute(bytes, Blake2bHashSize::Bits224), Blake2bHash::compute(bytes, 28));
  EXPECT_EQ(Blake2bHash::compute(bytes, Blake2bHashSize::Bits256), Blake2bHash::compute(bytes, 32));
  EXPECT_EQ(Blake2bHash::compute(bytes, Blake2bHashSize::Bits512), Blake2bHash::compute(bytes, 64));
  EXPECT_THROW(Blake2bHash::compute(bytes, static_cast<Blake2bHashSize>(1)), std::invalid_argument);
}
