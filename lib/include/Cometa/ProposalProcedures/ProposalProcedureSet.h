/**
 * @file ProposalProcedureSet.h
 *
 * @brief Ledger proposal procedure collections.
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

#include <Cometa/ProposalProcedures/ProposalProcedure.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_proposal_procedure_set_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A collection of ledger proposal procedures in insertion order.
 */
class ProposalProcedureSet
{
  public:

    /** @brief Creates an empty proposal procedure collection. */
    ProposalProcedureSet();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    ProposalProcedureSet(const ProposalProcedureSet& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    ProposalProcedureSet(ProposalProcedureSet&& other) noexcept;

    /** @brief Destroys the collection. */
    ~ProposalProcedureSet();

  public:

    /**
     * @brief Decodes one CBOR collection.
     * @param cbor The encoded collection.
     * @return The decoded collection.
     */
    static ProposalProcedureSet fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded collection.
     */
    static ProposalProcedureSet fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the number of proposal procedures.
     * @return The number of elements.
     */
    std::size_t size() const;

    /**
     * @brief Appends a proposal procedure.
     * @param value The proposal procedure to copy.
     */
    void add(const ProposalProcedure& value);

    /**
     * @brief Gets a proposal procedure by index.
     * @param index The zero-based index.
     * @return An independent proposal procedure.
     */
    ProposalProcedure at(std::size_t index) const;

    /**
     * @brief Encodes the collection as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the collection as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges collections.
     * @param other The collection to exchange.
     */
    void swap(ProposalProcedureSet& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The collection to copy.
     * @return This collection.
     */
    ProposalProcedureSet& operator=(const ProposalProcedureSet& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The collection to move.
     * @return This collection.
     */
    ProposalProcedureSet& operator=(ProposalProcedureSet&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when equal.
     */
    bool operator==(const ProposalProcedureSet& other) const;

    /**
     * @brief Compares collections.
     * @param other The collection to compare.
     * @return True when different.
     */
    bool operator!=(const ProposalProcedureSet& other) const;

  private:

    /** @brief Allows sub-transaction bodies to copy this value. */
    friend class SubTransactionBody;

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit ProposalProcedureSet(cardano_proposal_procedure_set_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_proposal_procedure_set_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_proposal_procedure_set_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges proposal procedure collections through argument-dependent lookup.
 * @param lhs The first collection.
 * @param rhs The second collection.
 */
void swap(ProposalProcedureSet& lhs, ProposalProcedureSet& rhs) noexcept;
} // namespace Cometa
