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

#include <string>

/* IMPLEMENTATION ************************************************************/

int
main()
{
  const Cometa::AssetName name = Cometa::AssetName::fromCbor("46436f6d657461");

  const Cometa::ExUnits         budget = Cometa::ExUnits::fromCbor("820102");
  const Cometa::ProtocolVersion version(12, 0);
  const Cometa::SlotConfig      slots = Cometa::SlotConfig::forNetwork(Cometa::NetworkMagic::Preview);

  const Cometa::UnitInterval       interval = Cometa::UnitInterval::fromDouble(0.75);
  const Cometa::Blake2bHash        hash     = Cometa::Blake2bHash::fromHex(std::string(64, '0'));
  const Cometa::Anchor             anchor("https://example.com", hash);
  const Cometa::GovernanceActionId action(hash, 1);

  if (interval.numerator() != 3 || anchor.hash() != hash || Cometa::GovernanceActionId::fromBech32(action.toBech32()) != action)
  {
    return 1;
  }

  const Cometa::BigInt integer = Cometa::BigInt(2).pow(128);

  if (integer.toString() != "340282366920938463463374607431768211456")
  {
    return 1;
  }

  const Cometa::Bech32Data decoded = Cometa::decodeBech32("a12uel5l");

  if (Cometa::encodeBase58({ 58 }) != "21" || decoded.prefix != "a" || !decoded.bytes.empty() || Cometa::encodeBech32(decoded.prefix, decoded.bytes) != "a12uel5l")
  {
    return 1;
  }

  return name.toString() == "Cometa" && budget.cpuSteps() == 2 && version.major() == 12 &&
      slots.unixTimeFromSlot(0) == 1666656000ULL
    ? 0
    : 1;
}
