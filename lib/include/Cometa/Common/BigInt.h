/**
 * @file BigInt.h
 *
 * @brief Arbitrary-precision signed integer arithmetic.
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

#pragma once

/* INCLUDES ******************************************************************/

#include <Cometa/Common/ByteOrder.h>

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

/* FORWARD DECLARATIONS ******************************************************/

struct cardano_bigint_t;

/* CLASS DECLARATION *********************************************************/

namespace Cometa
{
/**
 * @brief An arbitrary-precision signed integer.
 *
 * Division truncates toward zero; remainders have the dividend's sign.
 * Bitwise operations use two's-complement semantics. Right shifts round toward
 * negative infinity. Byte conversions use unsigned magnitude.
 */
class BigInt
{
  private:

    /** @brief Allows Plutus data to copy integer components. */
    friend class PlutusData;

    /** @brief Allows Metadatum to copy integer values. */
    friend class Metadatum;

    /** @brief Allows CBOR decoding to adopt a decoded integer. */
    friend class CborReader;

    /** @brief Allows CBOR encoding to read the integer. */
    friend class CborWriter;

    /** @brief Allows JSON encoding to read the integer. */
    friend class JsonWriter;

  public:

    /**
     * @brief Initializes a signed integer.
     *
     * @param value The initial value.
     */
    explicit BigInt(std::int64_t value = 0);

    /**
     * @brief Initializes an independent copy.
     *
     * @param other The value to copy.
     *
     * @throws std::logic_error The source has been moved from.
     */
    BigInt(const BigInt& other);

    /**
     * @brief Moves another integer into this instance.
     *
     * @param other The source, which may subsequently be destroyed, assigned, or swapped.
     */
    BigInt(BigInt&& other) noexcept;

    /**
     * @brief Destroys this integer.
     */
    ~BigInt();

  public:

    /**
     * @brief Creates an unsigned integer.
     *
     * @param value The initial value.
     *
     * @return The integer.
     */
    static BigInt fromUnsigned(std::uint64_t value);

    /**
     * @brief Parses signed digits in the specified base.
     *
     * @param text Digits with an optional leading + or -; no whitespace or base prefix.
     * @param base The radix, from 2 through 62. Bases up to 36 ignore case; higher bases use A-Z for 10-35 and a-z for 36-61.
     *
     * @return The integer.
     *
     * @throws std::invalid_argument The base or digits are invalid.
     */
    static BigInt fromString(std::string_view text, int base = 10);

    /**
     * @brief Imports an unsigned magnitude.
     *
     * @param bytes Magnitude bytes; an empty vector represents zero.
     * @param order The byte order.
     *
     * @return The nonnegative integer.
     *
     * @throws std::invalid_argument The byte order is unknown.
     */
    static BigInt fromBytes(const std::vector<std::uint8_t>& bytes, ByteOrder order = ByteOrder::BigEndian);

  public:

    /**
     * @brief Formats signed digits in the specified base.
     *
     * @param base The radix, from 2 through 62. Bases up to 36 use lowercase digits; higher bases use A-Z then a-z.
     *
     * @return The digits, with a leading minus sign for negative values.
     *
     * @throws std::invalid_argument The base is invalid.
     */
    std::string toString(int base = 10) const;

    /**
     * @brief Converts to a signed 64-bit integer.
     *
     * @return The exact value.
     *
     * @throws std::overflow_error The value is outside the signed 64-bit range.
     */
    std::int64_t toInt64() const;

    /**
     * @brief Converts to an unsigned 64-bit integer.
     *
     * @return The exact value.
     *
     * @throws std::overflow_error The value is negative or exceeds the unsigned 64-bit range.
     */
    std::uint64_t toUInt64() const;

    /**
     * @brief Exports the unsigned magnitude.
     *
     * @param order The byte order.
     *
     * @return The magnitude without leading zero bytes; zero is encoded as one zero byte. The sign is not included.
     *
     * @throws std::invalid_argument The byte order is unknown.
     */
    std::vector<std::uint8_t> toBytes(ByteOrder order = ByteOrder::BigEndian) const;

    /**
     * @brief Gets the sign.
     *
     * @return -1 for negative values, 0 for zero, or 1 for positive values.
     */
    int sign() const;

    /**
     * @brief Tests whether this integer is zero.
     *
     * @return True for zero.
     */
    bool isZero() const;

