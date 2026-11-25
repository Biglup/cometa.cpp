/**
 * @file RemoteCodec.h
 *
 * @brief Private remote provider implementation.
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

#include <Cometa/Cometa.h>

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/** @brief Escapes a string as JSON. @param value Text. @return Encoded JSON string. */
std::string remoteString(const std::string& value);

/** @brief Assembles encoded JSON properties. @param fields Encoded values. @return JSON object. */
std::string remoteObject(std::initializer_list<std::pair<std::string, std::string>> fields);

/** @brief Encodes a byte string as hexadecimal. @param bytes Bytes. @return Lowercase hex. */
std::string remoteHex(const std::vector<std::uint8_t>& bytes);

/** @brief Decodes and validates hexadecimal. @param value Hex. @return Bytes. */
std::vector<std::uint8_t> remoteBytes(const std::string& value);

/** @brief Checks a complete successful response. @param response HTTP response. @return Parsed JSON. */
JsonValue remoteJson(const HttpResponse& response);

/** @brief Converts a full Ogmios snapshot. @param value Snapshot. @return Parameters. */
ProtocolParameters remoteParameters(const JsonValue& value);

/** @brief Decodes and verifies a reference script. @param kind Language. @param bytes Encoded bytes. @param hash Expected hash. @return Script. */
Script remoteScript(const std::string& kind, const std::string& bytes, const std::string& hash);

/** @brief Verifies datum bytes. @param bytes CBOR hex. @param hash Expected hash. @return Datum. */
PlutusData remoteDatum(const std::string& bytes, const Blake2bHash& hash);

/** @brief Encodes an additional Ogmios input. @param value UTxO. @return Encoded JSON. */
std::string remoteOgmiosOutput(const Utxo& value);

/** @brief Applies a complete, unique budget response. @param transaction Original transaction. @param result Budget array. @return Redeemers. */
RedeemerList remoteBudgets(const Transaction& transaction, const JsonValue& result);

/** @brief Creates a stable output identity. @param input Output reference. @return Transaction hash and index. */
std::string remoteInputKey(const TransactionInput& input);

/** @brief Reads an unsigned quantity fitting the signed ledger value. @param value Quantity. @return Nonnegative quantity. */
std::int64_t remoteQuantity(const JsonValue& value);

/** @brief Checks a property for a non-null value. @param value Object. @param key Property. @return Presence. */
bool remotePresent(const JsonValue& value, const std::string& key);

/** @brief Decodes Hydra CLI parameters. @param value JSON snapshot. @return Parameters. */
ProtocolParameters remoteHydraParameters(const JsonValue& value);

/** @brief Decodes Hydra confirmed UTxOs. @param value JSON map. @return Outputs. */
UtxoList remoteHydraOutputs(const JsonValue& value);

} // namespace Cometa
