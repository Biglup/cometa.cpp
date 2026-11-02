/**
 * @file NetworkIdTest.cpp
 *
 * @brief Tests for NetworkId.
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

#include <Cometa/Common/NetworkId.h>

#include <cardano/common/network_id.h>

#include <gtest/gtest.h>

#include <cstdint>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(NetworkId, MatchesCardanoValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(NetworkId::Testnet), static_cast<std::uint32_t>(CARDANO_NETWORK_ID_TEST_NET));
  EXPECT_EQ(static_cast<std::uint32_t>(NetworkId::Mainnet), static_cast<std::uint32_t>(CARDANO_NETWORK_ID_MAIN_NET));
}
