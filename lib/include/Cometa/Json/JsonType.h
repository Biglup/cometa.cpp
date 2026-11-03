/**
 * @file JsonType.h
 *
 * @brief JSON type.
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
 * @brief Identifies the value kind of a parsed JSON item.
 *
 * Inspect this discriminator before accessing an object, array, string, number, boolean, or null.
 */
enum class JsonType
{
  /** @brief Object type. */
  Object = 0,

  /** @brief Array type. */
  Array = 1,

  /** @brief String type. */
  String = 2,

  /** @brief Number type. */
  Number = 3,

  /** @brief Boolean type. */
  Boolean = 4,

  /** @brief Null type. */
  Null = 5,
};
} // namespace Cometa
