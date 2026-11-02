/**
 * @file TransactionMetadata.h
 *
 * @brief Transaction metadata indexed by unsigned labels.
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

#include <Cometa/AuxiliaryData/Metadatum.h>
#include <Cometa/AuxiliaryData/MetadatumLabelList.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_transaction_metadata_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Transaction metadata indexed by unsigned labels. */
class TransactionMetadata
{
  public:

    /**
     * @brief Initializes empty transaction metadata.
     */
    TransactionMetadata();

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     */
    TransactionMetadata(const TransactionMetadata& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    TransactionMetadata(TransactionMetadata&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~TransactionMetadata();

  public:

    /**
     * @brief Decodes exactly one CBOR map.
     *
     * @param cbor The encoded map; empty input and trailing bytes are rejected.
     *
     * @return The decoded map.
     */
    static TransactionMetadata fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     *
     * @param cborHex Pairs of hexadecimal digits without a prefix.
     *
     * @return The decoded map.
     */
    static TransactionMetadata fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of labels.
     *
     * @return The entry count.
     */
    std::size_t size() const;

    /**
     * @brief Checks whether there are no entries.
     *
     * @return True when empty.
     */
    bool empty() const;

    /**
     * @brief Looks up an independent metadata value.
     *
     * @param label The metadata label.
     *
     * @return The copied value, or nullopt when absent.
     */
    std::optional<Metadatum> get(std::uint64_t label) const;

    /**
     * @brief Gets an independent value for a required label.
     *
     * @param label The metadata label.
     *
     * @return The copied value.
     *
     * @throws std::out_of_range The label is absent.
     */
    Metadatum at(std::uint64_t label) const;

    /**
     * @brief Inserts or replaces a copied metadata value.
     *
     * @param label The unsigned metadata label.
     * @param value The value to copy.
     */
    void set(std::uint64_t label, const Metadatum& value);

    /**
     * @brief Copies an entry in ascending label order.
     *
     * @param index The zero-based entry index.
     *
     * @return The label and independent metadata value.
     *
     * @throws std::out_of_range The index is outside the map.
     */
    std::pair<std::uint64_t, Metadatum> entryAt(std::size_t index) const;

    /**
     * @brief Gets labels in ascending order.
     *
     * @return The copied labels.
     */
    MetadatumLabelList keys() const;

    /**
     * @brief Gets independent values in ascending label order.
     *
     * @return The copied values.
     */
    std::vector<Metadatum> values() const;

    /**
     * @brief Gets independent entries in ascending label order.
     *
     * @return The copied entries.
     */
    std::vector<std::pair<std::uint64_t, Metadatum>> entries() const;

    /**
     * @brief Serializes the map as CBOR.
     *
     * @return A sorted, definite-length map.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Serializes the map as CIP-116 inspection JSON.
     *
     * @return The compact inspection object.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values without allocating.
     *
     * @param other The value to exchange.
     */
    void swap(TransactionMetadata& other) noexcept;

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
    TransactionMetadata& operator=(const TransactionMetadata& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source value.
     *
     * @return This instance.
     */
    TransactionMetadata& operator=(TransactionMetadata&& other) noexcept;

    /**
     * @brief Compares map entries.
     *
     * @param other The map to compare.
     *
     * @return True when the entries are equal.
     */
    bool operator==(const TransactionMetadata& other) const;

    /**
     * @brief Compares map entries for inequality.
     *
     * @param other The map to compare.
     *
     * @return True when the entries differ.
     */
    bool operator!=(const TransactionMetadata& other) const;

  private:

    /** @brief Allows AuxiliaryData to copy attached values. */
    friend class AuxiliaryData;

    /**
     * @brief Takes ownership of an existing reference.
     *
     * @param value The owned reference.
     */
    explicit TransactionMetadata(cardano_transaction_metadata_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The borrowed reference.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_transaction_metadata_t* checked() const;

    /**
     * @brief Rejects duplicate labels in decoded metadata.
     *
     * @throws std::invalid_argument A label occurs more than once.
     */
    void validate() const;

  private:

    /** @brief The owned reference, or null after a move. */
    cardano_transaction_metadata_t* m_value;
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
void swap(TransactionMetadata& lhs, TransactionMetadata& rhs) noexcept;
} // namespace Cometa
