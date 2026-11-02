/**
 * @file PlutusDataKindTest.cpp
 *
 * @brief Unit tests for PlutusDataKind.
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

#include <cardano/plutus_data/plutus_data_kind.h>

#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <limits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* IMPLEMENTATION ************************************************************/

TEST(PlutusDataKind, MatchesNativeDiscriminators)
{
  // Act & Assert
  EXPECT_EQ(static_cast<int>(PlutusDataKind::Constructor), CARDANO_PLUTUS_DATA_KIND_CONSTR);
  EXPECT_EQ(static_cast<int>(PlutusDataKind::Map), CARDANO_PLUTUS_DATA_KIND_MAP);
  EXPECT_EQ(static_cast<int>(PlutusDataKind::List), CARDANO_PLUTUS_DATA_KIND_LIST);
  EXPECT_EQ(static_cast<int>(PlutusDataKind::Integer), CARDANO_PLUTUS_DATA_KIND_INTEGER);
  EXPECT_EQ(static_cast<int>(PlutusDataKind::Bytes), CARDANO_PLUTUS_DATA_KIND_BYTES);
}
