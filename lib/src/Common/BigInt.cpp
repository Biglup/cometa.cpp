/**
 * @file BigInt.cpp
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

/* INCLUDES ******************************************************************/

#include <Cometa/Common/BigInt.h>

#include "../CardanoInterop.h"

#include <cardano/common/bigint.h>

#include <charconv>
#include <limits>

#include <utility>

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace Cometa
{
namespace
{
/**
 * @brief Validates a supported positional radix.
 * @param base The radix; values outside 2 through 62 throw std::invalid_argument.
 */
void
validateBase(int base)
{
  if (base < 2 || base > 62)
  {
    throw std::invalid_argument("BigInt base must be between 2 and 62");
  }
}

/**
 * @brief Converts a checked byte-order enum.
 * @param order The requested order; unknown values throw std::invalid_argument.
 * @return The corresponding native order.
 */
cardano_byte_order_t
nativeByteOrder(ByteOrder order)
{
  switch (order)
  {
    case ByteOrder::LittleEndian:
      return CARDANO_BYTE_ORDER_LITTLE_ENDIAN;
    case ByteOrder::BigEndian:
      return CARDANO_BYTE_ORDER_BIG_ENDIAN;
  }

  throw std::invalid_argument("Unknown byte order");
}
} // namespace

/* CLASS IMPLEMENTATION ******************************************************/

BigInt::BigInt(std::int64_t value)
: m_value(nullptr)
{
  const std::uint64_t   magnitude = value < 0 ? static_cast<std::uint64_t>(-(value + 1)) + 1 : static_cast<std::uint64_t>(value);
  cardano_bigint_t*     parsed    = nullptr;
  const cardano_error_t result    = cardano_bigint_from_unsigned_int(magnitude, &parsed);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(parsed);

  checkCardanoError(result, "BigInt");

  if (value < 0)
  {
    cardano_bigint_negate(owner.get(), owner.get());
  }

  m_value = owner.release();
}

BigInt::BigInt(const BigInt& other)
: m_value(nullptr)
{
  cardano_bigint_t*     copied = nullptr;
  const cardano_error_t result = cardano_bigint_deep_clone(other.checked(), &copied);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(copied);

  checkCardanoError(result, "BigInt copy");

  m_value = owner.release();
}

BigInt::BigInt(BigInt&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

BigInt::~BigInt()
{
  cardano_bigint_unref(&m_value);
}

BigInt
BigInt::fromUnsigned(std::uint64_t value)
{
  cardano_bigint_t*     parsed = nullptr;
  const cardano_error_t result = cardano_bigint_from_unsigned_int(value, &parsed);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(parsed);

  checkCardanoError(result, "BigInt fromUnsigned");

  return BigInt(*owner.release());
}

BigInt
BigInt::fromString(std::string_view text, int base)
{
  validateBase(base);

  if (text.empty())
  {
    throw std::invalid_argument("BigInt digits must not be empty");
  }

  const std::size_t start = text.front() == '-' || text.front() == '+' ? 1 : 0;

  if (start == text.size())
  {
    throw std::invalid_argument("BigInt sign must be followed by digits");
  }

  for (std::size_t i = start; i < text.size(); ++i)
  {
    const char digit = text[i];
    int        value = 62;

    if (digit >= '0' && digit <= '9')
    {
      value = digit - '0';
    }
    else if (digit >= 'A' && digit <= 'Z')
    {
      value = digit - 'A' + 10;
    }
    else if (digit >= 'a' && digit <= 'z')
    {
      value = digit - 'a' + (base > 36 ? 36 : 10);
    }

    if (value >= base)
    {
      throw std::invalid_argument("Invalid BigInt digit");
    }
  }

  const std::string     terminated(text.front() == '+' ? text.substr(1) : text);
  cardano_bigint_t*     parsed = nullptr;
  const cardano_error_t result = cardano_bigint_from_string(terminated.c_str(), terminated.size(), base, &parsed);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(parsed);

  checkCardanoError(result, "BigInt fromString");

  return BigInt(*owner.release());
}

BigInt
BigInt::fromBytes(const std::vector<std::uint8_t>& bytes, ByteOrder order)
{
  const cardano_byte_order_t byteOrder = nativeByteOrder(order);
  cardano_bigint_t*          parsed    = nullptr;
  const cardano_error_t      result    = cardano_bigint_from_bytes(byteData(bytes), bytes.size(), byteOrder, &parsed);

  CardanoObject<cardano_bigint_t, cardano_bigint_unref> owner(parsed);

  checkCardanoError(result, "BigInt fromBytes");

  return BigInt(*owner.release());
}

std::string
BigInt::toString(int base) const
{
  validateBase(base);

  const cardano_bigint_t* value = checked();
  std::string             text(cardano_bigint_get_string_size(value, base), '\0');

  checkCardanoError(cardano_bigint_to_string(value, text.data(), text.size(), base), "BigInt toString");

  text.pop_back();

  return text;
}

std::int64_t
BigInt::toInt64() const
{
  const std::string            text   = toString();
  std::int64_t                 value  = 0;
  const std::from_chars_result parsed = std::from_chars(text.data(), text.data() + text.size(), value);

  if (parsed.ec != std::errc() || parsed.ptr != text.data() + text.size())
  {
    throw std::overflow_error("BigInt exceeds the signed 64-bit range");
  }

  return value;
}

std::uint64_t
BigInt::toUInt64() const
{
  const std::string            text   = toString();
  std::uint64_t                value  = 0;
  const std::from_chars_result parsed = std::from_chars(text.data(), text.data() + text.size(), value);

  if (parsed.ec != std::errc() || parsed.ptr != text.data() + text.size())
  {
    throw std::overflow_error("BigInt is outside the unsigned 64-bit range");
  }

  return value;
}

std::vector<std::uint8_t>
BigInt::toBytes(ByteOrder order) const
{
  const cardano_byte_order_t byteOrder = nativeByteOrder(order);
  const cardano_bigint_t*    value     = checked();

  if (cardano_bigint_is_zero(value))
  {
    return { 0 };
  }

  std::vector<std::uint8_t> bytes(cardano_bigint_get_bytes_size(value));

  checkCardanoError(cardano_bigint_to_bytes(value, byteOrder, bytes.data(), bytes.size()), "BigInt toBytes");

  return bytes;
}

int
BigInt::sign() const
{
  return cardano_bigint_signum(checked());
}

bool
BigInt::isZero() const
{
  return cardano_bigint_is_zero(checked());
}

BigInt
BigInt::abs() const
{
  BigInt result;

  cardano_bigint_abs(checked(), result.checked());

  return result;
}

std::size_t
BigInt::bitCount() const
{
  return cardano_bigint_bit_count(checked());
}

std::size_t
BigInt::bitLength() const
{
  return cardano_bigint_bit_length(checked());
}

bool
BigInt::testBit(std::uint32_t position) const
{
  return cardano_bigint_test_bit(checked(), position);
}

void
BigInt::setBit(std::uint32_t position)
{
  cardano_bigint_set_bit(checked(), position);
}

void
BigInt::clearBit(std::uint32_t position)
{
  cardano_bigint_clear_bit(checked(), position);
}

void
BigInt::flipBit(std::uint32_t position)
{
  cardano_bigint_flip_bit(checked(), position);
}

BigInt
BigInt::gcd(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_gcd(checked(), other.checked(), result.checked());

  return result;
}

BigInt
BigInt::pow(std::uint64_t exponent) const
{
  BigInt factor(*this);
  BigInt result(1);

  // Keep the full exponent width on platforms where unsigned long is 32 bits.
  while (exponent != 0)
  {
    if ((exponent & 1) != 0)
    {
      result *= factor;
    }

    exponent >>= 1;

    if (exponent != 0)
    {
      factor *= factor;
    }
  }

  return result;
}

BigInt
BigInt::modPow(const BigInt& exponent, const BigInt& modulus) const
{
  if (modulus.sign() <= 0)
  {
    throw std::domain_error("Modulus must be positive");
  }

  const cardano_bigint_t* baseValue     = checked();
  const cardano_bigint_t* exponentValue = exponent.checked();

  if (modulus == BigInt(1))
  {
    return BigInt(0);
  }

  if (exponent.sign() < 0 && gcd(modulus) != BigInt(1))
  {
    throw std::domain_error("Negative modular exponent requires an invertible base");
  }

  BigInt result;

  cardano_bigint_mod_pow(baseValue, exponentValue, modulus.checked(), result.checked());

  return result;
}

BigInt
BigInt::modInverse(const BigInt& modulus) const
{
  if (modulus.sign() <= 0 || gcd(modulus) != BigInt(1))
  {
    throw std::domain_error("Modular inverse requires a positive coprime modulus");
  }

  BigInt result;

  cardano_bigint_mod_inverse(checked(), modulus.checked(), result.checked());

  return result;
}

std::pair<BigInt, BigInt>
BigInt::divRem(const BigInt& divisor) const
{
  if (divisor.isZero())
  {
    throw std::domain_error("Cannot divide by zero");
  }

  BigInt quotient;
  BigInt remainder;

  cardano_bigint_divide_and_reminder(checked(), divisor.checked(), quotient.checked(), remainder.checked());

  return { std::move(quotient), std::move(remainder) };
}

void
BigInt::swap(BigInt& other) noexcept
{
  std::swap(m_value, other.m_value);
}

BigInt&
BigInt::operator=(const BigInt& other)
{
  if (this != &other)
  {
    BigInt copy(other);

    swap(copy);
  }

  return *this;
}

BigInt&
BigInt::operator=(BigInt&& other) noexcept
{
  if (this != &other)
  {
    cardano_bigint_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

BigInt
BigInt::operator+(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_add(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator+=(const BigInt& other)
{
  BigInt result = *this + other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator-(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_subtract(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator-=(const BigInt& other)
{
  BigInt result = *this - other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator*(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_multiply(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator*=(const BigInt& other)
{
  BigInt result = *this * other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator/(const BigInt& other) const
{
  if (other.isZero())
  {
    throw std::domain_error("Cannot divide by zero");
  }

  BigInt result;

  cardano_bigint_divide(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator/=(const BigInt& other)
{
  BigInt result = *this / other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator%(const BigInt& other) const
{
  if (other.isZero())
  {
    throw std::domain_error("Cannot divide by zero");
  }

  BigInt result;

  cardano_bigint_reminder(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator%=(const BigInt& other)
{
  BigInt result = *this % other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator&(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_and(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator&=(const BigInt& other)
{
  BigInt result = *this & other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator|(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_or(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator|=(const BigInt& other)
{
  BigInt result = *this | other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator^(const BigInt& other) const
{
  BigInt result;

  cardano_bigint_xor(checked(), other.checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator^=(const BigInt& other)
{
  BigInt result = *this ^ other;

  swap(result);

  return *this;
}

BigInt
BigInt::operator<<(std::uint32_t bits) const
{
  BigInt result;

  cardano_bigint_shift_left(checked(), bits, result.checked());

  return result;
}

BigInt&
BigInt::operator<<=(std::uint32_t bits)
{
  BigInt result = *this << bits;

  swap(result);

  return *this;
}

BigInt
BigInt::operator>>(std::uint32_t bits) const
{
  BigInt result;

  cardano_bigint_shift_right(checked(), bits, result.checked());

  return result;
}

BigInt&
BigInt::operator>>=(std::uint32_t bits)
{
  BigInt result = *this >> bits;

  swap(result);

  return *this;
}

BigInt
BigInt::operator-() const
{
  BigInt result;

  cardano_bigint_negate(checked(), result.checked());

  return result;
}

BigInt
BigInt::operator~() const
{
  BigInt result;

  cardano_bigint_not(checked(), result.checked());

  return result;
}

BigInt&
BigInt::operator++()
{
  cardano_bigint_increment(checked());

  return *this;
}

BigInt
BigInt::operator++(int)
{
  BigInt previous(*this);

  ++(*this);

  return previous;
}

BigInt&
BigInt::operator--()
{
  cardano_bigint_decrement(checked());

  return *this;
}

BigInt
BigInt::operator--(int)
{
  BigInt previous(*this);

  --(*this);

  return previous;
}

bool
BigInt::operator==(const BigInt& other) const
{
  return cardano_bigint_compare(checked(), other.checked()) == 0;
}

bool
BigInt::operator!=(const BigInt& other) const
{
  return cardano_bigint_compare(checked(), other.checked()) != 0;
}

bool
BigInt::operator<(const BigInt& other) const
{
  return cardano_bigint_compare(checked(), other.checked()) < 0;
}

bool
BigInt::operator<=(const BigInt& other) const
{
  return cardano_bigint_compare(checked(), other.checked()) <= 0;
}

bool
BigInt::operator>(const BigInt& other) const
{
  return cardano_bigint_compare(checked(), other.checked()) > 0;
}

bool
BigInt::operator>=(const BigInt& other) const
{
  return cardano_bigint_compare(checked(), other.checked()) >= 0;
}

BigInt
BigInt::adopt(cardano_bigint_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null BigInt");
  }

  return BigInt(*value);
}

cardano_bigint_t*
BigInt::nativeHandle() noexcept
{
  return m_value;
}

const cardano_bigint_t*
BigInt::nativeHandle() const noexcept
{
  return m_value;
}

cardano_bigint_t*
BigInt::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

BigInt::BigInt(cardano_bigint_t& value) noexcept
: m_value(&value)
{
}

cardano_bigint_t*
BigInt::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from BigInt");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(BigInt& lhs, BigInt& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
