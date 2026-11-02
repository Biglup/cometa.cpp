/**
 * @file RedeemerTag.cpp
 *
 * @brief Script purposes for redeemers.
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

#include <Cometa/WitnessSet/RedeemerTag.h>

#include <stdexcept>

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
std::string
toString(RedeemerTag value)
{
  switch (value)
  {
    case RedeemerTag::Spend:
      return "spend";
    case RedeemerTag::Mint:
      return "mint";
    case RedeemerTag::Certifying:
      return "certifying";
    case RedeemerTag::Reward:
      return "reward";
    case RedeemerTag::Voting:
      return "voting";
    case RedeemerTag::Proposing:
      return "proposing";
    case RedeemerTag::Guarding:
      return "guarding";
  }

  throw std::invalid_argument("Invalid redeemer purpose");
}
} // namespace Cometa
