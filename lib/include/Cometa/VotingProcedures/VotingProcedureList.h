/**
 * @file VotingProcedureList.h
 *
 * @brief Ordered VotingProcedure values.
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

#include <Cometa/VotingProcedures/VotingProcedure.h>

#include <vector>

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Governance voting choices and optional metadata anchors in application order.
 */
using VotingProcedureList = std::vector<VotingProcedure>;
} // namespace Cometa
