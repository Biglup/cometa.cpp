/**
 * @file NativeScriptTypeTest.cpp
 *
 * @brief Unit tests for PlutusData.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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

#include <Cometa/Scripts/NativeScripts/NativeScriptType.h>

#include <cardano/scripts/native_scripts/native_script_type.h>
#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(NativeScriptType, MatchesNativeDiscriminators)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(NativeScriptType::RequirePubkey), CARDANO_NATIVE_SCRIPT_TYPE_REQUIRE_PUBKEY);
  EXPECT_EQ(static_cast<int>(NativeScriptType::RequireAllOf), CARDANO_NATIVE_SCRIPT_TYPE_REQUIRE_ALL_OF);
  EXPECT_EQ(static_cast<int>(NativeScriptType::RequireAnyOf), CARDANO_NATIVE_SCRIPT_TYPE_REQUIRE_ANY_OF);
  EXPECT_EQ(static_cast<int>(NativeScriptType::RequireNOfK), CARDANO_NATIVE_SCRIPT_TYPE_REQUIRE_N_OF_K);
  EXPECT_EQ(static_cast<int>(NativeScriptType::InvalidBefore), CARDANO_NATIVE_SCRIPT_TYPE_INVALID_BEFORE);
  EXPECT_EQ(static_cast<int>(NativeScriptType::InvalidAfter), CARDANO_NATIVE_SCRIPT_TYPE_INVALID_AFTER);
  EXPECT_EQ(static_cast<int>(NativeScriptType::RequireGuard), CARDANO_NATIVE_SCRIPT_TYPE_REQUIRE_GUARD);
}

TEST(NativeScriptType, ReturnsNativeTypeNames)
{
  // Arrange
  const NativeScriptType values[] = { NativeScriptType::RequirePubkey, NativeScriptType::RequireAllOf, NativeScriptType::RequireAnyOf, NativeScriptType::RequireNOfK, NativeScriptType::InvalidBefore, NativeScriptType::InvalidAfter, NativeScriptType::RequireGuard };

  // Act & Assert
  for (const NativeScriptType value: values)
  {
    EXPECT_EQ(toString(value), cardano_native_script_type_to_string(static_cast<cardano_native_script_type_t>(value)));
  }
}
