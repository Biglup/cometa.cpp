/**
 * @file StakePointer.h
 *
 * @brief StakePointer value.
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

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief The registration location referenced by a pointer address.
 */
struct StakePointer
{
    /** @brief Absolute slot of the registration certificate. */
    std::uint64_t slot = 0;

    /** @brief Transaction index within the slot. */
    std::uint64_t transactionIndex = 0;

    /** @brief Certificate index within the transaction. */
    std::uint64_t certificateIndex = 0;
};
} // namespace Cometa
