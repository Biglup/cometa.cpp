/**
 * @file PoolRegistrationCert.h
 *
 * @brief Registers a stake pool or updates its parameters.
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

#include <Cometa/PoolParams/PoolParams.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_pool_registration_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Registers a stake pool or updates its parameters.
 *
 * Carries PoolParams, including operator and VRF hashes, reward terms, owners, and relays. Whether a
 * deposit is required depends on the pool's ledger registration state.
 */
class PoolRegistrationCert
{
  public:

    /**
     * @brief Registers a stake pool.
     *
     * @param params The pool parameters to copy.
     */
    explicit PoolRegistrationCert(const PoolParams& params);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    PoolRegistrationCert(const PoolRegistrationCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    PoolRegistrationCert(PoolRegistrationCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~PoolRegistrationCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static PoolRegistrationCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static PoolRegistrationCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the pool parameters.
     *
     * @return An independent value.
     */
    PoolParams params() const;

    /**
     * @brief Replaces the pool parameters.
     *
     * @param value The parameters to copy.
     */
    void setParams(const PoolParams& value);

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
    void swap(PoolRegistrationCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    PoolRegistrationCert& operator=(const PoolRegistrationCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    PoolRegistrationCert& operator=(PoolRegistrationCert&& other) noexcept;

    /**
     * @brief Compares pool parameters.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const PoolRegistrationCert& other) const;

    /**
     * @brief Compares pool parameters.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const PoolRegistrationCert& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static PoolRegistrationCert adopt(cardano_pool_registration_cert_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_pool_registration_cert_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_pool_registration_cert_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_pool_registration_cert_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit PoolRegistrationCert(cardano_pool_registration_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_pool_registration_cert_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_pool_registration_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(PoolRegistrationCert& lhs, PoolRegistrationCert& rhs) noexcept;
} // namespace Cometa
