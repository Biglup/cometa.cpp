/**
 * @file PassphraseProvider.h
 *
 * @brief Passphrase retrieval for software signing handlers.
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
#include <functional>
#include <vector>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/** @brief Supplies a passphrase when encrypted software keys are used. */
using PassphraseProvider = std::function<std::vector<std::uint8_t>()>;
} // namespace Cometa
