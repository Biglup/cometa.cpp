/**
 * @file ByteOrderTest.cpp
 *
 * @brief Tests for ByteOrder.
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

#include <Cometa/Common/ByteOrder.h>

#include <cardano/common/byte_order.h>

#include <gtest/gtest.h>

#include <cstdint>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(ByteOrder, MatchesCardanoValues)
{
  // Act & Assert
  EXPECT_EQ(static_cast<std::uint32_t>(ByteOrder::LittleEndian), static_cast<std::uint32_t>(CARDANO_BYTE_ORDER_LITTLE_ENDIAN));
  EXPECT_EQ(static_cast<std::uint32_t>(ByteOrder::BigEndian), static_cast<std::uint32_t>(CARDANO_BYTE_ORDER_BIG_ENDIAN));
}
