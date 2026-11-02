/**
 * @file CommitteeTest.cpp
 *
 * @brief Unit tests for constitutional committees.
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

TEST(Committee, MatchesNativeCbor)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  Committee        committee(UnitInterval(1, 2));
  committee.addMember(credential, 200);
  const std::string cbor = "82a18200581c" + std::string(56, '0') + "18c8d81e820102";
  CborReader        reader(cbor);

  // Act
  const Committee decoded = Committee::fromCbor(cbor);

  // Assert
  EXPECT_EQ(committee.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(decoded, committee);
  EXPECT_EQ(committee.quorumThreshold(), UnitInterval(1, 2));
  EXPECT_EQ(committee.keys().size(), 1U);
  EXPECT_EQ(committee.memberEpoch(credential), 200U);
  EXPECT_EQ(committee.keyAt(0), credential);
  EXPECT_EQ(committee.valueAt(0), 200U);
  EXPECT_EQ(committee.at(0), (std::make_pair(credential, std::uint64_t(200))));
}

TEST(Committee, CopiesMembersAndThresholdIndependently)
{
  // Arrange
  const Credential credential(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '0')));
  Committee        committee(UnitInterval(1, 2));
  committee.addMember(credential, 200);
  const Committee  copied(committee);
  const Credential absent(CredentialType::KeyHash, Blake2bHash::fromHex(std::string(56, '1')));

  // Act
  committee.setQuorumThreshold(UnitInterval(2, 3));
  committee.addMember(absent, 201);

  // Assert
  EXPECT_EQ(copied.quorumThreshold(), UnitInterval(1, 2));
  EXPECT_EQ(copied.keys().size(), 1U);
  EXPECT_EQ(committee.keys().size(), 2U);
  EXPECT_EQ(copied.memberEpoch(absent), 0U);
  EXPECT_THROW(committee.keyAt(2), std::out_of_range);
  EXPECT_THROW(committee.valueAt(2), std::out_of_range);
  EXPECT_THROW(committee.at(2), std::out_of_range);
  EXPECT_EQ(Committee::fromCbor(copied.toCbor()), copied);
}
