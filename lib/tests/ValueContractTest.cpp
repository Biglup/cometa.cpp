/**
 * @file ValueContractTest.cpp
 *
 * @brief Unit tests for value semantics, serialization, and allocation failures.
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

#include <src/allocators.h>

#include <gtest/gtest.h>

#include <cstdlib>
#include <limits>
#include <type_traits>
#include <utility>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
const std::string HASH_HEX(56, '0');

Blake2bHash
keyHash()
{
  return Blake2bHash::fromHex(HASH_HEX);
}

std::size_t failureIndex    = 0;
std::size_t allocationIndex = 0;

void*
failingMalloc(std::size_t size)
{
  return allocationIndex++ == failureIndex ? nullptr : std::malloc(size);
}

class AllocationFailure
{
  public:

    explicit AllocationFailure(std::size_t index)
    {
      allocationIndex = 0;
      failureIndex    = index;
      cardano_set_allocators(failingMalloc, std::realloc, std::free);
    }

    ~AllocationFailure()
    {
      cardano_set_allocators(std::malloc, std::realloc, std::free);
    }

    AllocationFailure(const AllocationFailure&)            = delete;
    AllocationFailure& operator=(const AllocationFailure&) = delete;
};
template<class T>
T sampleValue();

template<>
Blake2bHash
sampleValue<Blake2bHash>()
{
  return keyHash();
}

template<>
AssetName
sampleValue<AssetName>()
{
  return AssetName::fromString("Cometa");
}

template<>
Credential
sampleValue<Credential>()
{
  return Credential(CredentialType::KeyHash, keyHash());
}

template<class T>
class ValueContract : public testing::Test
{
};

template<>
ExUnits
sampleValue<ExUnits>()
{
  return ExUnits(100, 200);
}

template<>
ProtocolVersion
sampleValue<ProtocolVersion>()
{
  return ProtocolVersion(12, 0);
}

template<>
UnitInterval
sampleValue<UnitInterval>()
{
  return UnitInterval(3, 4);
}

template<>
Anchor
sampleValue<Anchor>()
{
  return Anchor("https://example.com", Blake2bHash::fromHex(std::string(64, '0')));
}

template<>
GovernanceActionId
sampleValue<GovernanceActionId>()
{
  return GovernanceActionId(Blake2bHash::fromHex(std::string(64, '0')), 1);
}

template<>
BigInt
sampleValue<BigInt>()
{
  return BigInt::fromString("340282366920938463463374607431768211456");
}

using ValueTypes = testing::Types<Blake2bHash, AssetName, Credential, ExUnits, ProtocolVersion, UnitInterval, Anchor, GovernanceActionId>;
TYPED_TEST_SUITE(ValueContract, ValueTypes);
} // namespace

/* IMPLEMENTATION ************************************************************/

TEST(Credential, FailedCopyAssignmentPreservesDestination)
{
  // Arrange
  const Credential source(CredentialType::KeyHash, keyHash());
  Credential       destination(CredentialType::ScriptHash, Blake2bHash::fromHex(std::string(56, '1')));
  const Credential before          = destination;
  std::size_t      allocationCount = 0;
  {
    AllocationFailure count(std::numeric_limits<std::size_t>::max());
    const Credential  copy = source;
    allocationCount        = allocationIndex;
  }
  ASSERT_GT(allocationCount, 0);

  // Act & Assert
  for (std::size_t i = 0; i < allocationCount; ++i)
  {
    {
      AllocationFailure fail(i);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }
    EXPECT_EQ(destination, before);
    EXPECT_EQ(source.type(), CredentialType::KeyHash);
  }
}

TYPED_TEST(ValueContract, SupportsCopyAndMove)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();

  // Act
  const TypeParam copied = source;
  const TypeParam moved  = std::move(source);

  // Assert
  EXPECT_EQ(copied, moved);
  EXPECT_THROW(source.toCbor(), std::logic_error);
  static_assert(std::is_nothrow_move_constructible_v<TypeParam>);
  static_assert(std::is_nothrow_move_assignable_v<TypeParam>);
}

TYPED_TEST(ValueContract, SupportsCopySelfAssignment)
{
  // Arrange
  TypeParam        source = sampleValue<TypeParam>();
  const TypeParam  before = source;
  const TypeParam* alias  = &source;

  // Act
  source = *alias;

  // Assert
  EXPECT_EQ(source, before);
}

TYPED_TEST(ValueContract, SupportsMoveSelfAssignment)
{
  // Arrange
  TypeParam       source = sampleValue<TypeParam>();
  const TypeParam before = source;
  TypeParam*      alias  = &source;

  // Act
  source = std::move(*alias);

  // Assert
  EXPECT_EQ(source, before);
}

TYPED_TEST(ValueContract, SupportsAssignmentToMovedFromValues)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();
  TypeParam moved  = std::move(source);

  // Act
  source = moved;
  moved  = std::move(source);

  // Assert
  EXPECT_EQ(moved, sampleValue<TypeParam>());
  EXPECT_THROW(source.toCbor(), std::logic_error);
}

TYPED_TEST(ValueContract, SupportsSwapWithMovedFromValues)
{
  // Arrange
  TypeParam source = sampleValue<TypeParam>();
  TypeParam moved  = std::move(source);

  // Act
  swap(source, moved);

  // Assert
  EXPECT_EQ(source, sampleValue<TypeParam>());
  EXPECT_THROW(moved.toCbor(), std::logic_error);
}

