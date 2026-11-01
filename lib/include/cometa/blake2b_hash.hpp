/**
 * \file blake2b_hash.hpp
 *
 * \author angel.castillo
 * \date   Nov 01, 2026
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
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

struct cardano_blake2b_hash_t;

namespace Biglup::Cometa
{
/** Owning Blake2bHash value with independent copies and noexcept moves.
 * Moved-from values may be destroyed or assigned. Other operations throw
 * std::logic_error until a value is assigned to them.
 */
class Blake2bHash
{
  public:

    /** Construct from raw bytes. Invalid lengths throw Error. */
    explicit Blake2bHash(const std::vector<std::uint8_t>& bytes);

    /** Parse hexadecimal bytes; malformed input throws std::invalid_argument. */
    static Blake2bHash fromHex(std::string_view hex);

    /** Return an owned byte sequence. */
    std::vector<std::uint8_t> toBytes() const;

    /** Return lowercase hexadecimal bytes. */
    std::string toHex() const;

    /** Compute a digest of 1–64 bytes from nonempty input. */
    static Blake2bHash compute(const std::vector<std::uint8_t>& data, std::size_t digestSize = 32);

    /** Parse exactly one CBOR value; trailing bytes are rejected. */
    static Blake2bHash fromCbor(const std::vector<std::uint8_t>& cbor);

    /** Serialize this value as CBOR bytes. */
    std::vector<std::uint8_t> toCbor() const;

    /** Serialize this value as compact CIP-116 JSON. */
    std::string toJson() const;

    /** Order values for standard containers. */
    bool operator<(const Blake2bHash& other) const;

    /** Create an independent native copy. */
    Blake2bHash(const Blake2bHash& other);

    /** Transfer ownership without allocation. */
    Blake2bHash(Blake2bHash&& other) noexcept;

    /** Copy with the strong exception guarantee. */
    Blake2bHash& operator=(const Blake2bHash& other);

    /** Transfer ownership, releasing the previous value. Self-move is safe. */
    Blake2bHash& operator=(Blake2bHash&& other) noexcept;

    /** Release owned native storage. */
    ~Blake2bHash();

    /** Exchange values without allocation. */
    void swap(Blake2bHash& other) noexcept;

    /** Compare values, not native addresses. */
    bool operator==(const Blake2bHash& other) const;

    /** Compare values for inequality. */
    bool operator!=(const Blake2bHash& other) const;

  private:

    explicit Blake2bHash(cardano_blake2b_hash_t* value) noexcept;
    cardano_blake2b_hash_t* checked() const;
    cardano_blake2b_hash_t* value_;
};

/** Exchange two values through argument-dependent lookup. */
void swap(Blake2bHash& lhs, Blake2bHash& rhs) noexcept;
} // namespace Biglup::Cometa
