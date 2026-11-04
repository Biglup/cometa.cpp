/**
 * @file RegisterDRepCert.h
 *
 * @brief Registers a DRep credential with a deposit and optional metadata anchor.
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

#include <Cometa/Common/Anchor.h>
#include <Cometa/Common/Credential.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_register_drep_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Registers a DRep credential with a deposit and optional metadata anchor.
 *
 * The deposit is in lovelace. The anchor describes the representative; authorization comes from the
 * DRep's key or script credential.
 */
class RegisterDRepCert
{
  public:

    /**
     * @brief Registers a DRep credential with a deposit.
     *
     * @param credential The DRep credential to copy.
     * @param deposit The deposit in lovelace.
     * @param anchor Optional DRep metadata anchor.
     */
    explicit RegisterDRepCert(const Credential& credential, std::uint64_t deposit, std::optional<Anchor> anchor = std::nullopt);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    RegisterDRepCert(const RegisterDRepCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    RegisterDRepCert(RegisterDRepCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~RegisterDRepCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static RegisterDRepCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static RegisterDRepCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the DRep credential.
     *
     * @return An independent credential.
     */
    Credential credential() const;

    /**
     * @brief Replaces the DRep credential.
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
     * @brief Gets the DRep metadata anchor, if present.
     *
     * @return An independent anchor or no value.
     */
    std::optional<Anchor> anchor() const;

    /**
     * @brief Replaces or removes the metadata anchor.
     *
     * @param value The new anchor, or no value to remove it.
     */
    void setAnchor(std::optional<Anchor> value);

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
    void swap(RegisterDRepCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    RegisterDRepCert& operator=(const RegisterDRepCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    RegisterDRepCert& operator=(RegisterDRepCert&& other) noexcept;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const RegisterDRepCert& other) const;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const RegisterDRepCert& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static RegisterDRepCert adopt(cardano_register_drep_cert_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_register_drep_cert_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_register_drep_cert_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_register_drep_cert_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit RegisterDRepCert(cardano_register_drep_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_register_drep_cert_t* checked() const;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_register_drep_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(RegisterDRepCert& lhs, RegisterDRepCert& rhs) noexcept;
} // namespace Cometa
