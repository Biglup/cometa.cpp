/**
 * @file JsonContext.h
 *
 * @brief JSON context.
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

/* DEFINITIONS ***************************************************************/

namespace Cometa
{
/**
 * @brief Identifies the container currently being written by JsonWriter.
 *
 * Object contexts require property names before values; array contexts contain values without names.
 */
enum class JsonContext
{
  /** @brief Root context. */
  Root = 0,

  /** @brief Object context. */
  Object = 1,

  /** @brief Array context. */
  Array = 2,
};
} // namespace Cometa
