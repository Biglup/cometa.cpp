/**
 * \file credential.hpp
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
#include <cometa/blake2b_hash.hpp>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

struct cardano_credential_t;

namespace Biglup::Cometa
{
/** Identifies the authorization represented by a credential. */
enum class CredentialType
{
  KeyHash    = 0,
  ScriptHash = 1
};

/** Owning Credential value with independent copies and noexcept moves.
 * Moved-from values may be destroyed or assigned. Other operations throw
 * std::logic_error until a value is assigned to them.
 */
class Credential
{
  public:

    /** Construct a credential with a 28-byte key or script hash. */
    Credential(CredentialType type, const Blake2bHash& hash);

    /** Return an independent hash value. */
    Blake2bHash hash() const;

    /** Replace the hash; requires exactly 28 bytes. */
    void setHash(const Blake2bHash& hash);

    /** Return the key/script discriminator. */
    CredentialType type() const;

    /** Change the discriminator without changing the hash. */
    void setType(CredentialType type);

    /** Parse exactly one CBOR value; trailing bytes are rejected. */
    static Credential fromCbor(const std::vector<std::uint8_t>& cbor);

    /** Serialize this value as CBOR bytes. */
    std::vector<std::uint8_t> toCbor() const;

    /** Serialize this value as compact CIP-116 JSON. */
    std::string toJson() const;

    /** Order values for standard containers. */
    bool operator<(const Credential& other) const;

    /** Create an independent native copy. */
    Credential(const Credential& other);

    /** Transfer ownership without allocation. */
    Credential(Credential&& other) noexcept;

    /** Copy with the strong exception guarantee. */
    Credential& operator=(const Credential& other);

    /** Transfer ownership, releasing the previous value. Self-move is safe. */
    Credential& operator=(Credential&& other) noexcept;

    /** Release owned native storage. */
    ~Credential();

    /** Exchange values without allocation. */
    void swap(Credential& other) noexcept;

    /** Compare values, not native addresses. */
    bool operator==(const Credential& other) const;

    /** Compare values for inequality. */
    bool operator!=(const Credential& other) const;

  private:

    explicit Credential(cardano_credential_t* value) noexcept;
    cardano_credential_t* checked() const;
    cardano_credential_t* value_;
};

/** Exchange two values through argument-dependent lookup. */
void swap(Credential& lhs, Credential& rhs) noexcept;
} // namespace Biglup::Cometa