TYPED_TEST(ValueContract, ReleasesPartialCopiesOnEveryAllocationFailure)
{
  // Arrange
  const TypeParam source = sampleValue<TypeParam>();
  std::size_t     count  = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   copy(source);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW({ const TypeParam copy(source); }, std::bad_alloc);
  }
  EXPECT_EQ(source, sampleValue<TypeParam>());
}

TYPED_TEST(ValueContract, ReleasesParserStateOnAllocationFailure)
{
  // Arrange
  const std::vector<std::uint8_t> cbor  = sampleValue<TypeParam>().toCbor();
  std::size_t                     count = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    const TypeParam   parsed = TypeParam::fromCbor(cbor);
    count                    = allocationIndex;
  }

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    AllocationFailure fail(i);
    try
    {
      const TypeParam parsed = TypeParam::fromCbor(cbor);
      // Optional native caches can fail without invalidating the parsed value.
      EXPECT_EQ(parsed.toCbor(), cbor);
    }
    catch (const std::bad_alloc&)
    {
    }
    catch (const Error& error)
    {
      // Some native parser paths translate allocation errors into CBOR errors.
      EXPECT_NE(error.code(), makeErrorCode(ErrorCode::Success));
    }
  }
}

TYPED_TEST(ValueContract, ReleasesStateWhenConstructionFails)
{
  // Arrange
  AllocationFailure fail(0);

  // Act & Assert
  EXPECT_THROW(sampleValue<TypeParam>(), std::bad_alloc);
}

TYPED_TEST(ValueContract, FailedCopyAssignmentPreservesDestination)
{
  // Arrange
  const TypeParam source      = sampleValue<TypeParam>();
  TypeParam       destination = sampleValue<TypeParam>();
  const TypeParam before      = destination;
  std::size_t     count       = 0;
  {
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    destination = source;
    count       = allocationIndex;
  }

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    {
      AllocationFailure fail(i);
      EXPECT_THROW(destination = source, std::bad_alloc);
    }
    EXPECT_EQ(destination, before);
  }
}

TEST(Anchor, FailedMutationPreservesValue)
{
  // Arrange
  const Anchor before = sampleValue<Anchor>();
  std::size_t  count  = 0;
  {
    Anchor            value = before;
    AllocationFailure tracker(std::numeric_limits<std::size_t>::max());
    value.setUrl("https://changed.example");
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    Anchor value = before;
    {
      AllocationFailure fail(i);
      EXPECT_THROW(value.setUrl("https://changed.example"), std::bad_alloc);
    }
    EXPECT_EQ(value, before);
    EXPECT_EQ(value.toCbor(), before.toCbor());
  }
}

TEST(GovernanceActionId, FailedMutationPreservesValue)
{
  // Arrange
  const GovernanceActionId before = sampleValue<GovernanceActionId>();
  std::size_t              count  = 0;
  {
    GovernanceActionId value = before;
    AllocationFailure  tracker(std::numeric_limits<std::size_t>::max());
    value.setIndex(2);
    count = allocationIndex;
  }
  ASSERT_GT(count, 0);

  // Act & Assert
  for (std::size_t i = 0; i < count; ++i)
  {
    GovernanceActionId value = before;
    {
      AllocationFailure fail(i);
      EXPECT_THROW(value.setIndex(2), std::bad_alloc);
    }
    EXPECT_EQ(value, before);
    EXPECT_EQ(value.toCbor(), before.toCbor());
  }
}

TEST(UnitInterval, FloatingPointConversionReleasesStateOnFailure)
{
  // Arrange
  AllocationFailure fail(0);

  // Act & Assert
  EXPECT_THROW(UnitInterval::fromDouble(0.75), std::bad_alloc);
}

TEST(BigInt, FailedArithmeticPreservesOperands)
{
  // Arrange
  BigInt       value  = sampleValue<BigInt>();
  const BigInt before = value;

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(value *= value, std::bad_alloc);
  }
  EXPECT_EQ(value, before);
}

TEST(BigInt, FailedDivisionReleasesPartialResults)
{
  // Arrange
  const BigInt value = sampleValue<BigInt>();
  const BigInt divisor(7);

  // Act & Assert
  for (std::size_t i = 0; i < 2; ++i)
  {
    AllocationFailure fail(i);
    EXPECT_THROW(value.divRem(divisor), std::bad_alloc);
  }
}

TEST(BigInt, FailedCopyPreservesDestination)
{
  // Arrange
  const BigInt source = sampleValue<BigInt>();
  BigInt       destination(-7);

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(destination = source, std::bad_alloc);
  }
  EXPECT_EQ(destination, BigInt(-7));
  EXPECT_EQ(source, sampleValue<BigInt>());
}

TEST(BigInt, ConstructorsReleaseStateOnFailure)
{
  // Arrange
  const std::vector<std::uint8_t> bytes { 1, 2, 3 };

  // Act & Assert
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt(1), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromUnsigned(1), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromString("12345678901234567890"), std::bad_alloc);
  }
  {
    AllocationFailure fail(0);
    EXPECT_THROW(BigInt::fromBytes(bytes), std::bad_alloc);
  }
}
