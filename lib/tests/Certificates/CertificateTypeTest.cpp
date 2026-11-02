/**
 * @file CertificateTypeTest.cpp
 *
 * @brief Certificate discriminators.
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

#include <cardano/certs/cert_type.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(CertificateType, MatchesNativeDiscriminatorsAndNames)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(CertificateType::StakeRegistration), CARDANO_CERT_TYPE_STAKE_REGISTRATION);
  EXPECT_EQ(toString(CertificateType::StakeRegistration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_STAKE_REGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::StakeDeregistration), CARDANO_CERT_TYPE_STAKE_DEREGISTRATION);
  EXPECT_EQ(toString(CertificateType::StakeDeregistration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_STAKE_DEREGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::StakeDelegation), CARDANO_CERT_TYPE_STAKE_DELEGATION);
  EXPECT_EQ(toString(CertificateType::StakeDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_STAKE_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::PoolRegistration), CARDANO_CERT_TYPE_POOL_REGISTRATION);
  EXPECT_EQ(toString(CertificateType::PoolRegistration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_POOL_REGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::PoolRetirement), CARDANO_CERT_TYPE_POOL_RETIREMENT);
  EXPECT_EQ(toString(CertificateType::PoolRetirement), cardano_cert_type_to_string(CARDANO_CERT_TYPE_POOL_RETIREMENT));
  EXPECT_EQ(static_cast<int>(CertificateType::GenesisKeyDelegation), CARDANO_CERT_TYPE_GENESIS_KEY_DELEGATION);
  EXPECT_EQ(toString(CertificateType::GenesisKeyDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_GENESIS_KEY_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::MoveInstantaneousRewards), CARDANO_CERT_TYPE_MOVE_INSTANTANEOUS_REWARDS);
  EXPECT_EQ(toString(CertificateType::MoveInstantaneousRewards), cardano_cert_type_to_string(CARDANO_CERT_TYPE_MOVE_INSTANTANEOUS_REWARDS));
  EXPECT_EQ(static_cast<int>(CertificateType::Registration), CARDANO_CERT_TYPE_REGISTRATION);
  EXPECT_EQ(toString(CertificateType::Registration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_REGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::Unregistration), CARDANO_CERT_TYPE_UNREGISTRATION);
  EXPECT_EQ(toString(CertificateType::Unregistration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_UNREGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::VoteDelegation), CARDANO_CERT_TYPE_VOTE_DELEGATION);
  EXPECT_EQ(toString(CertificateType::VoteDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_VOTE_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::StakeVoteDelegation), CARDANO_CERT_TYPE_STAKE_VOTE_DELEGATION);
  EXPECT_EQ(toString(CertificateType::StakeVoteDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_STAKE_VOTE_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::StakeRegistrationDelegation), CARDANO_CERT_TYPE_STAKE_REGISTRATION_DELEGATION);
  EXPECT_EQ(toString(CertificateType::StakeRegistrationDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_STAKE_REGISTRATION_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::VoteRegistrationDelegation), CARDANO_CERT_TYPE_VOTE_REGISTRATION_DELEGATION);
  EXPECT_EQ(toString(CertificateType::VoteRegistrationDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_VOTE_REGISTRATION_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::StakeVoteRegistrationDelegation), CARDANO_CERT_TYPE_STAKE_VOTE_REGISTRATION_DELEGATION);
  EXPECT_EQ(toString(CertificateType::StakeVoteRegistrationDelegation), cardano_cert_type_to_string(CARDANO_CERT_TYPE_STAKE_VOTE_REGISTRATION_DELEGATION));
  EXPECT_EQ(static_cast<int>(CertificateType::AuthorizeCommitteeHot), CARDANO_CERT_TYPE_AUTH_COMMITTEE_HOT);
  EXPECT_EQ(toString(CertificateType::AuthorizeCommitteeHot), cardano_cert_type_to_string(CARDANO_CERT_TYPE_AUTH_COMMITTEE_HOT));
  EXPECT_EQ(static_cast<int>(CertificateType::ResignCommitteeCold), CARDANO_CERT_TYPE_RESIGN_COMMITTEE_COLD);
  EXPECT_EQ(toString(CertificateType::ResignCommitteeCold), cardano_cert_type_to_string(CARDANO_CERT_TYPE_RESIGN_COMMITTEE_COLD));
  EXPECT_EQ(static_cast<int>(CertificateType::DRepRegistration), CARDANO_CERT_TYPE_DREP_REGISTRATION);
  EXPECT_EQ(toString(CertificateType::DRepRegistration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_DREP_REGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::DRepUnregistration), CARDANO_CERT_TYPE_DREP_UNREGISTRATION);
  EXPECT_EQ(toString(CertificateType::DRepUnregistration), cardano_cert_type_to_string(CARDANO_CERT_TYPE_DREP_UNREGISTRATION));
  EXPECT_EQ(static_cast<int>(CertificateType::UpdateDRep), CARDANO_CERT_TYPE_UPDATE_DREP);
  EXPECT_EQ(toString(CertificateType::UpdateDRep), cardano_cert_type_to_string(CARDANO_CERT_TYPE_UPDATE_DREP));
}
