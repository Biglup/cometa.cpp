/**
 * @file TransactionOutput.h
 *
 * @brief An address, value, and optional output data.
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

#include <Cometa/Address/Address.h>
#include <Cometa/Common/Datum.h>
#include <Cometa/Scripts/Script.h>
#include <Cometa/TransactionBody/Value.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_transaction_output_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief An address, value, and optional output data. */
class TransactionOutput
{
  public:

    /**
     * @brief Initializes an output without datum or reference script.
     *
     * @param address The destination address to copy.
     * @param amount The value to copy; lovelace must be nonnegative.
     */
    TransactionOutput(const Address& address, const Value& amount);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    TransactionOutput(const TransactionOutput& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    TransactionOutput(TransactionOutput&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~TransactionOutput();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static TransactionOutput fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static TransactionOutput fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the destination address.
     *
     * @return An independent value.
     */
    Address address() const;

    /**
     * @brief Replaces the destination address.
     *
     * @param value The replacement to copy.
     */
    void setAddress(const Address& value);

    /**
     * @brief Gets the lovelace and assets.
     *
     * @return An independent value.
     */
    Value amount() const;

    /**
     * @brief Replaces the lovelace and assets.
     *
     * @param value The replacement to copy.
     */
    void setAmount(const Value& value);

    /**
     * @brief Gets the datum.
     *
     * @return An independent value, or nullopt if absent.
     */
    std::optional<Datum> datum() const;

    /**
     * @brief Replaces or removes the datum.
     *
     * @param value The replacement to copy; nullopt removes the field.
     */
    void setDatum(const std::optional<Datum>& value);

    /**
     * @brief Gets the reference script.
     *
     * @return An independent value, or nullopt if absent.
     */
    std::optional<Script> scriptRef() const;

    /**
     * @brief Replaces or removes the reference script.
     *
     * @param value The replacement to copy; nullopt removes the field.
     */
    void setScriptRef(const std::optional<Script>& value);

    /**
     * @brief Serializes the value as CBOR.
     *
     * @return The encoded value; output forms and container headers are normalized.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the value as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(TransactionOutput& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The source value.
     *
     * @return This instance.
     *
     * @remark If copying fails, this instance is unchanged.
     */
    TransactionOutput& operator=(const TransactionOutput& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    TransactionOutput& operator=(TransactionOutput&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when the values are equal.
     */
    bool operator==(const TransactionOutput& other) const;

    /**
     * @brief Compares values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True when the values differ.
     */
    bool operator!=(const TransactionOutput& other) const;

  private:

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /** @brief Allows Utxo to copy and validate components. */
    friend class Utxo;

    /** @brief Allows TransactionOutputList to copy and validate components. */
    friend class TransactionOutputList;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit TransactionOutput(cardano_transaction_output_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_transaction_output_t* checked() const;

    /**
     * @brief Checks the decoded value invariants.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_transaction_output_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges values through argument-dependent lookup.
 *
 * @param lhs The first value.
 * @param rhs The second value.
 */
void swap(TransactionOutput& lhs, TransactionOutput& rhs) noexcept;
} // namespace Cometa