    /**
     * @brief Gets the absolute value.
     *
     * @return The absolute value.
     */
    BigInt abs() const;

    /**
     * @brief Counts set bits in the absolute value.
     *
     * @return The number of set bits.
     */
    std::size_t bitCount() const;

    /**
     * @brief Gets the number of binary digits in the absolute value.
     *
     * @return The bit length; zero has length one.
     */
    std::size_t bitLength() const;

    /**
     * @brief Tests a bit using two's-complement semantics.
     *
     * @param position The zero-based bit position.
     *
     * @return True if the bit is set.
     */
    bool testBit(std::uint32_t position) const;

    /**
     * @brief Sets a bit using two's-complement semantics.
     *
     * @param position The zero-based bit position.
     */
    void setBit(std::uint32_t position);

    /**
     * @brief Clears a bit using two's-complement semantics.
     *
     * @param position The zero-based bit position.
     */
    void clearBit(std::uint32_t position);

    /**
     * @brief Toggles a bit using two's-complement semantics.
     *
     * @param position The zero-based bit position.
     */
    void flipBit(std::uint32_t position);

    /**
     * @brief Computes the greatest common divisor.
     *
     * @param other The other integer.
     *
     * @return The nonnegative greatest common divisor, or zero when both operands are zero.
     */
    BigInt gcd(const BigInt& other) const;

    /**
     * @brief Raises this integer to a nonnegative power.
     *
     * @param exponent The exponent.
     *
     * @return The power; zero raised to zero is one.
     */
    BigInt pow(std::uint64_t exponent) const;

    /**
     * @brief Computes a modular power.
     *
     * @param exponent The exponent; negative values use the modular inverse.
     * @param modulus A positive modulus.
     *
     * @return The result in [0, modulus).
     *
     * @throws std::domain_error The modulus is not positive, or a negative exponent has no modular inverse.
     */
    BigInt modPow(const BigInt& exponent, const BigInt& modulus) const;

    /**
     * @brief Computes the modular multiplicative inverse.
     *
     * @param modulus A positive modulus.
     *
     * @return The inverse in [0, modulus); modulus one returns zero.
     *
     * @throws std::domain_error The modulus is not positive or no inverse exists.
     */
    BigInt modInverse(const BigInt& modulus) const;

    /**
     * @brief Computes quotient and remainder together.
     *
     * @param divisor A nonzero divisor.
     *
     * @return The quotient rounded toward zero and the remainder, in that order.
     *
     * @throws std::domain_error The divisor is zero.
     */
    std::pair<BigInt, BigInt> divRem(const BigInt& divisor) const;

    /**
     * @brief Exchanges two integers.
     *
     * @param other The integer to exchange with this instance.
     */
    void swap(BigInt& other) noexcept;

  public:

    /**
     * @brief Assigns an independent copy.
     *
     * @param other The value to copy.
     *
     * @return This instance; it remains unchanged if copying fails.
     */
    BigInt& operator=(const BigInt& other);

    /**
     * @brief Moves another integer into this instance.
     *
     * @param other The source; self-move assignment is safe.
     *
     * @return This instance.
     */
    BigInt& operator=(BigInt&& other) noexcept;

    /**
     * @brief Adds another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     */
    BigInt operator+(const BigInt& other) const;

    /**
     * @brief Adds another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     */
    BigInt& operator+=(const BigInt& other);

    /**
     * @brief Subtracts another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     */
    BigInt operator-(const BigInt& other) const;

    /**
     * @brief Subtracts another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     */
    BigInt& operator-=(const BigInt& other);

    /**
     * @brief Multiplies by another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     */
    BigInt operator*(const BigInt& other) const;

    /**
     * @brief Multiplies by another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     */
    BigInt& operator*=(const BigInt& other);

    /**
     * @brief Divides by another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     *
     * @throws std::domain_error The divisor is zero.
     */
    BigInt operator/(const BigInt& other) const;

    /**
     * @brief Divides by another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     *
     * @throws std::domain_error The divisor is zero.
     */
    BigInt& operator/=(const BigInt& other);

    /**
     * @brief Computes the remainder with another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     *
     * @throws std::domain_error The divisor is zero.
     */
    BigInt operator%(const BigInt& other) const;

    /**
     * @brief Computes the remainder with another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     *
     * @throws std::domain_error The divisor is zero.
     */
    BigInt& operator%=(const BigInt& other);

