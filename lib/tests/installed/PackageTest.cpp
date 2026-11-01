/**
 * @file PackageTest.cpp
 *
 * @brief Verifies the installed package headers and static libraries.
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

/* INCLUDES ******************************************************************/

#include <Cometa/Cometa.h>

/* IMPLEMENTATION ************************************************************/

int
main()
{
  const Cometa::AssetName name = Cometa::AssetName::fromCbor("46436f6d657461");

  return name.toString() == "Cometa" ? 0 : 1;
}
