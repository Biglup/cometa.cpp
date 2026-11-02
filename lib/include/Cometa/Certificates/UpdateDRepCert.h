/**
 * @file UpdateDRepCert.h
 *
 * @brief DRep metadata update certificates.
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

struct cardano_update_drep_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A DRep metadata update certificate.
 */
class UpdateDRepCert
{
  public:

    /**
     * @brief Updates the DRep metadata anchor.
     *
     * @param credential The DRep credential to copy.
     * @param anchor Optional DRep metadata anchor.
     */
    explicit UpdateDRepCert(const Credential& credential, std::optional<Anchor> anchor = std::nullopt);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    UpdateDRepCert(const UpdateDRepCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     */
    UpdateDRepCert(UpdateDRepCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~UpdateDRepCert();

  public:

    /**
     * @brief Decodes exactly one CBOR certificate.
     *
     * @param cbor The complete encoded certificate.
     *
     * @return The decoded certificate.
     */
    static UpdateDRepCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static UpdateDRepCert fromCbor(std::string_view cborHex);

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
    void swap(UpdateDRepCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    UpdateDRepCert& operator=(const UpdateDRepCert& other);

    /**
     * @brief Moves another certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    UpdateDRepCert& operator=(UpdateDRepCert&& other) noexcept;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const UpdateDRepCert& other) const;

    /**
     * @brief Compares credentials.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const UpdateDRepCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit UpdateDRepCert(cardano_update_drep_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_update_drep_cert_t* checked() const;

    /**
     * @brief Allows Certificate to copy this variant.
     */
    friend class Certificate;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_update_drep_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(UpdateDRepCert& lhs, UpdateDRepCert& rhs) noexcept;
} // namespace Cometa
