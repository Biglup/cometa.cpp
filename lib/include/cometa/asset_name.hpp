/**
 * \file asset_name.hpp
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

struct cardano_asset_name_t;

namespace Biglup::Cometa
{
/** Owning AssetName value with independent copies and noexcept moves.
 * Moved-from values may be destroyed or assigned. Other operations throw
 * std::logic_error until a value is assigned to them.
 */
class AssetName
{
  public:

    /** Construct from raw bytes. Invalid lengths throw Error. */
    explicit AssetName(const std::vector<std::uint8_t>& bytes);

    /** Parse hexadecimal bytes; malformed input throws std::invalid_argument. */
    static AssetName fromHex(std::string_view hex);

    /** Return an owned byte sequence. */
    std::vector<std::uint8_t> toBytes() const;

    /** Return lowercase hexadecimal bytes. */
    std::string toHex() const;

    /** Construct an asset name from UTF-8 bytes; the maximum length is 32 bytes. */
    static AssetName fromString(std::string_view text);

    /** Return the asset bytes as a string, preserving embedded NUL bytes. */
    std::string toString() const;

    /** Parse exactly one CBOR value; trailing bytes are rejected. */
    static AssetName fromCbor(const std::vector<std::uint8_t>& cbor);

    /** Serialize this value as CBOR bytes. */
    std::vector<std::uint8_t> toCbor() const;

    /** Serialize this value as compact CIP-116 JSON. */
    std::string toJson() const;

    /** Order values for standard containers. */
    bool operator<(const AssetName& other) const;

    /** Create an independent native copy. */
    AssetName(const AssetName& other);

    /** Transfer ownership without allocation. */
    AssetName(AssetName&& other) noexcept;

    /** Copy with the strong exception guarantee. */
    AssetName& operator=(const AssetName& other);

    /** Transfer ownership, releasing the previous value. Self-move is safe. */
    AssetName& operator=(AssetName&& other) noexcept;

    /** Release owned native storage. */
    ~AssetName();

    /** Exchange values without allocation. */
    void swap(AssetName& other) noexcept;

    /** Compare values, not native addresses. */
    bool operator==(const AssetName& other) const;

    /** Compare values for inequality. */
    bool operator!=(const AssetName& other) const;

  private:

    explicit AssetName(cardano_asset_name_t* value) noexcept;
    cardano_asset_name_t* checked() const;
    cardano_asset_name_t* value_;
};

/** Exchange two values through argument-dependent lookup. */
void swap(AssetName& lhs, AssetName& rhs) noexcept;
} // namespace Biglup::Cometa
