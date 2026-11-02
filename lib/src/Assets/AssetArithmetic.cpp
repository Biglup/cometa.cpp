/**
 * @file AssetArithmetic.cpp
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

/* INCLUDES ******************************************************************/

#include "AssetArithmetic.h"

#include <limits>
#include <stdexcept>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::int64_t
checkedAddAmount(std::int64_t left, std::int64_t right)
{
  const std::int64_t maximum = std::numeric_limits<std::int64_t>::max();
  const std::int64_t minimum = std::numeric_limits<std::int64_t>::min();

  if ((right > 0 && left > maximum - right) || (right < 0 && left < minimum - right))
  {
    throw std::overflow_error("Asset quantity exceeds int64_t range");
  }

  return left + right;
}

std::int64_t
checkedSubtractAmount(std::int64_t left, std::int64_t right)
{
  const std::int64_t maximum = std::numeric_limits<std::int64_t>::max();
  const std::int64_t minimum = std::numeric_limits<std::int64_t>::min();

  if ((right > 0 && left < minimum + right) || (right < 0 && left > maximum + right))
  {
    throw std::overflow_error("Asset quantity exceeds int64_t range");
  }

  return left - right;
}

} // namespace Cometa
