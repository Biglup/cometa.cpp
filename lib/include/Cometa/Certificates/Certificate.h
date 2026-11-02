/**
 * @file Certificate.h
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Certificates/AuthCommitteeHotCert.h>
#include <Cometa/Certificates/CertificateType.h>
#include <Cometa/Certificates/GenesisKeyDelegationCert.h>
#include <Cometa/Certificates/MirCert.h>
#include <Cometa/Certificates/PoolRegistrationCert.h>
#include <Cometa/Certificates/PoolRetirementCert.h>
#include <Cometa/Certificates/RegisterDRepCert.h>
#include <Cometa/Certificates/RegistrationCert.h>
#include <Cometa/Certificates/ResignCommitteeColdCert.h>
#include <Cometa/Certificates/StakeDelegationCert.h>
#include <Cometa/Certificates/StakeDeregistrationCert.h>
#include <Cometa/Certificates/StakeRegistrationCert.h>
#include <Cometa/Certificates/StakeRegistrationDelegationCert.h>
#include <Cometa/Certificates/StakeVoteDelegationCert.h>
#include <Cometa/Certificates/StakeVoteRegistrationDelegationCert.h>
#include <Cometa/Certificates/UnregisterDRepCert.h>
#include <Cometa/Certificates/UnregistrationCert.h>
#include <Cometa/Certificates/UpdateDRepCert.h>
#include <Cometa/Certificates/VoteDelegationCert.h>
#include <Cometa/Certificates/VoteRegistrationDelegationCert.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_certificate_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A ledger certificate containing one typed certificate value.
 */
class Certificate
{
  public:

    /**
     * @brief Creates a certificate from AuthCommitteeHotCert.
     * @param value The value to copy.
     */
    explicit Certificate(const AuthCommitteeHotCert& value);

    /**
     * @brief Creates a certificate from GenesisKeyDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const GenesisKeyDelegationCert& value);

    /**
     * @brief Creates a certificate from MirCert.
     * @param value The value to copy.
     */
    explicit Certificate(const MirCert& value);

    /**
     * @brief Creates a certificate from PoolRegistrationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const PoolRegistrationCert& value);

    /**
     * @brief Creates a certificate from PoolRetirementCert.
     * @param value The value to copy.
     */
    explicit Certificate(const PoolRetirementCert& value);

    /**
     * @brief Creates a certificate from RegisterDRepCert.
     * @param value The value to copy.
     */
    explicit Certificate(const RegisterDRepCert& value);

    /**
     * @brief Creates a certificate from RegistrationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const RegistrationCert& value);

    /**
     * @brief Creates a certificate from ResignCommitteeColdCert.
     * @param value The value to copy.
     */
    explicit Certificate(const ResignCommitteeColdCert& value);

    /**
     * @brief Creates a certificate from StakeDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const StakeDelegationCert& value);

    /**
     * @brief Creates a certificate from StakeDeregistrationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const StakeDeregistrationCert& value);

    /**
     * @brief Creates a certificate from StakeRegistrationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const StakeRegistrationCert& value);

    /**
     * @brief Creates a certificate from StakeRegistrationDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const StakeRegistrationDelegationCert& value);

    /**
     * @brief Creates a certificate from StakeVoteDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const StakeVoteDelegationCert& value);

    /**
     * @brief Creates a certificate from StakeVoteRegistrationDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const StakeVoteRegistrationDelegationCert& value);

    /**
     * @brief Creates a certificate from UnregisterDRepCert.
     * @param value The value to copy.
     */
    explicit Certificate(const UnregisterDRepCert& value);

    /**
     * @brief Creates a certificate from UnregistrationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const UnregistrationCert& value);

    /**
     * @brief Creates a certificate from UpdateDRepCert.
     * @param value The value to copy.
     */
    explicit Certificate(const UpdateDRepCert& value);

    /**
     * @brief Creates a certificate from VoteDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const VoteDelegationCert& value);

    /**
     * @brief Creates a certificate from VoteRegistrationDelegationCert.
     * @param value The value to copy.
     */
    explicit Certificate(const VoteRegistrationDelegationCert& value);

    /**
     * @brief Copies a certificate.
     * @param other The certificate to copy.
     */
    Certificate(const Certificate& other);

