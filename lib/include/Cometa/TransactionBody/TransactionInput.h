/**
 * @file TransactionInput.h
 *
 * @brief Identifies an output by its creating transaction ID and zero-based output index.
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

struct cardano_transaction_input_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Identifies an output by its creating transaction ID and zero-based output index.
 *
 * The reference contains no address, value, or datum. Use Utxo when those resolved output details are
 * needed for balancing or script evaluation.
 */
class TransactionInput
{
  public:

    /**
     * @brief Initializes an output reference.
     *
     * @param id The 32-byte transaction hash to copy.
     * @param index The output index.
     */
    TransactionInput(const Blake2bHash& id, std::uint64_t index);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    TransactionInput(const TransactionInput& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    TransactionInput(TransactionInput&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~TransactionInput();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded value; empty input and trailing bytes are rejected.
     *
     * @return The decoded value.
     */
    static TransactionInput fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded value.
     */
    static TransactionInput fromCbor(std::string_view cborHex);

    /**
     * @brief Decodes a transaction ID and associates an output index.
     *
     * @param idHex The 64 hexadecimal digits of the transaction ID.
     * @param index The output index.
     *
     * @return The output reference.
     */
    static TransactionInput fromHex(std::string_view idHex, std::uint64_t index);

  public:

    /**
     * @brief Gets the creating transaction identifier.
     *
     * @return The 32-byte transaction body hash.
     */
    Blake2bHash id() const;

    /**
     * @brief Replaces the transaction ID.
     *
     * @param value The replacement to copy.
     */
    void setId(const Blake2bHash& value);

    /**
     * @brief Gets the referenced output position.
     *
     * @return The zero-based output index in the creating transaction.
     */
    std::uint64_t index() const;

    /**
     * @brief Replaces the output index.
     *
     * @param index The new output index.
     */
    void setIndex(std::uint64_t index);

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
    void swap(TransactionInput& other) noexcept;

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
    TransactionInput& operator=(const TransactionInput& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    TransactionInput& operator=(TransactionInput&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True when the values are equal.
     */
    bool operator==(const TransactionInput& other) const;

    /**
     * @brief Compares values for inequality.
     *
     * @param other The value to compare.
     *
     * @return True when the values differ.
     */
    bool operator!=(const TransactionInput& other) const;

    /**
     * @brief Compares transaction IDs, then output indices.
     *
     * @param other The reference to compare.
     *
     * @return True when this reference precedes the other.
     */
    bool operator<(const TransactionInput& other) const;

    /**
     * @brief Compares transaction IDs, then output indices.
     *
     * @param other The reference to compare.
     *
     * @return True when this reference precedes or equals the other.
     */
    bool operator<=(const TransactionInput& other) const;

    /**
     * @brief Compares transaction IDs, then output indices.
     *
     * @param other The reference to compare.
     *
     * @return True when this reference follows the other.
     */
    bool operator>(const TransactionInput& other) const;

    /**
     * @brief Compares transaction IDs, then output indices.
     *
     * @param other The reference to compare.
     *
     * @return True when this reference follows or equals the other.
     */
    bool operator>=(const TransactionInput& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static TransactionInput adopt(cardano_transaction_input_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_transaction_input_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_transaction_input_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_transaction_input_t* release() noexcept;

  private:

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit TransactionInput(cardano_transaction_input_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_transaction_input_t* checked() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_transaction_input_t* m_value;
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
void swap(TransactionInput& lhs, TransactionInput& rhs) noexcept;
} // namespace Cometa
