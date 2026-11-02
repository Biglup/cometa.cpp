/**
 * @file RelayTypeTest.cpp
 *
 * @brief Unit tests for RelayType.
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

#include <cardano/pool_params/relay_type.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(RelayType, MatchesNativeDiscriminatorsAndNames)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(RelayType::SingleHostAddress), CARDANO_RELAY_TYPE_SINGLE_HOST_ADDRESS);
  EXPECT_EQ(static_cast<int>(RelayType::SingleHostName), CARDANO_RELAY_TYPE_SINGLE_HOST_NAME);
  EXPECT_EQ(static_cast<int>(RelayType::MultiHostName), CARDANO_RELAY_TYPE_MULTI_HOST_NAME);
  EXPECT_EQ(toString(RelayType::SingleHostAddress), cardano_relay_type_to_string(CARDANO_RELAY_TYPE_SINGLE_HOST_ADDRESS));
  EXPECT_EQ(toString(RelayType::SingleHostName), cardano_relay_type_to_string(CARDANO_RELAY_TYPE_SINGLE_HOST_NAME));
  EXPECT_EQ(toString(RelayType::MultiHostName), cardano_relay_type_to_string(CARDANO_RELAY_TYPE_MULTI_HOST_NAME));
}
