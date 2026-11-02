/**
 * @file PlutusLanguageVersionTest.cpp
 *
 * @brief Unit tests for PlutusLanguageVersion.
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

#include <Cometa/Scripts/PlutusScripts/PlutusLanguageVersion.h>

#include <cardano/scripts/plutus_scripts/plutus_language_version.h>
#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(PlutusLanguageVersion, MatchesLedgerLanguageTags)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Cometa::PlutusLanguageVersion::V1), CARDANO_PLUTUS_LANGUAGE_VERSION_V1);
  EXPECT_EQ(static_cast<int>(Cometa::PlutusLanguageVersion::V2), CARDANO_PLUTUS_LANGUAGE_VERSION_V2);
  EXPECT_EQ(static_cast<int>(Cometa::PlutusLanguageVersion::V3), CARDANO_PLUTUS_LANGUAGE_VERSION_V3);
  EXPECT_EQ(static_cast<int>(Cometa::PlutusLanguageVersion::V4), CARDANO_PLUTUS_LANGUAGE_VERSION_V4);
}
