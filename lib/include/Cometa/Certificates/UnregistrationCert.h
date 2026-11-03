/**
 * @file UnregistrationCert.h
 *
 * @brief Deregisters a staking credential with an explicit deposit refund.
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

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_unregistration_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Deregisters a staking credential with an explicit deposit refund.
 *
 * The refund is in lovelace. Ledger validation checks registration and reward-account state before
 * accepting the deregistration.
 */
class UnregistrationCert
{
  public:

    /**
     * @brief Unregistration certificate with a stake credential and deposit.
     *
     * @param credential The stake credential to copy.
     * @param deposit The deposit in lovelace.
     */
    explicit UnregistrationCert(const Credential& credential, std::uint64_t deposit);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    UnregistrationCert(const UnregistrationCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    UnregistrationCert(UnregistrationCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~UnregistrationCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static UnregistrationCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static UnregistrationCert fromCbor(std::string_view cborHex);

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
     * @brief Gets the deposit amount.
     *
     * @return The deposit in lovelace.
     */
    std::uint64_t deposit() const;

    /**
     * @brief Replaces the deposit amount.
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
    void swap(UnregistrationCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    UnregistrationCert& operator=(const UnregistrationCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    UnregistrationCert& operator=(UnregistrationCert&& other) noexcept;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const UnregistrationCert& other) const;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const UnregistrationCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit UnregistrationCert(cardano_unregistration_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_unregistration_cert_t* checked() const;

    /**
     * @brief Allows Certificate to copy this variant.
     */
    friend class Certificate;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_unregistration_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(UnregistrationCert& lhs, UnregistrationCert& rhs) noexcept;
} // namespace Cometa
