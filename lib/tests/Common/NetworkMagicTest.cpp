/**
 * @file NetworkMagicTest.cpp
 *
 * @brief Tests for NetworkMagic.
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

#include <Cometa/Common/NetworkMagic.h>

#include <cardano/common/network_magic.h>

#include <gtest/gtest.h>

#include <cstdint>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(NetworkMagic, MatchesCardanoValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(NetworkMagic::Preprod), static_cast<std::uint32_t>(CARDANO_NETWORK_MAGIC_PREPROD));
  EXPECT_EQ(static_cast<std::uint32_t>(NetworkMagic::Preview), static_cast<std::uint32_t>(CARDANO_NETWORK_MAGIC_PREVIEW));
  EXPECT_EQ(static_cast<std::uint32_t>(NetworkMagic::SanchoNet), static_cast<std::uint32_t>(CARDANO_NETWORK_MAGIC_SANCHONET));
  EXPECT_EQ(static_cast<std::uint32_t>(NetworkMagic::Mainnet), static_cast<std::uint32_t>(CARDANO_NETWORK_MAGIC_MAINNET));
}

TEST(NetworkMagic, NamesKnownAndUnknownNetworks)
{
  // Act & Assert
  EXPECT_EQ(toString(NetworkMagic::Mainnet), "mainnet");
  EXPECT_EQ(toString(NetworkMagic::Preprod), "preprod");
  EXPECT_EQ(toString(NetworkMagic::Preview), "preview");
  EXPECT_EQ(toString(NetworkMagic::SanchoNet), "sanchonet");
  EXPECT_EQ(toString(static_cast<NetworkMagic>(0xffffffffU)), "unknown");
}
