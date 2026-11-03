/**
 * @file SlotConfig.h
 *
 * @brief Converts between ledger slot numbers and Unix time using a reference point.
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

#include <Cometa/Common/NetworkMagic.h>

#include <cstdint>

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief Converts between ledger slot numbers and Unix time using a reference point.
 *
 * The slot length and reference time must match the target network timeline. Use it when translating
 * application deadlines into transaction validity bounds.
 */
struct SlotConfig
{
    /** @brief Unix time in milliseconds corresponding to zeroSlot. */
    std::uint64_t zeroTime;

    /** @brief Reference slot number. */
    std::uint64_t zeroSlot;

    /** @brief Slot duration in milliseconds; must be greater than zero. */
    std::uint64_t slotLength;

    /**
     * @brief Gets the reference configuration for a named network.
     *
     * @param network Mainnet, Preview, or Preprod.
     *
     * @return The network's slot configuration.
     *
     * @throws std::invalid_argument The network has no predefined configuration.
     */
    static SlotConfig forNetwork(NetworkMagic network);

    /**
     * @brief Finds the slot enclosing a Unix timestamp in seconds.
     *
     * @param unixTime Seconds since the Unix epoch.
     *
     * @return The enclosing slot number.
     *
     * @throws std::invalid_argument The slot duration is zero.
     * @throws std::out_of_range The timestamp precedes the reference time.
     * @throws std::overflow_error The timestamp or result exceeds the supported range.
     */
    std::uint64_t slotFromUnixTime(std::uint64_t unixTime) const;

    /**
     * @brief Gets the Unix timestamp at the beginning of a slot.
     *
     * @param slot The slot number.
     *
     * @return Seconds since the Unix epoch, rounded down.
     *
     * @throws std::invalid_argument The slot duration is zero.
     * @throws std::out_of_range The slot precedes the reference slot.
     * @throws std::overflow_error The timestamp exceeds the supported range.
     */
    std::uint64_t unixTimeFromSlot(std::uint64_t slot) const;

    /**
     * @brief Finds the slot enclosing a Unix timestamp in milliseconds.
     *
     * @param unixTime Milliseconds since the Unix epoch.
     *
     * @return The enclosing slot number.
     *
     * @throws std::invalid_argument The slot duration is zero.
     * @throws std::out_of_range The timestamp precedes the reference time.
     * @throws std::overflow_error The slot number exceeds the supported range.
     */
    std::uint64_t slotFromUnixTimeMilliseconds(std::uint64_t unixTime) const;

    /**
     * @brief Gets the Unix timestamp at the beginning of a slot in milliseconds.
     *
     * @param slot The slot number.
     *
     * @return Milliseconds since the Unix epoch.
     *
     * @throws std::invalid_argument The slot duration is zero.
     * @throws std::out_of_range The slot precedes the reference slot.
     * @throws std::overflow_error The timestamp exceeds the supported range.
     */
    std::uint64_t unixTimeMillisecondsFromSlot(std::uint64_t slot) const;
};
} // namespace Cometa
