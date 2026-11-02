/**
 * @file ScriptLanguageTest.cpp
 *
 * @brief Unit tests for ScriptLanguage.
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

#include <Cometa/Scripts/ScriptLanguage.h>

#include <cardano/scripts/script_language.h>
#include <gtest/gtest.h>

/* IMPLEMENTATION ************************************************************/

TEST(ScriptLanguage, MatchesLedgerLanguageTags)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(Cometa::ScriptLanguage::Native), CARDANO_SCRIPT_LANGUAGE_NATIVE);
  EXPECT_EQ(static_cast<int>(Cometa::ScriptLanguage::PlutusV1), CARDANO_SCRIPT_LANGUAGE_PLUTUS_V1);
  EXPECT_EQ(static_cast<int>(Cometa::ScriptLanguage::PlutusV2), CARDANO_SCRIPT_LANGUAGE_PLUTUS_V2);
  EXPECT_EQ(static_cast<int>(Cometa::ScriptLanguage::PlutusV3), CARDANO_SCRIPT_LANGUAGE_PLUTUS_V3);
  EXPECT_EQ(static_cast<int>(Cometa::ScriptLanguage::PlutusV4), CARDANO_SCRIPT_LANGUAGE_PLUTUS_V4);
}
