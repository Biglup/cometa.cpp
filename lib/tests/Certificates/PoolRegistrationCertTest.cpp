/**
 * @file PoolRegistrationCertTest.cpp
 *
 * @brief Unit tests for pool registration certificates.
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
sampleParams()
{
  const Blake2bHash operatorHash = Blake2bHash::fromHex(std::string(56, '1'));
  const Credential  credential(CredentialType::KeyHash, operatorHash);
  return PoolParams(operatorHash, Blake2bHash::fromHex(std::string(64, '2')), 10'000, 1'000, UnitInterval(1, 5), RewardAddress(NetworkId::Testnet, credential), PoolOwners { operatorHash }, Relays());
}

TEST(PoolRegistrationCert, MatchesNativeCborFixture)
{
  // Arrange
  const std::string fixture =
    "8a03581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef9258208dd154228946bd12967c12bedb1cb603"
    "8b78f8b84a1760b1a788fa72a4af3db01927101903e8d81e820105581de1cb0ec2692497b458e46812c8a5bfa2931d1a2d96"
    "5a99893828ec810fd9010281581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f8383011913886b65"
    "78616d706c652e636f6d8400191770447f000001f682026b6578616d706c652e636f6d827368747470733a2f2f6578616d70"
    "6c652e636f6d58200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d5";
  const std::string paramsFixture =
    "581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef9258208dd154228946bd12967c12bedb1cb6038b78"
    "f8b84a1760b1a788fa72a4af3db01927101903e8d81e820105581de1cb0ec2692497b458e46812c8a5bfa2931d1a2d965a99"
    "893828ec810fd9010281581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f8383011913886b657861"
    "6d706c652e636f6d8400191770447f000001f682026b6578616d706c652e636f6d827368747470733a2f2f6578616d706c65"
    "2e636f6d58200f3abbc8fc19c2e61bab6059bf8a466e6e754833a08a62a6c56fe0e78f19d9d5";
  CborReader encoded(fixture);

  // Act
  const PoolRegistrationCert cert = PoolRegistrationCert::fromCbor(fixture);

  // Assert
  EXPECT_EQ(cert.params(), PoolParams::fromCbor(paramsFixture));
  EXPECT_EQ(cert.toCbor(), encoded.readEncodedValue());
  EXPECT_FALSE(cert.toJson().empty());
}

TEST(PoolRegistrationCert, CopiesAndReplacesParameters)
{
  // Arrange
  const PoolParams           originalParams = sampleParams();
  PoolRegistrationCert       cert(originalParams);
  const PoolRegistrationCert original(cert);
  PoolParams                 changed = cert.params();

  // Act
  changed.setPledge(20'000);
  cert.setParams(changed);

  // Assert
  EXPECT_EQ(cert.params(), changed);
  EXPECT_EQ(original.params(), originalParams);
  EXPECT_NE(cert, original);
  EXPECT_EQ(PoolRegistrationCert::fromCbor(cert.toCbor()), cert);
}

TEST(PoolRegistrationCert, RejectsMalformedCbor)
{
  // Arrange
  const PoolRegistrationCert cert(sampleParams());
  std::vector<std::uint8_t>  trailing = cert.toCbor();
  trailing.push_back(0);

  // Act & Assert
  EXPECT_THROW(PoolRegistrationCert::fromCbor(trailing), std::invalid_argument);
  EXPECT_THROW(PoolRegistrationCert::fromCbor("z"), std::invalid_argument);
}
