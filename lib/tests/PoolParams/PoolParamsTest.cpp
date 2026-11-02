/**
 * @file PoolParamsTest.cpp
 *
 * @brief Unit tests for stake pool parameters.
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

static PoolParams
samplePoolParams()
{
  const Blake2bHash   operatorHash = Blake2bHash::fromHex(std::string(56, '1'));
  const Blake2bHash   vrfHash      = Blake2bHash::fromHex(std::string(64, '2'));
  const Credential    credential(CredentialType::KeyHash, operatorHash);
  const RewardAddress reward(NetworkId::Testnet, credential);
  const PoolOwners    owners { operatorHash };
  const Relays        relays { Relay(SingleHostNameRelay("relay.example.com", 3001)) };

  return PoolParams(operatorHash, vrfHash, 10'000'000, 340'000, UnitInterval(1, 20), reward, owners, relays);
}

TEST(PoolParams, RoundTripsFieldsAndOptionalExtensions)
{
  // Arrange
  PoolParams         params = samplePoolParams();
  const PoolParams   original(params);
  const BlsKey       bls(std::vector<std::uint8_t>(96, 1), std::vector<std::uint8_t>(48, 2));
  const PoolMetadata metadata("https://example.com/pool.json", Blake2bHash::fromHex(std::string(64, '3')));

  // Act
  params.setBlsKey(bls);
  params.setMetadata(metadata);

  // Assert
  EXPECT_EQ(params.pledge(), 10'000'000U);
  EXPECT_EQ(params.cost(), 340'000U);
  EXPECT_EQ(params.margin(), UnitInterval(1, 20));
  EXPECT_EQ(params.owners().size(), 1U);
  EXPECT_EQ(params.relays().size(), 1U);
  EXPECT_EQ(params.blsKey(), bls);
  EXPECT_EQ(params.metadata(), metadata);
  EXPECT_FALSE(original.blsKey().has_value());
  EXPECT_FALSE(original.metadata().has_value());
  EXPECT_EQ(PoolParams::fromCbor(params.toCbor()), params);
  EXPECT_EQ(PoolParams::fromCbor(original.toCbor()), original);
  EXPECT_NE(params, original);
  EXPECT_FALSE(params.toJson().empty());

  // Act
  params.setBlsKey(std::nullopt);
  params.setMetadata(std::nullopt);

  // Assert
  EXPECT_EQ(params, original);
}

TEST(PoolParams, CopiesAndReplacesComponents)
{
  // Arrange
  PoolParams          params = samplePoolParams();
  const PoolParams    original(params);
  Blake2bHash         operatorHash = params.operatorKeyHash();
  PoolOwners          owners       = params.owners();
  const Blake2bHash   extra        = Blake2bHash::fromHex(std::string(56, '4'));
  const Blake2bHash   vrf          = Blake2bHash::fromHex(std::string(64, '5'));
  const RewardAddress reward(NetworkId::Mainnet, Credential(CredentialType::KeyHash, extra));
  const Relays        relays { Relay(MultiHostNameRelay("other.example.com")) };

  // Act
  operatorHash = extra;
  owners.insert(extra);
  params.setOperatorKeyHash(operatorHash);
  params.setOwners(owners);
  params.setPledge(11);
  params.setCost(12);
  params.setMargin(UnitInterval(1, 2));
  params.setVrfKeyHash(vrf);
  params.setRewardAccount(reward);
  params.setRelays(relays);

  // Assert
  EXPECT_EQ(params.operatorKeyHash(), extra);
  EXPECT_EQ(params.owners().size(), 2U);
  EXPECT_EQ(params.pledge(), 11U);
  EXPECT_EQ(params.cost(), 12U);
  EXPECT_EQ(params.margin(), UnitInterval(1, 2));
  EXPECT_EQ(params.vrfKeyHash(), vrf);
  EXPECT_EQ(params.rewardAccount(), reward);
  EXPECT_EQ(params.relays(), relays);
  EXPECT_EQ(original.owners().size(), 1U);
  EXPECT_EQ(original.pledge(), 10'000'000U);
  EXPECT_EQ(PoolParams::fromCbor(params.toCbor()), params);
}

TEST(PoolParams, RejectsInvalidHashesAndTrailingCbor)
{
  // Arrange
  PoolParams                params = samplePoolParams();
  const PoolParams          original(params);
  const Blake2bHash         shortHash = Blake2bHash::fromHex("11");
  std::vector<std::uint8_t> trailing  = params.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(params.setOperatorKeyHash(shortHash), std::invalid_argument);
  EXPECT_THROW(params.setVrfKeyHash(shortHash), std::invalid_argument);
  EXPECT_THROW(PoolParams::fromCbor(trailing), std::invalid_argument);
  EXPECT_EQ(params, original);
}

TEST(PoolParams, ReadsNativePoolRegistrationFixture)
{
  // Arrange
  const std::string fixture =
    "581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef9258208dd154228946bd12967c12bedb1cb6038b78"
    "f8b84a1760b1a788fa72a4af3db01927101903e8d81e820105581de1cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99"
    "893828ec810fd9010281581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f8383011913886b657861"
    "6d706c652e636f6d8400191770447f000001f682026b6578616d706c652e636f6d827368747470733a2f2f6578616d706c65"
    "2e636f6d58200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d5";
  CborReader                reader(fixture);
  std::vector<std::uint8_t> expected;

  while (reader.bytesRemaining() != 0)
  {
    const std::vector<std::uint8_t> field = reader.readEncodedValue();
    expected.insert(expected.end(), field.begin(), field.end());
  }

  // Act
  const PoolParams params = PoolParams::fromCbor(fixture);

  // Assert
  EXPECT_EQ(params.operatorKeyHash().toHex(), "d85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef92");
  EXPECT_EQ(params.pledge(), 10000U);
  EXPECT_EQ(params.cost(), 1000U);
  EXPECT_EQ(params.margin(), UnitInterval(1, 5));
  ASSERT_TRUE(params.metadata().has_value());
  EXPECT_EQ(params.metadata()->url(), "https://example.com");
  EXPECT_EQ(params.toCbor(), expected);
}
