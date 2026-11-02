/**
 * @file WitnessSetTest.cpp
 *
 * @brief Unit tests for aggregate transaction witnesses.
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

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(WitnessSet, EmptySetMatchesLedgerCbor)
{
  // Arrange
  const WitnessSet set;
  CborReader       reader("a0");

  // Act
  const WitnessSet decoded = WitnessSet::fromCbor(set.toCbor());

  // Assert
  EXPECT_EQ(set.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, set);
  EXPECT_FALSE(set.nativeScripts().has_value());
  EXPECT_FALSE(set.redeemers().has_value());
  EXPECT_FALSE(set.toJson().empty());
}

TEST(WitnessSet, ScriptAdditionSelectsLanguageAndCopiesIndependently)
{
  // Arrange
  WitnessSet   set;
  const Script native(NativeScript(ScriptInvalidBefore(5)));
  const Script plutus(PlutusV4Script(std::vector<std::uint8_t> { 0x01, 0x02 }));

  // Act
  set.addScript(native);
  set.addScript(plutus);
  const WitnessSet copy(set);
  set.setNativeScripts(std::nullopt);

  // Assert
  ASSERT_TRUE(copy.nativeScripts().has_value());
  EXPECT_EQ(copy.nativeScripts()->size(), 1U);
  ASSERT_TRUE(copy.plutusV4Scripts().has_value());
  EXPECT_EQ(copy.plutusV4Scripts()->size(), 1U);
  EXPECT_FALSE(set.nativeScripts().has_value());
  EXPECT_EQ(WitnessSet::fromCbor(copy.toCbor()), copy);
}

TEST(WitnessSet, OptionalFieldsCopyAndClear)
{
  // Arrange
  WitnessSet set;

  // Act
  set.setVkeys(VkeyWitnessSet());
  set.setBootstrap(BootstrapWitnessSet());
  set.setNativeScripts(NativeScriptSet());
  set.setPlutusV1Scripts(PlutusV1ScriptSet());
  set.setPlutusV2Scripts(PlutusV2ScriptSet());
  set.setPlutusV3Scripts(PlutusV3ScriptSet());
  set.setPlutusV4Scripts(PlutusV4ScriptSet());
  set.setPlutusData(PlutusDataSet());
  set.setRedeemers(RedeemerList());
  const WitnessSet copy(set);

  // Assert
  EXPECT_TRUE(copy.vkeys().has_value());
  EXPECT_TRUE(copy.bootstrap().has_value());
  EXPECT_TRUE(copy.nativeScripts().has_value());
  EXPECT_TRUE(copy.plutusV1Scripts().has_value());
  EXPECT_TRUE(copy.plutusV2Scripts().has_value());
  EXPECT_TRUE(copy.plutusV3Scripts().has_value());
  EXPECT_TRUE(copy.plutusV4Scripts().has_value());
  EXPECT_TRUE(copy.plutusData().has_value());
  EXPECT_TRUE(copy.redeemers().has_value());

  // Act
  set.setVkeys(std::nullopt);
  set.setBootstrap(std::nullopt);
  set.setNativeScripts(std::nullopt);
  set.setPlutusV1Scripts(std::nullopt);
  set.setPlutusV2Scripts(std::nullopt);
  set.setPlutusV3Scripts(std::nullopt);
  set.setPlutusV4Scripts(std::nullopt);
  set.setPlutusData(std::nullopt);
  set.setRedeemers(std::nullopt);

  // Assert
  EXPECT_FALSE(set.vkeys().has_value());
  EXPECT_FALSE(set.bootstrap().has_value());
  EXPECT_FALSE(set.nativeScripts().has_value());
  EXPECT_FALSE(set.plutusV1Scripts().has_value());
  EXPECT_FALSE(set.plutusV2Scripts().has_value());
  EXPECT_FALSE(set.plutusV3Scripts().has_value());
  EXPECT_FALSE(set.plutusV4Scripts().has_value());
  EXPECT_FALSE(set.plutusData().has_value());
  EXPECT_FALSE(set.redeemers().has_value());
  EXPECT_TRUE(copy.vkeys().has_value());
}
