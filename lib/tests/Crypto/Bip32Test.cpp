/**
 * @file Bip32Test.cpp
 *
 * @brief BIP32 derivation indices.
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

#include <Cometa/Crypto/Bip32.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(Bip32, HardensIndicesWithoutChangingAlreadyHardenedIndices)
{
  // Act & Assert
  EXPECT_EQ(hardenIndex(0), 0x80000000U);
  EXPECT_EQ(hardenIndex(1852), 0x8000073cU);
  EXPECT_EQ(hardenIndex(0x7fffffffU), 0xffffffffU);
  EXPECT_EQ(hardenIndex(0x80000000U), 0x80000000U);
  EXPECT_EQ(hardenIndex(0xffffffffU), 0xffffffffU);
}
