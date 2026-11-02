/**
 * @file ProposedParamUpdatesTest.cpp
 *
 * @brief Unit tests for genesis delegate parameter proposals.
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

TEST(ProposedParamUpdates, MatchesNativeCborMap)
{
  // Arrange
  const Blake2bHash   key = Blake2bHash::fromHex(std::string(56, '0'));
  ProtocolParamUpdate update;
  update.setMinFeeA(std::uint64_t(200));
  const std::string    cbor = "a1581c" + std::string(56, '0') + "a10018c8";
  CborReader           reader(cbor);
  ProposedParamUpdates proposals;

  // Act
  proposals.add(key, update);

  // Assert
  EXPECT_EQ(proposals.size(), 1);
  EXPECT_EQ(proposals.toCbor(), reader.readEncodedValue());
  ASSERT_TRUE(proposals.get(key).has_value());
  EXPECT_EQ(proposals.get(key)->minFeeA(), std::optional<std::uint64_t>(200));
  EXPECT_EQ(proposals.keyAt(0), key);
  EXPECT_EQ(proposals.valueAt(0), update);
  EXPECT_EQ(proposals.at(0).first, key);
  EXPECT_EQ(proposals.at(0).second, update);
  EXPECT_EQ(ProposedParamUpdates::fromCbor(cbor), proposals);
  EXPECT_FALSE(proposals.toJson().empty());
}

TEST(ProposedParamUpdates, SortsAndCopiesProposals)
{
  // Arrange
  const Blake2bHash   first  = Blake2bHash::fromHex(std::string(56, '0'));
  const Blake2bHash   second = Blake2bHash::fromHex(std::string(56, '1'));
  ProtocolParamUpdate update;
  update.setMinFeeA(std::uint64_t(44));
  ProposedParamUpdates proposals;
  proposals.add(second, update);
  proposals.add(first, update);
  const ProposedParamUpdates copied(proposals);
  ProposedParamUpdates       assigned;

  // Act
  assigned                                     = proposals;
  std::optional<ProtocolParamUpdate> retrieved = proposals.get(first);
  ASSERT_TRUE(retrieved.has_value());
  retrieved->setMinFeeA(std::uint64_t(99));

  // Assert
  EXPECT_EQ(proposals.keyAt(0), first);
  EXPECT_EQ(proposals.keyAt(1), second);
  EXPECT_EQ(proposals.get(first)->minFeeA(), std::optional<std::uint64_t>(44));
  EXPECT_EQ(copied, proposals);
  EXPECT_EQ(assigned, proposals);
  EXPECT_FALSE(proposals.get(Blake2bHash::fromHex(std::string(56, '2'))).has_value());
  EXPECT_THROW(proposals.at(2), std::out_of_range);
}
