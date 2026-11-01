/**
 * \file value_types.cpp
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

#include <cometa/cometa.hpp>
#include <iostream>

int
main()
{
  using namespace Biglup::Cometa;
  const auto keyHash = Blake2bHash::fromHex("00000000000000000000000000000000000000000000000000000000");
  Credential original(CredentialType::KeyHash, keyHash);
  auto       copy = original;
  copy.setType(CredentialType::ScriptHash);
  std::cout << "Original remains a key credential: " << (original.type() == CredentialType::KeyHash) << '\n';
  std::cout << "Asset: " << AssetName::fromString("Cometa").toHex() << '\n';
}
