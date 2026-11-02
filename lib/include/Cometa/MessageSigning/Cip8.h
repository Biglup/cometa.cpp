/**
 * @file Cip8.h
 *
 * @brief CIP-8 message signatures.
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

#include <Cometa/Address/Address.h>
#include <Cometa/Crypto/Blake2bHash.h>
#include <Cometa/Crypto/Ed25519PrivateKey.h>

#include <cstdint>
#include <vector>

/* DECLARATIONS **************************************************************/

namespace Cometa
{
/** @brief A CIP-8 signature and its verification key, encoded as COSE CBOR. */
struct Cip8Signature
{
    /** @brief The COSE_Sign1 CBOR bytes. */
    std::vector<std::uint8_t> coseSign1;

    /** @brief The COSE_Key CBOR bytes. */
    std::vector<std::uint8_t> coseKey;
};

/**
 * @brief Signs a message bound to a Cardano address.
 * @param message The bytes to sign.
 * @param address The address to bind to the signature.
 * @param key The Ed25519 signing key.
 * @return The COSE signature and verification key.
 */
Cip8Signature signCip8(const std::vector<std::uint8_t>& message, const Address& address, const Ed25519PrivateKey& key);

/**
 * @brief Signs a message bound to a key hash.
 * @param message The bytes to sign.
 * @param keyHash The key hash to bind to the signature.
 * @param key The Ed25519 signing key.
 * @return The COSE signature and verification key.
 */
Cip8Signature signCip8(const std::vector<std::uint8_t>& message, const Blake2bHash& keyHash, const Ed25519PrivateKey& key);
} // namespace Cometa
