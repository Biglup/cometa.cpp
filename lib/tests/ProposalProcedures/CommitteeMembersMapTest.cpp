/**
 * @file CommitteeMembersMapTest.cpp
 *
 * @brief Unit tests for committee members maps.
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

TEST(CommitteeMembersMap, MatchesNativeCbor)
{
  // Arrange
  const Credential    credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  CommitteeMembersMap map;
  map.insert(credential, 200);
  const std::string cbor = "a18200581c" + std::string(56, '0') + "18c8";
  CborReader        reader(cbor);

  // Act
  const CommitteeMembersMap decoded = CommitteeMembersMap::fromCbor(cbor);

  // Assert
  EXPECT_EQ(map.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, map);
  EXPECT_EQ(map.size(), 1U);
  EXPECT_EQ(map.get(credential), 200U);
  EXPECT_EQ(map.keyAt(0), credential);
  EXPECT_EQ(map.valueAt(0), 200U);
  EXPECT_EQ(map.at(0), (std::make_pair(credential, std::uint64_t(200))));
  EXPECT_TRUE(map.keys().contains(credential));
  EXPECT_FALSE(map.toJson().empty());
}

TEST(CommitteeMembersMap, CopiesAndRejectsInvalidIndex)
{
  // Arrange
  const Credential    credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  CommitteeMembersMap map;
  map.insert(credential, 200);
  const CommitteeMembersMap copied(map);
  const Credential          absent(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));

  // Act
  map.insert(absent, 201);

  // Assert
  EXPECT_EQ(copied.size(), 1U);
  EXPECT_EQ(map.size(), 2U);
  EXPECT_FALSE(copied.get(absent).has_value());
  EXPECT_THROW(map.keyAt(2), std::out_of_range);
  EXPECT_THROW(map.valueAt(2), std::out_of_range);
  EXPECT_THROW(map.at(2), std::out_of_range);
  EXPECT_EQ(CommitteeMembersMap::fromCbor(copied.toCbor()), copied);
}