    /**
     * @brief Computes the bitwise AND with another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     */
    BigInt operator&(const BigInt& other) const;

    /**
     * @brief Computes the bitwise AND with another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     */
    BigInt& operator&=(const BigInt& other);

    /**
     * @brief Computes the bitwise OR with another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     */
    BigInt operator|(const BigInt& other) const;

    /**
     * @brief Computes the bitwise OR with another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     */
    BigInt& operator|=(const BigInt& other);

    /**
     * @brief Computes the bitwise XOR with another integer.
     *
     * @param other The right operand.
     *
     * @return The result.
     */
    BigInt operator^(const BigInt& other) const;

    /**
     * @brief Computes the bitwise XOR with another integer and assigns the result.
     *
     * @param other The right operand.
     *
     * @return This instance; unchanged if the operation throws.
     */
    BigInt& operator^=(const BigInt& other);

    /**
     * @brief Shifts left.
     *
     * @param bits The number of bits.
     *
     * @return The shifted value.
     */
    BigInt operator<<(std::uint32_t bits) const;

    /**
     * @brief Shifts left and assigns the result.
     *
     * @param bits The number of bits.
     *
     * @return This instance.
     */
    BigInt& operator<<=(std::uint32_t bits);

    /**
     * @brief Shifts right, rounding toward negative infinity.
     *
     * @param bits The number of bits.
     *
     * @return The shifted value.
     */
    BigInt operator>>(std::uint32_t bits) const;

    /**
     * @brief Shifts right, rounding toward negative infinity and assigns the result.
     *
     * @param bits The number of bits.
     *
     * @return This instance.
     */
    BigInt& operator>>=(std::uint32_t bits);

    /**
     * @brief Negates this integer.
     *
     * @return The result.
     */
    BigInt operator-() const;

    /**
     * @brief Complements all bits using two's-complement semantics.
     *
     * @return The result.
     */
    BigInt operator~() const;

    /**
     * @brief Increments this integer.
     *
     * @return This instance.
     */
    BigInt& operator++();

    /**
     * @brief Increments this integer, retaining its prior value.
     *
     * @return The value before the operation.
     */
    BigInt operator++(int);

    /**
     * @brief Decrements this integer.
     *
     * @return This instance.
     */
    BigInt& operator--();

    /**
     * @brief Decrements this integer, retaining its prior value.
     *
     * @return The value before the operation.
     */
    BigInt operator--(int);

    /**
     * @brief Compares integer values.
     *
     * @param other The right operand.
     *
     * @return True when this value == other.
     */
    bool operator==(const BigInt& other) const;

    /**
     * @brief Compares integer values.
     *
     * @param other The right operand.
     *
     * @return True when this value != other.
     */
    bool operator!=(const BigInt& other) const;

    /**
     * @brief Compares integer values.
     *
     * @param other The right operand.
     *
     * @return True when this value < other.
     */
    bool operator<(const BigInt& other) const;

    /**
     * @brief Compares integer values.
     *
     * @param other The right operand.
     *
     * @return True when this value <= other.
     */
    bool operator<=(const BigInt& other) const;

    /**
     * @brief Compares integer values.
     *
     * @param other The right operand.
     *
     * @return True when this value > other.
     */
    bool operator>(const BigInt& other) const;

    /**
     * @brief Compares integer values.
     *
     * @param other The right operand.
     *
     * @return True when this value >= other.
     */
    bool operator>=(const BigInt& other) const;

  private:

    /**
     * @brief Takes ownership of an existing value.
     *
     * @param value The reference transferred to this instance.
     */
    explicit BigInt(cardano_bigint_t& value) noexcept;

  private:

    /**
     * @brief Gets the stored reference after checking its lifetime.
     *
     * @return The reference without transferring ownership.
     *
     * @throws std::logic_error This instance has been moved from.
     */
    cardano_bigint_t* checked() const;

  private:

    /**
     * @brief The owned reference, or null after a move.
     */
    cardano_bigint_t* m_value;
};
} // namespace Cometa

/* PROTOTYPES ****************************************************************/

namespace Cometa
{
/**
 * @brief Exchanges integers through argument-dependent lookup.
 *
 * @param lhs The first integer.
 * @param rhs The second integer.
 */
void swap(BigInt& lhs, BigInt& rhs) noexcept;
} // namespace Cometa
