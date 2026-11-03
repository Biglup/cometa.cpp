/**
 * @file ExUnits.h
 *
 * @brief A Plutus execution budget measured in memory units and execution steps.
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

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_ex_units_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A Plutus execution budget measured in memory units and execution steps.
 *
 * These are ledger accounting units, not bytes or elapsed time. The prices in ExUnitPrices determine
 * its execution-fee contribution; TransactionEvaluator estimates budgets for redeemers.
 */
class ExUnits
{
  public:

    /**
     * @brief Initializes the value.
     *
     * @param memory Memory units.
     * @param cpuSteps CPU execution steps.
     */
    ExUnits(std::uint64_t memory, std::uint64_t cpuSteps);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    ExUnits(const ExUnits& other);

    /**
     * @brief Moves another value into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    ExUnits(ExUnits&& other) noexcept;

    /**
     * @brief Destroys this value.
     */
    ~ExUnits();

  public:

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cbor The encoded bytes.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static ExUnits fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes exactly one CBOR value.
     *
     * @param cborHex Hexadecimal CBOR without a 0x prefix.
     *
     * @return The decoded value.
     *
     * @throws std::invalid_argument Input is empty, hex is invalid, or trailing bytes remain.
     * @throws Error The CBOR input is invalid.
     */
    static ExUnits fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the Plutus memory budget.
     *
     * @return The number of ledger memory units, not a byte count.
     */
    std::uint64_t memory() const;

    /**
     * @brief Replaces the memory units.
     *
     * @param memory Memory units.
     */
    void setMemory(std::uint64_t memory);

    /**
     * @brief Gets the Plutus execution-step budget.
     *
     * @return The number of abstract execution steps, not processor cycles.
     */
    std::uint64_t cpuSteps() const;

    /**
     * @brief Replaces the CPU execution steps.
     *
     * @param cpuSteps CPU execution steps.
     */
    void setCpuSteps(std::uint64_t cpuSteps);

    /**
     * @brief Encodes the value as CBOR.
     *
     * @return The CBOR bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Encodes the value as CIP-116 JSON.
     *
     * @return The compact JSON string.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges two values.
     *
     * @param other The value to exchange with this instance.
     */
    void swap(ExUnits& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance. If copying fails, its value is unchanged.
     */
    ExUnits& operator=(const ExUnits& other);

    /**
     * @brief Replaces this value by moving another value.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    ExUnits& operator=(ExUnits&& other) noexcept;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are equal.
     */
    bool operator==(const ExUnits& other) const;

    /**
     * @brief Compares values.
     *
     * @param other The value to compare.
     *
     * @return True if the values are different.
     */
    bool operator!=(const ExUnits& other) const;

    /** @brief Allows redeemers to copy their execution units. */
    friend class Redeemer;

    /** @brief Allows protocol updates to copy native values. */
    friend class ProtocolParamUpdate;

    /** @brief Allows protocol parameters to copy native values. */
    friend class ProtocolParameters;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value A non-null reference transferred to this instance.
     */
    explicit ExUnits(cardano_ex_units_t* value) noexcept;

  private:

    /**
     * @brief Gets the stored value after checking its lifetime.
     *
     * @return The reference, without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_ex_units_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_ex_units_t* m_value;
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
void swap(ExUnits& lhs, ExUnits& rhs) noexcept;
} // namespace Cometa
