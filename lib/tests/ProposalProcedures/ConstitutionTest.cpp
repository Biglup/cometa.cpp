/**
 * @file ConstitutionTest.cpp
 *
 * @brief Unit tests for constitution content.
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

TEST(Constitution, MatchesNativeCborWithoutScriptHash)
{
  // Arrange
  const Anchor       anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  const Constitution constitution(anchor);
  const std::string  cbor = "82826968747470733a2f2f615820" + std::string(64, '0') + "f6";
  CborReader         reader(cbor);

  // Act
  const Constitution decoded = Constitution::fromCbor(cbor);

  // Assert
  EXPECT_EQ(constitution.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, constitution);
  EXPECT_EQ(constitution.anchor(), anchor);
  EXPECT_FALSE(constitution.scriptHash().has_value());
  EXPECT_FALSE(constitution.toJson().empty());
}

TEST(Constitution, CopiesAndSetsBothFieldsIndependently)
{
  // Arrange
  const Anchor       anchor("https://a", Blake2bHash::fromHex(std::string(64, '0')));
  const Blake2bHash  script = Blake2bHash::fromHex(std::string(56, '1'));
  Constitution       constitution(anchor, script);
  const Constitution copied(constitution);

  // Act
  constitution.setAnchor(Anchor("https://b", Blake2bHash::fromHex(std::string(64, '2'))));
  constitution.setScriptHash(std::nullopt);

  // Assert
  EXPECT_EQ(copied.anchor(), anchor);
  EXPECT_EQ(copied.scriptHash(), script);
  EXPECT_FALSE(constitution.scriptHash().has_value());
  EXPECT_NE(constitution, copied);
  EXPECT_EQ(Constitution::fromCbor(copied.toCbor()), copied);
}
