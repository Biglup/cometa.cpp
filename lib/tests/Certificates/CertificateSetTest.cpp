/**
 * @file CertificateSetTest.cpp
 *
 * @brief Unit tests for certificate collections.
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

#include <Cometa/Cometa.h>

#include <gtest/gtest.h>

/* USINGS ********************************************************************/

using namespace Cometa;

/* CONSTANTS *****************************************************************/

static const std::string CERTIFICATE1 = "83078200581c0000000000000000000000000000000000000000000000000000000000";
static const std::string CERTIFICATE2 = "83088200581c0000000000000000000000000000000000000000000000000000000000";
static const std::string CERTIFICATE3 = "830f8200581c00000000000000000000000000000000000000000000000000000000f6";
static const std::string CERTIFICATE4 = "83028200581ccb0ec2692497b458e46812c8a5bfa2931d1a2d965a99893828ec810f581cd85087c646951407198c27b1b950fd2e99f28586c000ce39f6e6ef92";
static const std::string SET_CBOR     = "d9010284" + CERTIFICATE1 + CERTIFICATE2 + CERTIFICATE3 + CERTIFICATE4;

/* IMPLEMENTATION ************************************************************/

TEST(CertificateSet, MatchesNativeCborFixture)
{
  // Arrange
  const Certificate first  = Certificate::fromCbor(CERTIFICATE1);
  const Certificate second = Certificate::fromCbor(CERTIFICATE2);
  const Certificate third  = Certificate::fromCbor(CERTIFICATE3);
  const Certificate fourth = Certificate::fromCbor(CERTIFICATE4);
  CborReader        reader(SET_CBOR);
  CertificateSet    set;

  // Act
  set.add(first);
  set.add(second);
  set.add(third);
  set.add(fourth);

  // Assert
  EXPECT_EQ(set.size(), 4);
  EXPECT_EQ(set.toCbor(), reader.readEncodedValue());
  EXPECT_EQ(set.at(0), first);
  EXPECT_EQ(set.at(3), fourth);
  EXPECT_EQ(CertificateSet::fromCbor(SET_CBOR), set);
  EXPECT_FALSE(set.toJson().empty());
}

TEST(CertificateSet, CopyAndAccessAreIndependent)
{
  // Arrange
  CertificateSet    set;
  const Certificate first = Certificate::fromCbor(CERTIFICATE1);
  set.add(first);
  const CertificateSet copy(set);
  CertificateSet       assigned;

  // Act
  assigned = set;
  set.add(Certificate::fromCbor(CERTIFICATE2));
  const Certificate retrieved = set.at(0);

  // Assert
  EXPECT_EQ(copy.size(), 1);
  EXPECT_EQ(assigned.size(), 1);
  EXPECT_EQ(retrieved, first);
  EXPECT_EQ(set.size(), 2);
  EXPECT_THROW(set.at(2), std::out_of_range);
}
