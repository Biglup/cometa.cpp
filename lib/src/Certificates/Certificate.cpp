/**
 * @file Certificate.cpp
 *
 * @brief Ledger certificate value variants.
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

#include <Cometa/Certificates/Certificate.h>

#include "../CardanoInterop.h"

#include <cardano/certs/auth_committee_hot_cert.h>
#include <cardano/certs/certificate.h>
#include <cardano/certs/genesis_key_delegation_cert.h>
#include <cardano/certs/mir_cert.h>
#include <cardano/certs/pool_registration_cert.h>
#include <cardano/certs/pool_retirement_cert.h>
#include <cardano/certs/register_drep_cert.h>
#include <cardano/certs/registration_cert.h>
#include <cardano/certs/resign_committee_cold_cert.h>
#include <cardano/certs/stake_delegation_cert.h>
#include <cardano/certs/stake_deregistration_cert.h>
#include <cardano/certs/stake_registration_cert.h>
#include <cardano/certs/stake_registration_delegation_cert.h>
#include <cardano/certs/stake_vote_delegation_cert.h>
#include <cardano/certs/stake_vote_registration_delegation_cert.h>
#include <cardano/certs/unregister_drep_cert.h>
#include <cardano/certs/unregistration_cert.h>
#include <cardano/certs/update_drep_cert.h>
#include <cardano/certs/vote_delegation_cert.h>
#include <cardano/certs/vote_registration_delegation_cert.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
Certificate::Certificate(const MirToPotCert& value)
: Certificate(MirCert(value))
{
}

Certificate::Certificate(const MirToStakeCredentialsCert& value)
: Certificate(MirCert(value))
{
}

Certificate::Certificate(const AuthCommitteeHotCert& value)
: m_value(nullptr)
{
  const AuthCommitteeHotCert                                      copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_auth_committee_hot(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate AuthorizeCommitteeHot");
  m_value = owner.release();
}

Certificate::Certificate(AuthCommitteeHotCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_auth_committee_hot(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate AuthorizeCommitteeHot");
  m_value = owner.release();

  AuthCommitteeHotCert consumed(std::move(value));
}

Certificate::Certificate(const GenesisKeyDelegationCert& value)
: m_value(nullptr)
{
  const GenesisKeyDelegationCert                                  copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_genesis_key_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate GenesisKeyDelegation");
  m_value = owner.release();
}

Certificate::Certificate(GenesisKeyDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_genesis_key_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate GenesisKeyDelegation");
  m_value = owner.release();

  GenesisKeyDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const MirCert& value)
: m_value(nullptr)
{
  const MirCert                                                   copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_mir(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate MoveInstantaneousRewards");
  m_value = owner.release();
}

Certificate::Certificate(MirCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_mir(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate MoveInstantaneousRewards");
  m_value = owner.release();

  MirCert consumed(std::move(value));
}

Certificate::Certificate(const PoolRegistrationCert& value)
: m_value(nullptr)
{
  const PoolRegistrationCert                                      copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_pool_registration(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate PoolRegistration");
  m_value = owner.release();
}

Certificate::Certificate(PoolRegistrationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_pool_registration(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate PoolRegistration");
  m_value = owner.release();

  PoolRegistrationCert consumed(std::move(value));
}

Certificate::Certificate(const PoolRetirementCert& value)
: m_value(nullptr)
{
  const PoolRetirementCert                                        copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_pool_retirement(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate PoolRetirement");
  m_value = owner.release();
}

Certificate::Certificate(PoolRetirementCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_pool_retirement(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate PoolRetirement");
  m_value = owner.release();

  PoolRetirementCert consumed(std::move(value));
}

Certificate::Certificate(const RegisterDRepCert& value)
: m_value(nullptr)
{
  const RegisterDRepCert                                          copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_register_drep(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate DRepRegistration");
  m_value = owner.release();
}

Certificate::Certificate(RegisterDRepCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_register_drep(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate DRepRegistration");
  m_value = owner.release();

  RegisterDRepCert consumed(std::move(value));
}

Certificate::Certificate(const RegistrationCert& value)
: m_value(nullptr)
{
  const RegistrationCert                                          copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_registration(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate Registration");
  m_value = owner.release();
}

Certificate::Certificate(RegistrationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_registration(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate Registration");
  m_value = owner.release();

  RegistrationCert consumed(std::move(value));
}

Certificate::Certificate(const ResignCommitteeColdCert& value)
: m_value(nullptr)
{
  const ResignCommitteeColdCert                                   copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_resign_committee_cold(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate ResignCommitteeCold");
  m_value = owner.release();
}

Certificate::Certificate(ResignCommitteeColdCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_resign_committee_cold(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate ResignCommitteeCold");
  m_value = owner.release();

  ResignCommitteeColdCert consumed(std::move(value));
}

Certificate::Certificate(const StakeDelegationCert& value)
: m_value(nullptr)
{
  const StakeDelegationCert                                       copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeDelegation");
  m_value = owner.release();
}

Certificate::Certificate(StakeDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeDelegation");
  m_value = owner.release();

  StakeDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const StakeDeregistrationCert& value)
: m_value(nullptr)
{
  const StakeDeregistrationCert                                   copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_deregistration(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeDeregistration");
  m_value = owner.release();
}

Certificate::Certificate(StakeDeregistrationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_deregistration(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeDeregistration");
  m_value = owner.release();

  StakeDeregistrationCert consumed(std::move(value));
}

Certificate::Certificate(const StakeRegistrationCert& value)
: m_value(nullptr)
{
  const StakeRegistrationCert                                     copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_registration(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeRegistration");
  m_value = owner.release();
}

Certificate::Certificate(StakeRegistrationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_registration(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeRegistration");
  m_value = owner.release();

  StakeRegistrationCert consumed(std::move(value));
}

Certificate::Certificate(const StakeRegistrationDelegationCert& value)
: m_value(nullptr)
{
  const StakeRegistrationDelegationCert                           copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_registration_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeRegistrationDelegation");
  m_value = owner.release();
}

Certificate::Certificate(StakeRegistrationDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_registration_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeRegistrationDelegation");
  m_value = owner.release();

  StakeRegistrationDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const StakeVoteDelegationCert& value)
: m_value(nullptr)
{
  const StakeVoteDelegationCert                                   copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_vote_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeVoteDelegation");
  m_value = owner.release();
}

Certificate::Certificate(StakeVoteDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_vote_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeVoteDelegation");
  m_value = owner.release();

  StakeVoteDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const StakeVoteRegistrationDelegationCert& value)
: m_value(nullptr)
{
  const StakeVoteRegistrationDelegationCert                       copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_vote_registration_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeVoteRegistrationDelegation");
  m_value = owner.release();
}

Certificate::Certificate(StakeVoteRegistrationDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_stake_vote_registration_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate StakeVoteRegistrationDelegation");
  m_value = owner.release();

  StakeVoteRegistrationDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const UnregisterDRepCert& value)
: m_value(nullptr)
{
  const UnregisterDRepCert                                        copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_unregister_drep(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate DRepUnregistration");
  m_value = owner.release();
}

Certificate::Certificate(UnregisterDRepCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_unregister_drep(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate DRepUnregistration");
  m_value = owner.release();

  UnregisterDRepCert consumed(std::move(value));
}

Certificate::Certificate(const UnregistrationCert& value)
: m_value(nullptr)
{
  const UnregistrationCert                                        copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_unregistration(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate Unregistration");
  m_value = owner.release();
}

Certificate::Certificate(UnregistrationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_unregistration(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate Unregistration");
  m_value = owner.release();

  UnregistrationCert consumed(std::move(value));
}

Certificate::Certificate(const UpdateDRepCert& value)
: m_value(nullptr)
{
  const UpdateDRepCert                                            copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_update_drep(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate UpdateDRep");
  m_value = owner.release();
}

Certificate::Certificate(UpdateDRepCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_update_drep(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate UpdateDRep");
  m_value = owner.release();

  UpdateDRepCert consumed(std::move(value));
}

Certificate::Certificate(const VoteDelegationCert& value)
: m_value(nullptr)
{
  const VoteDelegationCert                                        copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_vote_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate VoteDelegation");
  m_value = owner.release();
}

Certificate::Certificate(VoteDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_vote_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate VoteDelegation");
  m_value = owner.release();

  VoteDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const VoteRegistrationDelegationCert& value)
: m_value(nullptr)
{
  const VoteRegistrationDelegationCert                            copy(value);
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_vote_registration_delegation(copy.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate VoteRegistrationDelegation");
  m_value = owner.release();
}

Certificate::Certificate(VoteRegistrationDelegationCert&& value)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_new_vote_registration_delegation(value.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate VoteRegistrationDelegation");
  m_value = owner.release();

  VoteRegistrationDelegationCert consumed(std::move(value));
}

Certificate::Certificate(const Certificate& other)
: m_value(nullptr)
{
  cardano_certificate_t*                                          native = nullptr;
  const cardano_error_t                                           status = cardano_certificate_deep_clone(other.checked(), &native);
  CardanoObject<cardano_certificate_t, cardano_certificate_unref> owner(native);

  checkCardanoError(status, "Certificate copy");
  m_value = owner.release();
}

Certificate::Certificate(Certificate&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

Certificate::~Certificate()
{
  cardano_certificate_unref(&m_value);
}

Certificate
Certificate::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  Certificate value(decodeCbor<cardano_certificate_t, cardano_certificate_unref>(cbor, cardano_certificate_from_cbor));
  if (static_cast<int>(value.type()) < 0 || static_cast<int>(value.type()) > static_cast<int>(CertificateType::UpdateDRep))
  {
    throw std::invalid_argument("Unknown certificate type");
  }
  return value;
}

Certificate
Certificate::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

CertificateType
Certificate::type() const
{
  cardano_cert_type_t value = CARDANO_CERT_TYPE_STAKE_REGISTRATION;
  checkCardanoError(cardano_cert_get_type(checked(), &value), "Certificate type");
  return static_cast<CertificateType>(value);
}

AuthCommitteeHotCert
Certificate::asAuthCommitteeHot() const
{
  if (type() != CertificateType::AuthorizeCommitteeHot)
  {
    throw std::logic_error("Certificate is not AuthCommitteeHotCert");
  }

  cardano_auth_committee_hot_cert_t*                                                      native = nullptr;
  const cardano_error_t                                                                   status = cardano_certificate_to_auth_committee_hot(checked(), &native);
  CardanoObject<cardano_auth_committee_hot_cert_t, cardano_auth_committee_hot_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asAuthCommitteeHot");
  const AuthCommitteeHotCert value(owner.release());
  return AuthCommitteeHotCert(value);
}

GenesisKeyDelegationCert
Certificate::asGenesisKeyDelegation() const
{
  if (type() != CertificateType::GenesisKeyDelegation)
  {
    throw std::logic_error("Certificate is not GenesisKeyDelegationCert");
  }

  cardano_genesis_key_delegation_cert_t*                                                          native = nullptr;
  const cardano_error_t                                                                           status = cardano_certificate_to_genesis_key_delegation(checked(), &native);
  CardanoObject<cardano_genesis_key_delegation_cert_t, cardano_genesis_key_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asGenesisKeyDelegation");
  const GenesisKeyDelegationCert value(owner.release());
  return GenesisKeyDelegationCert(value);
}

MirCert
Certificate::asMir() const
{
  if (type() != CertificateType::MoveInstantaneousRewards)
  {
    throw std::logic_error("Certificate is not MirCert");
  }

  cardano_mir_cert_t*                                       native = nullptr;
  const cardano_error_t                                     status = cardano_certificate_to_mir(checked(), &native);
  CardanoObject<cardano_mir_cert_t, cardano_mir_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asMir");
  const MirCert value(owner.release());
  return MirCert(value);
}

PoolRegistrationCert
Certificate::asPoolRegistration() const
{
  if (type() != CertificateType::PoolRegistration)
  {
    throw std::logic_error("Certificate is not PoolRegistrationCert");
  }

  cardano_pool_registration_cert_t*                                                     native = nullptr;
  const cardano_error_t                                                                 status = cardano_certificate_to_pool_registration(checked(), &native);
  CardanoObject<cardano_pool_registration_cert_t, cardano_pool_registration_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asPoolRegistration");
  const PoolRegistrationCert value(owner.release());
  return PoolRegistrationCert(value);
}

PoolRetirementCert
Certificate::asPoolRetirement() const
{
  if (type() != CertificateType::PoolRetirement)
  {
    throw std::logic_error("Certificate is not PoolRetirementCert");
  }

  cardano_pool_retirement_cert_t*                                                   native = nullptr;
  const cardano_error_t                                                             status = cardano_certificate_to_pool_retirement(checked(), &native);
  CardanoObject<cardano_pool_retirement_cert_t, cardano_pool_retirement_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asPoolRetirement");
  const PoolRetirementCert value(owner.release());
  return PoolRetirementCert(value);
}

RegisterDRepCert
Certificate::asRegisterDRep() const
{
  if (type() != CertificateType::DRepRegistration)
  {
    throw std::logic_error("Certificate is not RegisterDRepCert");
  }

  cardano_register_drep_cert_t*                                                 native = nullptr;
  const cardano_error_t                                                         status = cardano_certificate_to_register_drep(checked(), &native);
  CardanoObject<cardano_register_drep_cert_t, cardano_register_drep_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asRegisterDRep");
  const RegisterDRepCert value(owner.release());
  return RegisterDRepCert(value);
}

RegistrationCert
Certificate::asRegistration() const
{
  if (type() != CertificateType::Registration)
  {
    throw std::logic_error("Certificate is not RegistrationCert");
  }

  cardano_registration_cert_t*                                                native = nullptr;
  const cardano_error_t                                                       status = cardano_certificate_to_registration(checked(), &native);
  CardanoObject<cardano_registration_cert_t, cardano_registration_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asRegistration");
  const RegistrationCert value(owner.release());
  return RegistrationCert(value);
}

ResignCommitteeColdCert
Certificate::asResignCommitteeCold() const
{
  if (type() != CertificateType::ResignCommitteeCold)
  {
    throw std::logic_error("Certificate is not ResignCommitteeColdCert");
  }

  cardano_resign_committee_cold_cert_t*                                                         native = nullptr;
  const cardano_error_t                                                                         status = cardano_certificate_to_resign_committee_cold(checked(), &native);
  CardanoObject<cardano_resign_committee_cold_cert_t, cardano_resign_committee_cold_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asResignCommitteeCold");
  const ResignCommitteeColdCert value(owner.release());
  return ResignCommitteeColdCert(value);
}

StakeDelegationCert
Certificate::asStakeDelegation() const
{
  if (type() != CertificateType::StakeDelegation)
  {
    throw std::logic_error("Certificate is not StakeDelegationCert");
  }

  cardano_stake_delegation_cert_t*                                                    native = nullptr;
  const cardano_error_t                                                               status = cardano_certificate_to_stake_delegation(checked(), &native);
  CardanoObject<cardano_stake_delegation_cert_t, cardano_stake_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asStakeDelegation");
  const StakeDelegationCert value(owner.release());
  return StakeDelegationCert(value);
}

StakeDeregistrationCert
Certificate::asStakeDeregistration() const
{
  if (type() != CertificateType::StakeDeregistration)
  {
    throw std::logic_error("Certificate is not StakeDeregistrationCert");
  }

  cardano_stake_deregistration_cert_t*                                                        native = nullptr;
  const cardano_error_t                                                                       status = cardano_certificate_to_stake_deregistration(checked(), &native);
  CardanoObject<cardano_stake_deregistration_cert_t, cardano_stake_deregistration_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asStakeDeregistration");
  const StakeDeregistrationCert value(owner.release());
  return StakeDeregistrationCert(value);
}

StakeRegistrationCert
Certificate::asStakeRegistration() const
{
  if (type() != CertificateType::StakeRegistration)
  {
    throw std::logic_error("Certificate is not StakeRegistrationCert");
  }

  cardano_stake_registration_cert_t*                                                      native = nullptr;
  const cardano_error_t                                                                   status = cardano_certificate_to_stake_registration(checked(), &native);
  CardanoObject<cardano_stake_registration_cert_t, cardano_stake_registration_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asStakeRegistration");
  const StakeRegistrationCert value(owner.release());
  return StakeRegistrationCert(value);
}

StakeRegistrationDelegationCert
Certificate::asStakeRegistrationDelegation() const
{
  if (type() != CertificateType::StakeRegistrationDelegation)
  {
    throw std::logic_error("Certificate is not StakeRegistrationDelegationCert");
  }

  cardano_stake_registration_delegation_cert_t*                                                                 native = nullptr;
  const cardano_error_t                                                                                         status = cardano_certificate_to_stake_registration_delegation(checked(), &native);
  CardanoObject<cardano_stake_registration_delegation_cert_t, cardano_stake_registration_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asStakeRegistrationDelegation");
  const StakeRegistrationDelegationCert value(owner.release());
  return StakeRegistrationDelegationCert(value);
}

StakeVoteDelegationCert
Certificate::asStakeVoteDelegation() const
{
  if (type() != CertificateType::StakeVoteDelegation)
  {
    throw std::logic_error("Certificate is not StakeVoteDelegationCert");
  }

  cardano_stake_vote_delegation_cert_t*                                                         native = nullptr;
  const cardano_error_t                                                                         status = cardano_certificate_to_stake_vote_delegation(checked(), &native);
  CardanoObject<cardano_stake_vote_delegation_cert_t, cardano_stake_vote_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asStakeVoteDelegation");
  const StakeVoteDelegationCert value(owner.release());
  return StakeVoteDelegationCert(value);
}

StakeVoteRegistrationDelegationCert
Certificate::asStakeVoteRegistrationDelegation() const
{
  if (type() != CertificateType::StakeVoteRegistrationDelegation)
  {
    throw std::logic_error("Certificate is not StakeVoteRegistrationDelegationCert");
  }

  cardano_stake_vote_registration_delegation_cert_t*                                                                      native = nullptr;
  const cardano_error_t                                                                                                   status = cardano_certificate_to_stake_vote_registration_delegation(checked(), &native);
  CardanoObject<cardano_stake_vote_registration_delegation_cert_t, cardano_stake_vote_registration_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asStakeVoteRegistrationDelegation");
  const StakeVoteRegistrationDelegationCert value(owner.release());
  return StakeVoteRegistrationDelegationCert(value);
}

UnregisterDRepCert
Certificate::asUnregisterDRep() const
{
  if (type() != CertificateType::DRepUnregistration)
  {
    throw std::logic_error("Certificate is not UnregisterDRepCert");
  }

  cardano_unregister_drep_cert_t*                                                   native = nullptr;
  const cardano_error_t                                                             status = cardano_certificate_to_unregister_drep(checked(), &native);
  CardanoObject<cardano_unregister_drep_cert_t, cardano_unregister_drep_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asUnregisterDRep");
  const UnregisterDRepCert value(owner.release());
  return UnregisterDRepCert(value);
}

UnregistrationCert
Certificate::asUnregistration() const
{
  if (type() != CertificateType::Unregistration)
  {
    throw std::logic_error("Certificate is not UnregistrationCert");
  }

  cardano_unregistration_cert_t*                                                  native = nullptr;
  const cardano_error_t                                                           status = cardano_certificate_to_unregistration(checked(), &native);
  CardanoObject<cardano_unregistration_cert_t, cardano_unregistration_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asUnregistration");
  const UnregistrationCert value(owner.release());
  return UnregistrationCert(value);
}

UpdateDRepCert
Certificate::asUpdateDRep() const
{
  if (type() != CertificateType::UpdateDRep)
  {
    throw std::logic_error("Certificate is not UpdateDRepCert");
  }

  cardano_update_drep_cert_t*                                               native = nullptr;
  const cardano_error_t                                                     status = cardano_certificate_to_update_drep(checked(), &native);
  CardanoObject<cardano_update_drep_cert_t, cardano_update_drep_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asUpdateDRep");
  const UpdateDRepCert value(owner.release());
  return UpdateDRepCert(value);
}

VoteDelegationCert
Certificate::asVoteDelegation() const
{
  if (type() != CertificateType::VoteDelegation)
  {
    throw std::logic_error("Certificate is not VoteDelegationCert");
  }

  cardano_vote_delegation_cert_t*                                                   native = nullptr;
  const cardano_error_t                                                             status = cardano_certificate_to_vote_delegation(checked(), &native);
  CardanoObject<cardano_vote_delegation_cert_t, cardano_vote_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asVoteDelegation");
  const VoteDelegationCert value(owner.release());
  return VoteDelegationCert(value);
}

VoteRegistrationDelegationCert
Certificate::asVoteRegistrationDelegation() const
{
  if (type() != CertificateType::VoteRegistrationDelegation)
  {
    throw std::logic_error("Certificate is not VoteRegistrationDelegationCert");
  }

  cardano_vote_registration_delegation_cert_t*                                                                native = nullptr;
  const cardano_error_t                                                                                       status = cardano_certificate_to_vote_registration_delegation(checked(), &native);
  CardanoObject<cardano_vote_registration_delegation_cert_t, cardano_vote_registration_delegation_cert_unref> owner(native);

  checkCardanoError(status, "Certificate asVoteRegistrationDelegation");
  const VoteRegistrationDelegationCert value(owner.release());
  return VoteRegistrationDelegationCert(value);
}

std::vector<std::uint8_t>
Certificate::toCbor() const
{
  return encodeCbor(checked(), cardano_certificate_to_cbor);
}

std::string
Certificate::toJson() const
{
  return encodeJson(checked(), cardano_certificate_to_cip116_json);
}

void
Certificate::swap(Certificate& other) noexcept
{
  std::swap(m_value, other.m_value);
}

Certificate&
Certificate::operator=(const Certificate& other)
{
  if (this != &other)
  {
    Certificate copy(other);
    swap(copy);
  }
  return *this;
}

Certificate&
Certificate::operator=(Certificate&& other) noexcept
{
  if (this != &other)
  {
    cardano_certificate_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
Certificate::operator==(const Certificate& other) const
{
  return type() == other.type() && toCbor() == other.toCbor();
}

bool
Certificate::operator!=(const Certificate& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

Certificate::Certificate(cardano_certificate_t* value) noexcept
: m_value(value)
{
}

cardano_certificate_t*
Certificate::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from Certificate");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(Certificate& lhs, Certificate& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
