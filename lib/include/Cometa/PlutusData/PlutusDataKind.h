/**
 * @file PlutusDataKind.h
 *
 * @brief Declaration of the PlutusDataKind enumeration.
 *
 * @author angel.castillo
 * @date   Nov 01, 2026
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
/** @brief Identifies a Plutus data variant. */
enum class PlutusDataKind
{
  /** @brief Constructor alternative and fields. */
  Constructor = 0,

  /** @brief Ordered key-value pairs. */
  Map = 1,

  /** @brief Ordered data values. */
  List = 2,

  /** @brief Arbitrary-precision signed integer. */
  Integer = 3,

  /** @brief Byte string. */
  Bytes = 4
};
} // namespace Cometa
