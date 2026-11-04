/**
 * @file Update.h
 *
 * @brief A historical collection of protocol parameter proposals for a target epoch.
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

#include <Cometa/ProtocolParams/ProposedParamUpdates.h>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_update_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief A historical collection of protocol parameter proposals for a target epoch.
 *
 * This is the legacy update mechanism. Use a governance ParameterChangeAction when constructing a
 * governance-era parameter proposal.
 */
class Update
{
  public:

    /**
     * @brief Creates an epoch update.
     * @param epoch The target epoch.
     * @param proposals Proposed parameters by genesis delegate.
     */
    Update(std::uint64_t epoch, const ProposedParamUpdates& proposals);

    /**
     * @brief Copies an update.
     * @param other The update to copy.
     */
    Update(const Update& other);

    /**
     * @brief Moves an update.
     * @param other The update to move.
     */
    Update(Update&& other) noexcept;

    /** @brief Destroys the update. */
    ~Update();

  public:

    /**
     * @brief Decodes an update from CBOR.
     * @param cbor The encoded update.
     * @return The decoded update.
     */
    static Update fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes an update from hexadecimal CBOR.
     * @param cborHex The hexadecimal encoding.
     * @return The decoded update.
     */
    static Update fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Gets the target epoch.
     * @return The epoch number.
     */
    std::uint64_t epoch() const;

    /**
     * @brief Gets the proposed parameter changes.
     * @return An independent collection of proposals.
     */
    ProposedParamUpdates proposals() const;

    /**
     * @brief Sets the target epoch.
     * @param value The epoch number.
     */
    void setEpoch(std::uint64_t value);

    /**
     * @brief Sets the proposed parameter changes.
     * @param value The proposals to set.
     */
    void setProposals(const ProposedParamUpdates& value);

    /**
     * @brief Encodes the update as CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the update as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges updates.
     * @param other The update to exchange.
     */
    void swap(Update& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The update to copy.
     * @return This update.
     */
    Update& operator=(const Update& other);

    /**
     * @brief Assigns a moved update.
     * @param other The update to move.
     * @return This update.
     */
    Update& operator=(Update&& other) noexcept;

    /**
     * @brief Compares updates.
     * @param other The update to compare.
     * @return True when equal.
     */
    bool operator==(const Update& other) const;

    /**
     * @brief Compares updates.
     * @param other The update to compare.
     * @return True when different.
     */
    bool operator!=(const Update& other) const;

  public:

    /**
     * @brief Takes ownership of one native reference without copying it.
     * @param value An owned reference; the caller must stop managing it after this call succeeds.
     * @return The C++ owner of the supplied reference.
     * @throws std::invalid_argument The reference is null; ownership is not transferred.
     */
    static Update adopt(cardano_update_t* value);

    /**
     * @brief Borrows the native object for C interoperability.
     * @return The native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    cardano_update_t* nativeHandle() noexcept;

    /**
     * @brief Borrows the native object for read-only C interoperability.
     * @return The read-only native pointer, or nullptr after a move or release.
     * @remark Do not unref the borrowed pointer. Use it only until the next operation on this object.
     */
    const cardano_update_t* nativeHandle() const noexcept;

    /**
     * @brief Transfers the owned native reference to the caller.
     * @return The owned pointer, or nullptr if this object is already empty.
     * @remark Leaves this object empty. The caller must adopt the reference or release it with the matching C unref function.
     */
    cardano_update_t* release() noexcept;

  private:

    /**
     * @brief Adopts an owned update.
     * @param value The owned update.
     */
    explicit Update(cardano_update_t* value) noexcept;

    /**
     * @brief Gets the stored update.
     * @return The borrowed update.
     */
    cardano_update_t* checked() const;

  private:

    /** @brief The owned update. */
    cardano_update_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges updates.
 * @param lhs The first update.
 * @param rhs The second update.
 */
void swap(Update& lhs, Update& rhs) noexcept;
} // namespace Cometa
