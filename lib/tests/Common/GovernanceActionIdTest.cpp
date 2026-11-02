/**
 * @file GovernanceActionIdTest.cpp
 *
 * @brief Tests for GovernanceActionId.
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

#include <Cometa/Common/GovernanceActionId.h>
#include <Cometa/Error.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(GovernanceActionId, MatchesKnownEncodings)
{
  // Arrange
  const Blake2bHash        hash = Blake2bHash::fromHex(std::string(64, '0'));
  const GovernanceActionId value(hash, 3);
  const std::string        cbor   = "825820" + std::string(64, '0') + "03";
  const std::string        bech32 = "gov_action1qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqpzklpgpf";

  // Act
  const GovernanceActionId decoded = GovernanceActionId::fromCbor(cbor);

  // Assert
  EXPECT_EQ(decoded, value);
  EXPECT_EQ(decoded.hash(), hash);
  EXPECT_EQ(decoded.index(), 3);
  EXPECT_EQ(decoded.toCbor(), value.toCbor());
  EXPECT_EQ(GovernanceActionId::fromCbor(value.toCbor()), value);
  EXPECT_EQ(GovernanceActionId::fromBech32(bech32), GovernanceActionId(hash, 17));
  EXPECT_EQ(GovernanceActionId(hash, 17).toBech32(), bech32);
  EXPECT_EQ(value.toJson(), "{\"transaction_id\":\"" + std::string(64, '0') + "\",\"gov_action_index\":\"3\"}");
}

TEST(GovernanceActionId, MutationUpdatesBech32AndPreservesOriginal)
{
  // Arrange
  const GovernanceActionId original(Blake2bHash::fromHex(std::string(64, '0')), 1);
  GovernanceActionId       copy = original;
  const Blake2bHash        hash = Blake2bHash::fromHex(std::string(64, '1'));

  // Act
  copy.setIndex(255);
  const GovernanceActionId changedIndex = GovernanceActionId::fromBech32(copy.toBech32());
  copy.setHash(hash);
  const GovernanceActionId changedHash = GovernanceActionId::fromBech32(copy.toBech32());

  // Assert
  EXPECT_EQ(changedIndex.index(), 255);
  EXPECT_EQ(changedIndex.hash(), original.hash());
  EXPECT_EQ(changedHash.hash(), hash);
  EXPECT_EQ(changedHash.index(), 255);
  EXPECT_EQ(original.index(), 1);
  EXPECT_NE(original.hash(), hash);
  EXPECT_EQ(GovernanceActionId::fromCbor(copy.toCbor()), copy);
}

TEST(GovernanceActionId, PreservesUnsignedIndexButRejectsTruncatedBech32)
{
  // Arrange
  const std::uint64_t      maximum = std::numeric_limits<std::uint64_t>::max();
  const GovernanceActionId value(Blake2bHash::fromHex(std::string(64, '0')), maximum);

  // Act
  const GovernanceActionId decoded = GovernanceActionId::fromCbor(value.toCbor());

  // Assert
  EXPECT_EQ(decoded.index(), maximum);
  EXPECT_NE(decoded.toJson().find("18446744073709551615"), std::string::npos);
  EXPECT_THROW(decoded.toBech32(), std::overflow_error);
}

TEST(GovernanceActionId, RejectsInvalidHashWithoutChangingValue)
{
  // Arrange
  GovernanceActionId       value(Blake2bHash::fromHex(std::string(64, '0')), 1);
  const GovernanceActionId before = value;

  // Act & Assert
  EXPECT_THROW(value.setHash(Blake2bHash::fromHex(std::string(56, '0'))), std::invalid_argument);
  EXPECT_EQ(value, before);
}

TEST(GovernanceActionId, RejectsMalformedInput)
{
  // Act & Assert
  EXPECT_THROW(GovernanceActionId::fromCbor(""), std::invalid_argument);
  EXPECT_THROW(GovernanceActionId::fromCbor("xz"), std::invalid_argument);
  EXPECT_THROW(GovernanceActionId::fromCbor("8100"), Error);
  EXPECT_THROW(GovernanceActionId::fromCbor("825820" + std::string(64, '0') + "0100"), std::invalid_argument);
  EXPECT_THROW(GovernanceActionId::fromBech32(""), Error);
  EXPECT_THROW(GovernanceActionId::fromBech32("gov_action1zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zyg3zygsqqjxekw"), Error);
}

TEST(GovernanceActionId, MovedFromValuesRejectAccessAndMutation)
{
  // Arrange
  GovernanceActionId       original(Blake2bHash::fromHex(std::string(64, '0')), 1);
  const GovernanceActionId moved = std::move(original);

  // Act & Assert
  EXPECT_THROW(original.index(), std::logic_error);
  EXPECT_THROW(original.hash(), std::logic_error);
  EXPECT_THROW(original.setIndex(2), std::logic_error);
  EXPECT_THROW(original.setHash(moved.hash()), std::logic_error);
  EXPECT_THROW(original.toBech32(), std::logic_error);
  EXPECT_THROW(original.toJson(), std::logic_error);
}
