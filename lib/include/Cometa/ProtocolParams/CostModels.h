/**
 * @file CostModels.h
 *
 * @brief Plutus cost models indexed by language.
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

#include <Cometa/ProtocolParams/CostModel.h>

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_costmdls_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Cost models indexed by Plutus language ID. */
class CostModels
{
  public:

    /** @brief Creates an empty collection. */
    CostModels();

    /**
     * @brief Copies a collection.
     * @param other The collection to copy.
     */
    CostModels(const CostModels& other);

    /**
     * @brief Moves a collection.
     * @param other The collection to move.
     */
    CostModels(CostModels&& other) noexcept;

    /** @brief Destroys the collection. */
    ~CostModels();

  public:

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded collection.
     */
    static CostModels fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded collection.
     */
    static CostModels fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Inserts or replaces a model.
     * @param model The model to copy.
     */
    void insert(const CostModel& model);

    /**
     * @brief Checks for a known language.
     * @param language The language.
     * @return True when present.
     */
    bool contains(PlutusLanguageVersion language) const;

    /**
     * @brief Gets a model by raw language ID.
     * @param languageId The ID.
     * @return An independent model, or empty.
     */
    std::optional<CostModel> get(std::uint8_t languageId) const;

    /**
     * @brief Gets a model by known language.
     * @param language The language.
     * @return An independent model, or empty.
     */
    std::optional<CostModel> get(PlutusLanguageVersion language) const;

    /**
     * @brief Gets the language views for script data hashes.
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> languageViews() const;

    /**
     * @brief Encodes the collection as CBOR.
     * @return Encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the collection as CIP-116 JSON.
     * @return Compact JSON.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges collections.
     * @param other The other collection.
     */
    void swap(CostModels& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The source.
     * @return This collection.
     */
    CostModels& operator=(const CostModels& other);

    /**
     * @brief Assigns a moved collection.
     * @param other The source.
     * @return This collection.
     */
    CostModels& operator=(CostModels&& other) noexcept;

    /**
     * @brief Compares collections.
     * @param other The other collection.
     * @return True when equal.
     */
    bool operator==(const CostModels& other) const;

    /**
     * @brief Compares collections.
     * @param other The other collection.
     * @return True when different.
     */
    bool operator!=(const CostModels& other) const;

  private:

    /**
     * @brief Adopts an owned collection.
     * @param value The owned collection.
     */
    explicit CostModels(cardano_costmdls_t* value) noexcept;

    /**
     * @brief Gets the stored collection.
     * @return The borrowed collection.
     */
    cardano_costmdls_t* checked() const;

  private:

    /** @brief The owned collection. */
    cardano_costmdls_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/** @brief Exchanges collections. @param lhs The first collection. @param rhs The second collection. */
void swap(CostModels& lhs, CostModels& rhs) noexcept;
} // namespace Cometa
