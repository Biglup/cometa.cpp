/**
 * @file Utxo.h
 *
 * @brief A transaction output together with the input reference that identifies it.
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

#include <Cometa/TransactionBody/TransactionInput.h>
#include <Cometa/TransactionBody/TransactionOutput.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_utxo_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A transaction output together with the input reference that identifies it.
 *
 * The output supplies the address, value, datum, and reference script needed for balancing and
 * evaluation. The reference identifies the transaction and output index to spend or read.
 */
class Utxo
{
  public:

    /**
     * @brief Initializes an unspent output.
     *
     * @param input The output reference to copy.
     * @param output The output contents to copy.
     */
    Utxo(const TransactionInput& input, const TransactionOutput& output);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    Utxo(const Utxo& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    Utxo(Utxo&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~Utxo();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static Utxo fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static Utxo fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the input.
     *
     * @return An independent value.
     */
    TransactionInput input() const;

    /**
     * @brief Replaces the input.
     *
     * @param value The replacement to copy.
     */
    void setInput(const TransactionInput& value);

    /**
     * @brief Gets the output.
     *
     * @return An independent value.
     */
    TransactionOutput output() const;

    /**
     * @brief Replaces the output.
     *
     * @param value The replacement to copy.
     */
    void setOutput(const TransactionOutput& value);

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
    void swap(Utxo& other) noexcept;

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
    Utxo& operator=(const Utxo& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    Utxo& operator=(Utxo&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when the values are equal.
     */
    bool operator==(const Utxo& other) const;

    /**
     * @brief Compares values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True when the values differ.
     */
    bool operator!=(const Utxo& other) const;

  private:

    /** @brief Allows transactions to use this value. */
    friend class Transaction;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit Utxo(cardano_utxo_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_utxo_t* checked() const;

    /**
     * @brief Checks the decoded value invariants.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_utxo_t* m_value;
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
void swap(Utxo& lhs, Utxo& rhs) noexcept;
} // namespace Cometa
