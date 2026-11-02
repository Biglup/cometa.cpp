/**
 * @file NativeScriptType.cpp
 *
 * @brief Native script discriminator.
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

/* INCLUDES ******************************************************************/

#include <Cometa/Scripts/NativeScripts/NativeScriptType.h>

#include <cardano/scripts/native_scripts/native_script_type.h>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::string
toString(NativeScriptType value)
{
  return cardano_native_script_type_to_string(static_cast<cardano_native_script_type_t>(value));
}
} // namespace Cometa
