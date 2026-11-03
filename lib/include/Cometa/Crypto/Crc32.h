/**
 * @file Crc32.h
 *
 * @brief CRC32 checksums for detecting accidental corruption of encoded data.
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
#include <vector>

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Computes a CRC32 checksum.
 *
 * @param bytes The input bytes; may be empty.
 *
 * @return The checksum, or zero for empty input.
 */
std::uint32_t crc32(const std::vector<std::uint8_t>& bytes) noexcept;
} // namespace Cometa
