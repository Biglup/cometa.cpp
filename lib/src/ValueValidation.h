/**
 * @file ValueValidation.h
 *
 * @brief Private validation of parsed Cardano values.
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

/* FORWARD DECLARATIONS ******************************************************/

namespace Cometa
{
class Utxo;
class TransactionMetadata;
class AuxiliaryData;
class Script;
class TransactionOutputList;
class NativeScriptList;
class TransactionInput;
class TransactionOutput;
class NativeScript;

/* PROTOTYPES ****************************************************************/

/**
 * @brief Checks the domain constraints of a parsed Utxo.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const Utxo& value);

/**
 * @brief Checks the domain constraints of a parsed TransactionMetadata.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const TransactionMetadata& value);

/**
 * @brief Checks the domain constraints of a parsed AuxiliaryData.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const AuxiliaryData& value);

/**
 * @brief Checks the domain constraints of a parsed Script.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const Script& value);

/**
 * @brief Checks the domain constraints of a parsed TransactionOutputList.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const TransactionOutputList& value);

/**
 * @brief Checks the domain constraints of a parsed NativeScriptList.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const NativeScriptList& value);

/**
 * @brief Checks the domain constraints of a parsed TransactionInput.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const TransactionInput& value);

/**
 * @brief Checks the domain constraints of a parsed TransactionOutput.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const TransactionOutput& value);

/**
 * @brief Checks the domain constraints of a parsed NativeScript.
 * @param value The value to validate.
 * @throws std::invalid_argument A domain constraint is violated.
 */
void validateValue(const NativeScript& value);

} // namespace Cometa
