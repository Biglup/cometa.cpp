/**
 * @file UpdateTest.cpp
 *
 * @brief Unit tests for epoch parameter update proposals.
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

TEST(Update, MatchesNativeCborArray)
{
  // Arrange
  const Blake2bHash   key = Blake2bHash::fromHex(std::string(56, '0'));
  ProtocolParamUpdate parameters;
  parameters.setMinFeeA(std::uint64_t(200));
  ProposedParamUpdates proposals;
  proposals.add(key, parameters);
  const std::string cbor = "82a1581c" + std::string(56, '0') + "a10018c81903e8";
  CborReader        reader(cbor);

  // Act
  const Update update(1000, proposals);

  // Assert
  EXPECT_EQ(update.epoch(), 1000);
  EXPECT_EQ(update.proposals(), proposals);
  EXPECT_EQ(update.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(Update::fromCbor(cbor), update);
  EXPECT_FALSE(update.toJson().empty());
}

TEST(Update, CopiesAndSettersAreIndependent)
{
  // Arrange
  const Blake2bHash   key = Blake2bHash::fromHex(std::string(56, '0'));
  ProtocolParamUpdate parameters;
  parameters.setMinFeeA(std::uint64_t(44));
  ProposedParamUpdates proposals;
  proposals.add(key, parameters);
  Update               update(12, proposals);
  const Update         copied(update);
  ProposedParamUpdates replacement;
  replacement.add(key, parameters);

  // Act
  proposals.add(Blake2bHash::fromHex(std::string(56, '1')), parameters);
  update.setEpoch(13);
  update.setProposals(replacement);
  replacement.add(Blake2bHash::fromHex(std::string(56, '2')), parameters);
  ProposedParamUpdates retrieved = update.proposals();
  retrieved.add(Blake2bHash::fromHex(std::string(56, '3')), parameters);

  // Assert
  EXPECT_EQ(copied.epoch(), 12);
  EXPECT_EQ(copied.proposals().size(), 1);
  EXPECT_EQ(update.epoch(), 13);
  EXPECT_EQ(update.proposals().size(), 1);
  EXPECT_EQ(Update::fromCbor(update.toCbor()), update);
}
