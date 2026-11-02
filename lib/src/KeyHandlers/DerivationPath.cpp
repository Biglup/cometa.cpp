/**
 * @file DerivationPath.cpp
 *
 * @brief BIP32 path to a Cardano key.
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

/* INCLUDES ******************************************************************/

#include <Cometa/KeyHandlers/DerivationPath.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
AccountDerivationPath
DerivationPath::accountPath() const noexcept
{
  return { purpose, coinType, account };
}

bool
DerivationPath::operator==(const DerivationPath& other) const noexcept
{
  return purpose == other.purpose && coinType == other.coinType && account == other.account && role == other.role && index == other.index;
}

bool
DerivationPath::operator!=(const DerivationPath& other) const noexcept
{
  return !(*this == other);
}
} // namespace Cometa
