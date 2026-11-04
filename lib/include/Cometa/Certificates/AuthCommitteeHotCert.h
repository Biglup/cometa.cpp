/**
 * @file AuthCommitteeHotCert.h
 *
 * @brief Authorizes a hot credential to vote for a constitutional committee member.
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

struct cardano_auth_committee_hot_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Authorizes a hot credential to vote for a constitutional committee member.
 *
 * The cold credential identifies the member; the hot credential authorizes its routine voting. A
 * transaction carrying this certificate must satisfy the cold credential's authorization.
 */
class AuthCommitteeHotCert
{
  public:

    /**
     * @brief Authorizes a committee hot credential with a cold credential.
     *
     * @param coldCredential The cold credential to copy.
     * @param hotCredential The hot credential to copy.
     */
    AuthCommitteeHotCert(const Credential& coldCredential, const Credential& hotCredential);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    AuthCommitteeHotCert(const AuthCommitteeHotCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    AuthCommitteeHotCert(AuthCommitteeHotCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~AuthCommitteeHotCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static AuthCommitteeHotCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static AuthCommitteeHotCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the committee cold credential.
     *
     * @return An independent credential.
     */
    Credential coldCredential() const;

    /**
     * @brief Replaces the committee cold credential.
     *
     * @param value The credential to copy.
     */
    void setColdCredential(const Credential& value);

    /**
     * @brief Gets the committee hot credential.
     *
     * @return An independent credential.
     */
    Credential hotCredential() const;

    /**
     * @brief Replaces the committee hot credential.
     *
     * @param value The credential to copy.
     */
    void setHotCredential(const Credential& value);

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
    void swap(AuthCommitteeHotCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    AuthCommitteeHotCert& operator=(const AuthCommitteeHotCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    AuthCommitteeHotCert& operator=(AuthCommitteeHotCert&& other) noexcept;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const AuthCommitteeHotCert& other) const;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const AuthCommitteeHotCert& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static AuthCommitteeHotCert adopt(cardano_auth_committee_hot_cert_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_auth_committee_hot_cert_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_auth_committee_hot_cert_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_auth_committee_hot_cert_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit AuthCommitteeHotCert(cardano_auth_committee_hot_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_auth_committee_hot_cert_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_auth_committee_hot_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(AuthCommitteeHotCert& lhs, AuthCommitteeHotCert& rhs) noexcept;
} // namespace Cometa
