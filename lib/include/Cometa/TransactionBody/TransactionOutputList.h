/**
 * @file TransactionOutputList.h
 *
 * @brief An ordered sequence of transaction outputs.
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

#include <Cometa/TransactionBody/TransactionOutput.h>

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_transaction_output_list_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief An ordered sequence of transaction outputs. */
class TransactionOutputList
{
  public:

    /**
     * @brief Initializes an empty output list.
     */
    TransactionOutputList();

    /**
     * @brief Initializes an ordered list of copied outputs.
     *
     * @param outputs The child outputs to copy.
     */
    TransactionOutputList(std::initializer_list<TransactionOutput> outputs);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    TransactionOutputList(const TransactionOutputList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    TransactionOutputList(TransactionOutputList&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~TransactionOutputList();

  public:

    /**
     * @brief Decodes exactly one CBOR output list.
     *
     * @param cbor The encoded output; empty input and trailing bytes are rejected.
     *
     * @return The decoded output.
     */
    static TransactionOutputList fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded output.
     */
    static TransactionOutputList fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of child outputs.
     *
     * @return The number of children.
     */
    std::size_t size() const;

    /**
     * @brief Tests whether the list is empty.
     *
     * @return True when there are no outputs.
     */
    bool empty() const;

    /**
     * @brief Gets an independent child output.
     *
     * @param index The zero-based child index.
     *
     * @return The copied output.
     *
     * @throws std::out_of_range The index is not less than size().
     */
    TransactionOutput at(std::size_t index) const;

    /**
     * @brief Appends a copied child output.
     *
     * @param output The child to append.
     */
    void pushBack(const TransactionOutput& output);

    /**
     * @brief Gets independent children in order.
     *
     * @return The copied child outputs.
     */
    std::vector<TransactionOutput> values() const;

    /**
     * @brief Serializes the output list as CBOR.
     *
     * @return A definite-length output array with normalized output forms.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the output list as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(TransactionOutputList& other) noexcept;

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
    TransactionOutputList& operator=(const TransactionOutputList& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    TransactionOutputList& operator=(TransactionOutputList&& other) noexcept;

    /**
     * @brief Compares output values.
     *
     * @param other The output list to compare.
     *
     * @return True when the values are equal.
     */
    bool operator==(const TransactionOutputList& other) const;

    /**
     * @brief Compares output values for inequality.
     *
     * @param other The output list to compare.
     *
     * @return True when the values differ.
     */
    bool operator!=(const TransactionOutputList& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /** @brief Allows transaction bodies to copy this value. */
    friend class TransactionBody;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit TransactionOutputList(cardano_transaction_output_list_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_transaction_output_list_t* checked() const;

    /**
     * @brief Checks nested output values before exposing decoded values.
     *
     * @throws std::invalid_argument An output violates the value constraints.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_transaction_output_list_t* m_value;
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
void swap(TransactionOutputList& lhs, TransactionOutputList& rhs) noexcept;
} // namespace Cometa
