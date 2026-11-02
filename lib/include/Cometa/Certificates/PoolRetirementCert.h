/**
 * @file PoolRetirementCert.h
 *
 * @brief Pool retirement certificates.
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

#include <Cometa/Crypto/Blake2bHash.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_pool_retirement_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A pool retirement certificate.
 */
class PoolRetirementCert
{
  public:

    /**
     * @brief Pool retirement certificate with a pool key hash and epoch.
     *
     * @param poolKeyHash The 28-byte pool key hash to copy.
     * @param epoch The retirement epoch.
     */
    explicit PoolRetirementCert(const Blake2bHash& poolKeyHash, std::uint64_t epoch);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    PoolRetirementCert(const PoolRetirementCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    PoolRetirementCert(PoolRetirementCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~PoolRetirementCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static PoolRetirementCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static PoolRetirementCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the pool key hash.
     *
     * @return An independent pool key hash.
     */
    Blake2bHash poolKeyHash() const;

    /**
     * @brief Replaces the pool key hash.
     *
     * @param value The 28-byte pool key hash to copy.
     */
    void setPoolKeyHash(const Blake2bHash& value);

    /**
     * @brief Gets the retirement epoch.
     *
     * @return The retirement epoch.
     */
    std::uint64_t epoch() const;

    /**
     * @brief Replaces the retirement epoch.
     *
     * @param value The new retirement epoch.
     */
    void setEpoch(std::uint64_t value);

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
    void swap(PoolRetirementCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    PoolRetirementCert& operator=(const PoolRetirementCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    PoolRetirementCert& operator=(PoolRetirementCert&& other) noexcept;

    /**
     * @brief Compares pool key hashes.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const PoolRetirementCert& other) const;

    /**
     * @brief Compares pool key hashes.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const PoolRetirementCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit PoolRetirementCert(cardano_pool_retirement_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_pool_retirement_cert_t* checked() const;

    /**
     * @brief Allows Certificate to copy this variant.
     */
    friend class Certificate;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_pool_retirement_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(PoolRetirementCert& lhs, PoolRetirementCert& rhs) noexcept;
} // namespace Cometa
