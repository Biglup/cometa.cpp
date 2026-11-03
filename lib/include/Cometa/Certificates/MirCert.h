/**
 * @file MirCert.h
 *
 * @brief A historical move-instantaneous-rewards certificate.
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

#include <Cometa/Certificates/MirCertType.h>
#include <Cometa/Certificates/MirToPotCert.h>
#include <Cometa/Certificates/MirToStakeCredentialsCert.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_mir_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A historical move-instantaneous-rewards certificate.
 *
 * Contains either a transfer between accounting pots or a distribution to staking credentials. Use it
 * to inspect historical transactions; applicability depends on the ledger era.
 */
class MirCert
{
  public:

    /**
     * @brief Wraps a transfer between reserve and treasury pots.
     * @param value The transfer to copy.
     */
    MirCert(const MirToPotCert& value);

    /**
     * @brief Initializes a value by moving its MirToPotCert alternative.
     * @param value The alternative to move.
     */
    MirCert(MirToPotCert&& value);

    /**
     * @brief Wraps rewards to stake credentials.
     * @param value The distribution to copy.
     */
    MirCert(const MirToStakeCredentialsCert& value);

    /**
     * @brief Initializes a value by moving its MirToStakeCredentialsCert alternative.
     * @param value The alternative to move.
     */
    MirCert(MirToStakeCredentialsCert&& value);

    /**
     * @brief Copies a certificate.
     * @param other The certificate to copy.
     */
    MirCert(const MirCert& other);

    /**
     * @brief Moves a certificate.
     * @param other The certificate to move.
     */
    MirCert(MirCert&& other) noexcept;

    /** @brief Destroys the certificate. */
    ~MirCert();

  public:

    /**
     * @brief Decodes one CBOR certificate.
     * @param cbor The encoded certificate.
     * @return The decoded certificate.
     */
    static MirCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded certificate.
     */
    static MirCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the certificate variant.
     * @return The variant discriminator.
     */
    MirCertType type() const;

    /**
     * @brief Gets the transfer between pots.
     * @return The transfer value.
     * @throws std::logic_error The certificate contains another variant.
     */
    MirToPotCert asToPot() const;

    /**
     * @brief Gets rewards to stake credentials.
     * @return The distribution value.
     * @throws std::logic_error The certificate contains another variant.
     */
    MirToStakeCredentialsCert asToStakeCredentials() const;

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
    void swap(MirCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The certificate to copy.
     * @return This certificate.
     */
    MirCert& operator=(const MirCert& other);

    /**
     * @brief Assigns a moved certificate.
     * @param other The certificate to move.
     * @return This certificate.
     */
    MirCert& operator=(MirCert&& other) noexcept;

    /**
     * @brief Compares certificates.
     * @param other The certificate to compare.
     * @return True when equal.
     */
    bool operator==(const MirCert& other) const;

    /**
     * @brief Compares certificates.
     * @param other The certificate to compare.
     * @return True when different.
     */
    bool operator!=(const MirCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     * @param value The owned certificate.
     */
    explicit MirCert(cardano_mir_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     * @return The borrowed certificate.
     */
    cardano_mir_cert_t* checked() const;

    /** @brief Allows the outer certificate variant to copy this value. */
    friend class Certificate;

  private:

    /** @brief The owned certificate. */
    cardano_mir_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 * @param lhs The first certificate.
 * @param rhs The second certificate.
 */
void swap(MirCert& lhs, MirCert& rhs) noexcept;
} // namespace Cometa
