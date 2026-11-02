/**
 * @file GenesisKeyDelegationCert.h
 *
 * @brief Historical genesis key delegation certificates.
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

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_genesis_key_delegation_cert_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A historical genesis key delegation certificate.
 */
class GenesisKeyDelegationCert
{
  public:

    /**
     * @brief Delegates a genesis key.
     *
     * @param genesisHash The 28-byte genesis key hash.
     * @param delegateHash The 28-byte delegate key hash.
     * @param vrfKeyHash The 32-byte VRF key hash.
     */
    GenesisKeyDelegationCert(const Blake2bHash& genesisHash, const Blake2bHash& delegateHash, const Blake2bHash& vrfKeyHash);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The certificate to copy.
     */
    GenesisKeyDelegationCert(const GenesisKeyDelegationCert& other);

    /**
     * @brief Moves a certificate.
     *
     * @param other The source certificate.
     */
    GenesisKeyDelegationCert(GenesisKeyDelegationCert&& other) noexcept;

    /**
     * @brief Destroys the certificate.
     */
    ~GenesisKeyDelegationCert();

  public:

    /**
     * @brief Decodes one CBOR certificate.
     *
     * @param cbor The encoded value.
     *
     * @return The decoded certificate.
     */
    static GenesisKeyDelegationCert fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Hexadecimal CBOR without a prefix.
     *
     * @return The decoded certificate.
     */
    static GenesisKeyDelegationCert fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the genesis key hash.
     *
     * @return An independent 28-byte hash.
     */
    Blake2bHash genesisHash() const;

    /**
     * @brief Replaces the genesis key hash.
     *
     * @param value The 28-byte hash to copy.
     */
    void setGenesisHash(const Blake2bHash& value);

    /**
     * @brief Gets the delegate key hash.
     *
     * @return An independent 28-byte hash.
     */
    Blake2bHash delegateHash() const;

    /**
     * @brief Replaces the delegate key hash.
     *
     * @param value The 28-byte hash to copy.
     */
    void setDelegateHash(const Blake2bHash& value);

    /**
     * @brief Gets the VRF key hash.
     *
     * @return An independent 32-byte hash.
     */
    Blake2bHash vrfKeyHash() const;

    /**
     * @brief Replaces the VRF key hash.
     *
     * @param value The 32-byte hash to copy.
     */
    void setVrfKeyHash(const Blake2bHash& value);

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
    void swap(GenesisKeyDelegationCert& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    GenesisKeyDelegationCert& operator=(const GenesisKeyDelegationCert& other);

    /**
     * @brief Moves a certificate.
     *
     * @param other The source certificate.
     *
     * @return This certificate.
     */
    GenesisKeyDelegationCert& operator=(GenesisKeyDelegationCert&& other) noexcept;

    /**
     * @brief Compares hashes.
     *
     * @param other The certificate to compare.
     *
     * @return True when equal.
     */
    bool operator==(const GenesisKeyDelegationCert& other) const;

    /**
     * @brief Compares hashes.
     *
     * @param other The certificate to compare.
     *
     * @return True when different.
     */
    bool operator!=(const GenesisKeyDelegationCert& other) const;

  private:

    /**
     * @brief Adopts an owned certificate.
     *
     * @param value The owned reference.
     */
    explicit GenesisKeyDelegationCert(cardano_genesis_key_delegation_cert_t* value) noexcept;

    /**
     * @brief Gets the stored certificate.
     *
     * @return The borrowed reference.
     */
    cardano_genesis_key_delegation_cert_t* checked() const;

    /**
     * @brief Checks a hash length.
     *
     * @param value The hash to inspect.
     *
     * @param size The required byte count.
     */
    static void validateHash(const Blake2bHash& value, std::size_t size);

    /**
     * @brief Allows Certificate to copy this variant.
     */
    friend class Certificate;

  private:

    /**
     * @brief The owned native reference, or null after a move.
     */
    cardano_genesis_key_delegation_cert_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges certificates through argument-dependent lookup.
 *
 * @param lhs The first certificate.
 *
 * @param rhs The second certificate.
 */
void swap(GenesisKeyDelegationCert& lhs, GenesisKeyDelegationCert& rhs) noexcept;
} // namespace Cometa
