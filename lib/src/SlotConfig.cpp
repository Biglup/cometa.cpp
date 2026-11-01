/**
 * @file SlotConfig.cpp
 *
 * @brief Checked conversion between slots and Unix time.
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

#include <Cometa/SlotConfig.h>

#include <cardano/slot_config.h>

#include <limits>
#include <stdexcept>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
SlotConfig
SlotConfig::forNetwork(NetworkMagic network)
{
  switch (network)
  {
    case NetworkMagic::Mainnet:
      return { CARDANO_MAINNET_SLOT_CONFIG.zero_time, CARDANO_MAINNET_SLOT_CONFIG.zero_slot, CARDANO_MAINNET_SLOT_CONFIG.slot_length };
    case NetworkMagic::Preview:
      return { CARDANO_PREVIEW_SLOT_CONFIG.zero_time, CARDANO_PREVIEW_SLOT_CONFIG.zero_slot, CARDANO_PREVIEW_SLOT_CONFIG.slot_length };
    case NetworkMagic::Preprod:
      return { CARDANO_PREPROD_SLOT_CONFIG.zero_time, CARDANO_PREPROD_SLOT_CONFIG.zero_slot, CARDANO_PREPROD_SLOT_CONFIG.slot_length };
    default:
      throw std::invalid_argument("No predefined slot configuration for this network");
  }
}

std::uint64_t
SlotConfig::slotFromUnixTime(std::uint64_t unixTime) const
{
  if (unixTime > std::numeric_limits<std::uint64_t>::max() / 1000)
  {
    throw std::overflow_error("Unix timestamp exceeds the millisecond range");
  }

  return slotFromUnixTimeMilliseconds(unixTime * 1000);
}

std::uint64_t
SlotConfig::unixTimeFromSlot(std::uint64_t slot) const
{
  return unixTimeMillisecondsFromSlot(slot) / 1000;
}

std::uint64_t
SlotConfig::slotFromUnixTimeMilliseconds(std::uint64_t unixTime) const
{
  if (slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be greater than zero");
  }

  if (unixTime < zeroTime)
  {
    throw std::out_of_range("Unix timestamp precedes the reference time");
  }

  const std::uint64_t elapsedSlots = (unixTime - zeroTime) / slotLength;

  if (elapsedSlots > std::numeric_limits<std::uint64_t>::max() - zeroSlot)
  {
    throw std::overflow_error("Slot number exceeds the supported range");
  }

  return zeroSlot + elapsedSlots;
}

std::uint64_t
SlotConfig::unixTimeMillisecondsFromSlot(std::uint64_t slot) const
{
  if (slotLength == 0)
  {
    throw std::invalid_argument("Slot duration must be greater than zero");
  }

  if (slot < zeroSlot)
  {
    throw std::out_of_range("Slot precedes the reference slot");
  }

  const std::uint64_t elapsedSlots = slot - zeroSlot;

  if (elapsedSlots > (std::numeric_limits<std::uint64_t>::max() - zeroTime) / slotLength)
  {
    throw std::overflow_error("Unix timestamp exceeds the supported range");
  }

  return zeroTime + elapsedSlots * slotLength;
}
} // namespace Cometa
