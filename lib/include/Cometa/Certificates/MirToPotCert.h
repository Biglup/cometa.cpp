/**
 * @file MirToPotCert.h
 *
 * @brief Historical MIR certificate values.
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

#include <Cometa/Certificates/MirPotType.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_mir_to_pot_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A historical transfer between reserve and treasury accounting pots.
 */
class MirToPotCert
{
  public:

    /**
     * @brief Initializes a pot transfer.
     *
     * @param pot The source pot.
     * @param amount The amount in lovelace.
     */
    MirToPotCert(MirPotType pot, std::uint64_t amount);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    MirToPotCert(const MirToPotCert& other);

    /**
     * @brief Moves a certificate.
     *
     * @param other The source certificate.
     */
    MirToPotCert(MirToPotCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~MirToPotCert();

  public:

    /**
     * @brief Decodes one CBOR certificate.
     *
     * @param cbor The complete encoded value.
     *
     * @return The decoded certificate.
     */
    static MirToPotCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static MirToPotCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the source pot.
     *
     * @return The pot.
     */
    MirPotType pot() const;

    /**
     * @brief Replaces the source pot.
     *
     * @param value The new source pot.
     */
    void setPot(MirPotType value);

    /**
     * @brief Gets the transferred amount.
     *
     * @return The amount in lovelace.
     */
    std::uint64_t amount() const;

    /**
     * @brief Replaces the transferred amount.
     *
     * @param value The new amount in lovelace.
     */
    void setAmount(std::uint64_t value);

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
    void swap(MirToPotCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    MirToPotCert& operator=(const MirToPotCert& other);

    /**
     * @brief Moves a certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    MirToPotCert& operator=(MirToPotCert&& other) noexcept;

    /**
     * @brief Compares pot and amount.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const MirToPotCert& other) const;

    /**
     * @brief Compares pot and amount.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const MirToPotCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit MirToPotCert(cardano_mir_to_pot_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_mir_to_pot_cert_t* checked() const;

    /**
     * @brief Checks the source pot.
     *
     * @param value The pot to validate.
     */
    static void validatePot(MirPotType value);

    /**
     * @brief Allows MirCert to copy this variant.
     */
    friend class MirCert;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_mir_to_pot_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(MirToPotCert& lhs, MirToPotCert& rhs) noexcept;
} // namespace Cometa
