/**
 * @file CostModel.h
 *
 * @brief Plutus operation costs.
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

#include <Cometa/Scripts/PlutusScripts/PlutusLanguageVersion.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_cost_model_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Indexed operation costs for a Plutus language. */
class CostModel
{
  public:

    /**
     * @brief Creates a model for a known Plutus language.
     * @param language The language version.
     * @param costs Costs in operation order.
     */
    CostModel(PlutusLanguageVersion language, const std::vector<std::int64_t>& costs);

    /**
     * @brief Copies a cost model.
     * @param other The model to copy.
     */
    CostModel(const CostModel& other);

    /**
     * @brief Moves a cost model.
     * @param other The model to move.
     */
    CostModel(CostModel&& other) noexcept;

    /** @brief Destroys the model. */
    ~CostModel();

  public:

    /**
     * @brief Decodes a cost model from CBOR.
     * @param cbor The encoded model.
     * @return The decoded model.
     */
    static CostModel fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded model.
     */
    static CostModel fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the raw language ID, including unknown future IDs.
     * @return The language ID.
     */
    std::uint8_t languageId() const;

    /**
     * @brief Gets a known Plutus language version.
     * @return The language version, or empty for an unknown ID.
     */
    std::optional<PlutusLanguageVersion> language() const;

    /**
     * @brief Gets the number of operation costs.
     * @return The number of costs.
     */
    std::size_t size() const;

    /**
     * @brief Gets an operation cost.
     * @param operation The zero-based operation index.
     * @return The cost.
     */
    std::int64_t cost(std::size_t operation) const;

    /**
     * @brief Copies all operation costs.
     * @return Costs in operation order.
     */
    std::vector<std::int64_t> costs() const;

    /**
     * @brief Sets an operation cost.
     * @param operation The zero-based operation index.
     * @param value The new cost.
     */
    void setCost(std::size_t operation, std::int64_t value);

    /**
     * @brief Encodes the model as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the model as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges models.
     * @param other The model to exchange.
     */
    void swap(CostModel& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The model to copy.
     * @return This model.
     */
    CostModel& operator=(const CostModel& other);

    /**
     * @brief Assigns a moved model.
     * @param other The model to move.
     * @return This model.
     */
    CostModel& operator=(CostModel&& other) noexcept;

    /**
     * @brief Compares models.
     * @param other The model to compare.
     * @return True when equal.
     */
    bool operator==(const CostModel& other) const;

    /**
     * @brief Compares models.
     * @param other The model to compare.
     * @return True when different.
     */
    bool operator!=(const CostModel& other) const;

  private:

    /** @brief Allows CostModels to copy model values. */
    friend class CostModels;

    /**
     * @brief Adopts an owned model.
     * @param value The owned model.
     */
    explicit CostModel(cardano_cost_model_t* value) noexcept;

    /**
     * @brief Gets the stored model.
     * @return The borrowed model.
     */
    cardano_cost_model_t* checked() const;

  private:

    /** @brief The owned model. */
    cardano_cost_model_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges cost models through argument-dependent lookup.
 * @param lhs The first model.
 * @param rhs The second model.
 */
void swap(CostModel& lhs, CostModel& rhs) noexcept;
} // namespace Cometa
