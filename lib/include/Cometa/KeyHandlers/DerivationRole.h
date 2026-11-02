/**
 * @file DerivationRole.h
 *
 * @brief CIP-1852 key roles.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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
/** @brief The role segment of a Cardano wallet derivation path. */
enum class DerivationRole : std::uint32_t
{
  /** @brief Payment keys for receiving addresses. */
  External = 0,

  /** @brief Payment keys for change addresses. */
  Internal = 1,

  /** @brief Staking keys. */
  Staking = 2,

  /** @brief DRep governance keys. */
  DRep = 3,

  /** @brief Constitutional committee cold keys. */
  CommitteeCold = 4,

  /** @brief Constitutional committee hot keys. */
  CommitteeHot = 5
};
} // namespace Cometa