    /**
     * @brief Moves a certificate.
     * @param other The certificate to move.
     */
    Certificate(Certificate&& other) noexcept;

    /** @brief Destroys the certificate. */
    ~Certificate();

  public:

    /**
     * @brief Decodes one CBOR certificate.
     * @param cbor The encoded certificate.
     * @return The decoded value.
     */
    static Certificate fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded value.
     */
    static Certificate fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the certificate kind.
     * @return The kind.
     */
    CertificateType type() const;

    /**
     * @brief Gets the AuthCommitteeHotCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    AuthCommitteeHotCert asAuthCommitteeHot() const;

    /**
     * @brief Gets the GenesisKeyDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    GenesisKeyDelegationCert asGenesisKeyDelegation() const;

    /**
     * @brief Gets the MirCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    MirCert asMir() const;

    /**
     * @brief Gets the PoolRegistrationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    PoolRegistrationCert asPoolRegistration() const;

    /**
     * @brief Gets the PoolRetirementCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    PoolRetirementCert asPoolRetirement() const;

    /**
     * @brief Gets the RegisterDRepCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    RegisterDRepCert asRegisterDRep() const;

    /**
     * @brief Gets the RegistrationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    RegistrationCert asRegistration() const;

    /**
     * @brief Gets the ResignCommitteeColdCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    ResignCommitteeColdCert asResignCommitteeCold() const;

    /**
     * @brief Gets the StakeDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    StakeDelegationCert asStakeDelegation() const;

    /**
     * @brief Gets the StakeDeregistrationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    StakeDeregistrationCert asStakeDeregistration() const;

    /**
     * @brief Gets the StakeRegistrationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    StakeRegistrationCert asStakeRegistration() const;

    /**
     * @brief Gets the StakeRegistrationDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    StakeRegistrationDelegationCert asStakeRegistrationDelegation() const;

    /**
     * @brief Gets the StakeVoteDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    StakeVoteDelegationCert asStakeVoteDelegation() const;

    /**
     * @brief Gets the StakeVoteRegistrationDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    StakeVoteRegistrationDelegationCert asStakeVoteRegistrationDelegation() const;

    /**
     * @brief Gets the UnregisterDRepCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    UnregisterDRepCert asUnregisterDRep() const;

    /**
     * @brief Gets the UnregistrationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    UnregistrationCert asUnregistration() const;

    /**
     * @brief Gets the UpdateDRepCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    UpdateDRepCert asUpdateDRep() const;

    /**
     * @brief Gets the VoteDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    VoteDelegationCert asVoteDelegation() const;

    /**
     * @brief Gets the VoteRegistrationDelegationCert value.
     * @return An independent value.
     * @throws std::logic_error The certificate has another kind.
     */
    VoteRegistrationDelegationCert asVoteRegistrationDelegation() const;

    /**
     * @brief Encodes the certificate as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the certificate as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges certificates.
     * @param other The certificate to exchange.
     */
    void swap(Certificate& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The certificate to copy.
     * @return This certificate.
     */
    Certificate& operator=(const Certificate& other);

    /**
     * @brief Assigns a moved certificate.
     * @param other The certificate to move.
     * @return This certificate.
     */
    Certificate& operator=(Certificate&& other) noexcept;

    /**
     * @brief Compares certificates.
     * @param other The certificate to compare.
     * @return True when equal.
     */
    bool operator==(const Certificate& other) const;

    /**
     * @brief Compares certificates.
     * @param other The certificate to compare.
     * @return True when different.
     */
    bool operator!=(const Certificate& other) const;

  private:

    /** @brief Allows transactions to use this value. */
    friend class Transaction;

    /**
     * @brief Adopts an owned certificate.
     * @param value The owned certificate.
     */
    explicit Certificate(cardano_certificate_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     * @return The borrowed certificate.
     */
    cardano_certificate_t* checked() const;

    /** @brief Allows certificate collections to copy native values. */
    friend class CertificateSet;

  private:

    /** @brief The owned certificate. */
    cardano_certificate_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 * @param lhs The first certificate.
 * @param rhs The second certificate.
 */
void swap(Certificate& lhs, Certificate& rhs) noexcept;
} // namespace Cometa
