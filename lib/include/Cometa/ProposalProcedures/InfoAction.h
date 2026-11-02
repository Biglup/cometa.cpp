/**
 * @file InfoAction.h
 *
 * @brief Informational governance action.
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

struct cardano_info_action_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/** @brief Governance action that records information on chain. */
class InfoAction
{
  public:

    /** @brief Creates an informational action. */
    InfoAction();

    /**
     * @brief Copies an action.
     * @param other The action to copy.
     */
    InfoAction(const InfoAction& other);

    /**
     * @brief Moves an action.
     * @param other The action to move.
     */
    InfoAction(InfoAction&& other) noexcept;

    /** @brief Destroys the action. */
    ~InfoAction();

  public:

    /**
     * @brief Decodes CBOR.
     * @param cbor Encoded bytes.
     * @return The decoded action.
     */
    static InfoAction fromCbor(const std::vector<std::uint8_t>& cbor);

    /**
     * @brief Decodes hexadecimal CBOR.
     * @param cborHex Hexadecimal bytes.
     * @return The decoded action.
     */
    static InfoAction fromCbor(std::string_view cborHex);

  public:

    /**
     * @brief Encodes CBOR.
     * @return The encoded bytes.
     */
    std::vector<std::uint8_t> toCbor() const;

    /**
     * @brief Formats the action as CIP-116 JSON.
     * @return Compact JSON text.
     */
    std::string toJson() const;

    /**
     * @brief Exchanges actions.
     * @param other The action to exchange.
     */
    void swap(InfoAction& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     * @param other The action to copy.
     * @return This action.
     */
    InfoAction& operator=(const InfoAction& other);

    /**
     * @brief Assigns a moved action.
     * @param other The action to move.
     * @return This action.
     */
    InfoAction& operator=(InfoAction&& other) noexcept;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when equal.
     */
    bool operator==(const InfoAction& other) const;

    /**
     * @brief Compares actions.
     * @param other The action to compare.
     * @return True when different.
     */
    bool operator!=(const InfoAction& other) const;

  private:

    /**
     * @brief Adopts an owned action.
     * @param value The owned action.
     */
    explicit InfoAction(cardano_info_action_t* value) noexcept;

    /**
     * @brief Gets the stored action.
     * @return The borrowed action.
     */
    cardano_info_action_t* checked() const;

  private:

    /** @brief The owned action. */
    cardano_info_action_t* m_value;
};

/* PROTOTYPES ****************************************************************/

/**
 * @brief Exchanges informational actions.
 * @param lhs The first action.
 * @param rhs The second action.
 */
void swap(InfoAction& lhs, InfoAction& rhs) noexcept;
} // namespace Cometa
