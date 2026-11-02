/**
 * @file SlotConfigTest.cpp
 *
 * @brief Tests for slot and Unix-time conversion.
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
#include <cardano/time.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <stdexcept>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(SlotConfig, MatchesPredefinedNetworks)
{
  // Arrange
  const SlotConfig mainnet = SlotConfig::forNetwork(NetworkMagic::Mainnet);
  const SlotConfig preview = SlotConfig::forNetwork(NetworkMagic::Preview);
  const SlotConfig preprod = SlotConfig::forNetwork(NetworkMagic::Preprod);

  // Assert
  EXPECT_EQ(mainnet.zeroTime, 1596059091000ULL);
  EXPECT_EQ(mainnet.zeroSlot, 4492800);
  EXPECT_EQ(preview.zeroTime, 1666656000000ULL);
  EXPECT_EQ(preview.zeroSlot, 0);
  EXPECT_EQ(preprod.zeroTime, 1655769600000ULL);
  EXPECT_EQ(preprod.zeroSlot, 86400);
  EXPECT_EQ(mainnet.slotLength, 1000);
  EXPECT_EQ(preview.slotLength, 1000);
  EXPECT_EQ(preprod.slotLength, 1000);
}

TEST(SlotConfig, MatchesCardanoTimeConversion)
{
  // Arrange
  const NetworkMagic networks[] = { NetworkMagic::Mainnet, NetworkMagic::Preprod, NetworkMagic::Preview };

  for (NetworkMagic network: networks)
  {
    const SlotConfig    config  = SlotConfig::forNetwork(network);
    const std::uint64_t seconds = config.zeroTime / 1000 + 12345;

    // Act
    const std::uint64_t slot = config.slotFromUnixTime(seconds);

    // Assert
    EXPECT_EQ(slot, cardano_compute_slot_from_unix_time(static_cast<cardano_network_magic_t>(network), seconds));
    EXPECT_EQ(config.unixTimeFromSlot(slot), cardano_compute_unix_time_from_slot(static_cast<cardano_network_magic_t>(network), slot));
    EXPECT_EQ(config.unixTimeFromSlot(slot), seconds);
  }
}

TEST(SlotConfig, UsesMillisecondResolutionAndEnclosingSlots)
{
  // Arrange
  const SlotConfig config { 1000, 10, 250 };

  // Act & Assert
  EXPECT_EQ(config.slotFromUnixTimeMilliseconds(1000), 10);
  EXPECT_EQ(config.slotFromUnixTimeMilliseconds(1249), 10);
  EXPECT_EQ(config.slotFromUnixTimeMilliseconds(1250), 11);
  EXPECT_EQ(config.unixTimeMillisecondsFromSlot(11), 1250);
  EXPECT_EQ(config.unixTimeFromSlot(11), 1);
}

TEST(SlotConfig, RejectsUnsupportedNetworksAndInvalidRanges)
{
  // Arrange
  const SlotConfig config { 1000, 10, 1000 };
  const SlotConfig invalid { 0, 0, 0 };

  // Act & Assert
  EXPECT_THROW(SlotConfig::forNetwork(NetworkMagic::SanchoNet), std::invalid_argument);
  EXPECT_THROW(SlotConfig::forNetwork(static_cast<NetworkMagic>(42)), std::invalid_argument);
  EXPECT_THROW(config.slotFromUnixTimeMilliseconds(999), std::out_of_range);
  EXPECT_THROW(config.unixTimeMillisecondsFromSlot(9), std::out_of_range);
  EXPECT_THROW(invalid.slotFromUnixTimeMilliseconds(0), std::invalid_argument);
  EXPECT_THROW(invalid.unixTimeMillisecondsFromSlot(0), std::invalid_argument);
}

TEST(SlotConfig, RejectsArithmeticOverflow)
{
  // Arrange
  const std::uint64_t maximum = std::numeric_limits<std::uint64_t>::max();
  const SlotConfig    secondsOverflow { 0, 0, 1000 };
  const SlotConfig    slotOverflow { 0, maximum, 1 };
  const SlotConfig    timeOverflow { maximum, 0, 2 };

  // Act & Assert
  EXPECT_THROW(secondsOverflow.slotFromUnixTime(maximum), std::overflow_error);
  EXPECT_THROW(slotOverflow.slotFromUnixTimeMilliseconds(1), std::overflow_error);
  EXPECT_THROW(timeOverflow.unixTimeMillisecondsFromSlot(1), std::overflow_error);
  EXPECT_EQ(slotOverflow.slotFromUnixTimeMilliseconds(0), maximum);
  EXPECT_EQ(timeOverflow.unixTimeMillisecondsFromSlot(0), maximum);
}
