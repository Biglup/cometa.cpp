/**
 * @file ProtocolParametersInterop.h
 *
 * @brief Private protocol-parameter conversions.
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

#include <Cometa/ProtocolParams/ProtocolParameters.h>

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Copies C++ protocol parameters for a native operation.
 * @param parameters The parameters to copy.
 * @return An owned native reference.
 */
cardano_protocol_parameters_t* copyProtocolParametersToNative(const ProtocolParameters& parameters);

/**
 * @brief Copies native protocol parameters for a C++ callback.
 * @param parameters The borrowed parameters to copy.
 * @return Independent C++ parameters.
 */
ProtocolParameters copyProtocolParametersFromNative(const cardano_protocol_parameters_t* parameters);
} // namespace Cometa
