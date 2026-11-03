/**
 * @file StakeRegistrationDelegationCert.h
 *
 * @brief Registers a staking credential and delegates it to a pool in one certificate.
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

#include <Cometa/Common/Credential.h>
#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_stake_registration_delegation_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Registers a staking credential and delegates it to a pool in one certificate.
 *
 * Includes the registration deposit in lovelace. Use this form when the credential is not yet
 * registered and pool delegation is required immediately.
 */
class StakeRegistrationDelegationCert
{
  public:

    /**
     * @brief Delegates a stake credential to a pool.
     *
     * @param credential The stake credential to copy.
     * @param poolKeyHash The 28-byte pool key hash to copy.
     * @param deposit The stake registration deposit in lovelace.
     */
    StakeRegistrationDelegationCert(const Credential& credential, const Blake2bHash& poolKeyHash, std::uint64_t deposit);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    StakeRegistrationDelegationCert(const StakeRegistrationDelegationCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    StakeRegistrationDelegationCert(StakeRegistrationDelegationCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~StakeRegistrationDelegationCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static StakeRegistrationDelegationCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static StakeRegistrationDelegationCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the stake credential.
     *
     * @return An independent credential.
     */
    Credential credential() const;

    /**
     * @brief Replaces the stake credential.
     *
     * @param value The credential to copy.
     */
    void setCredential(const Credential& value);

    /**
     * @brief Gets the pool key hash.
     *
     * @return An independent 28-byte hash.
     */
    Blake2bHash poolKeyHash() const;

    /**
     * @brief Replaces the pool key hash.
     *
     * @param value The 28-byte pool key hash to copy.
     */
    void setPoolKeyHash(const Blake2bHash& value);

    /**
     * @brief Gets the registration deposit.
     *
     * @return The deposit in lovelace.
     */
    std::uint64_t deposit() const;

    /**
     * @brief Replaces the registration deposit.
     *
     * @param value The new deposit in lovelace.
     */
    void setDeposit(std::uint64_t value);

    /**
     * @brief Encodes the certificate as CBOR.
     *
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the certificate as CIP-116 JSON.
     *
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges certificates.
     *
     * @param other The value to exchange.
     */
    void swap(StakeRegistrationDelegationCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    StakeRegistrationDelegationCert& operator=(const StakeRegistrationDelegationCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    StakeRegistrationDelegationCert& operator=(StakeRegistrationDelegationCert&& other) noexcept;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const StakeRegistrationDelegationCert& other) const;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const StakeRegistrationDelegationCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit StakeRegistrationDelegationCert(cardano_stake_registration_delegation_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_stake_registration_delegation_cert_t* checked() const;

    /**
     * @brief Allows Certificate to copy this variant.
     */
    friend class Certificate;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_stake_registration_delegation_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(StakeRegistrationDelegationCert& lhs, StakeRegistrationDelegationCert& rhs) noexcept;
} // namespace Cometa
