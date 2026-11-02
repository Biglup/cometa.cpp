/**
 * @file AssetArithmetic.h
 *
 * @brief Checked signed asset arithmetic.
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

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Adds signed quantities without overflow.
 *
 * @param left The left operand.
 * @param right The right operand.
 *
 * @return The result.
 *
 * @throws std::overflow_error The result is outside int64_t.
 */
std::int64_t checkedAddAmount(std::int64_t left, std::int64_t right);

/**
 * @brief Subtracts signed quantities without overflow.
 *
 * @param left The left operand.
 * @param right The right operand.
 *
 * @return The result.
 *
 * @throws std::overflow_error The result is outside int64_t.
 */
std::int64_t checkedSubtractAmount(std::int64_t left, std::int64_t right);

} // namespace Cometa
