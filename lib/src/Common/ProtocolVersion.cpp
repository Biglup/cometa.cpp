/**
 * @file ProtocolVersion.cpp
 *
 * @brief Major and minor Cardano protocol version.
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

#include <Cometa/Common/ProtocolVersion.h>

#include "../CardanoInterop.h"

#include <cardano/common/protocol_version.h>

#include <limits>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
ProtocolVersion::ProtocolVersion(std::uint64_t major, std::uint64_t minor)
: m_value(nullptr)
{
  if (minor > std::numeric_limits<std::uint32_t>::max())
  {
    throw std::invalid_argument("Minor protocol version exceeds UINT32_MAX");
  }

  cardano_protocol_version_t* value  = nullptr;
  const cardano_error_t       result = cardano_protocol_version_new(major, minor, &value);

  CardanoObject<cardano_protocol_version_t, cardano_protocol_version_unref> owner(value);

  checkCardanoError(result, "ProtocolVersion");

  m_value = owner.release();
}

ProtocolVersion::ProtocolVersion(const ProtocolVersion& other)
: m_value(nullptr)
{
  cardano_protocol_version_t* value  = nullptr;
  const cardano_error_t       result = cardano_protocol_version_deep_clone(other.checked(), &value);

  CardanoObject<cardano_protocol_version_t, cardano_protocol_version_unref> owner(value);

  checkCardanoError(result, "ProtocolVersion copy");

  m_value = owner.release();
}

ProtocolVersion::ProtocolVersion(ProtocolVersion&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

ProtocolVersion::~ProtocolVersion()
{
  cardano_protocol_version_unref(&m_value);
}

ProtocolVersion
ProtocolVersion::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return ProtocolVersion(decodeCbor<cardano_protocol_version_t, cardano_protocol_version_unref>(cbor, cardano_protocol_version_from_cbor));
}

ProtocolVersion
ProtocolVersion::fromCbor(const std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::uint64_t
ProtocolVersion::major() const
{
  return cardano_protocol_version_get_major(checked());
}

void
ProtocolVersion::setMajor(std::uint64_t major)
{
  checkCardanoError(cardano_protocol_version_set_major(checked(), major), "ProtocolVersion setMajor");
}

std::uint64_t
ProtocolVersion::minor() const
{
  return cardano_protocol_version_get_minor(checked());
}

void
ProtocolVersion::setMinor(std::uint64_t minor)
{
  if (minor > std::numeric_limits<std::uint32_t>::max())
  {
    throw std::invalid_argument("Minor protocol version exceeds UINT32_MAX");
  }

  checkCardanoError(cardano_protocol_version_set_minor(checked(), minor), "ProtocolVersion setMinor");
}

std::vector<std::uint8_t>
ProtocolVersion::toCbor() const
{
  return encodeCbor(checked(), cardano_protocol_version_to_cbor);
}

std::string
ProtocolVersion::toJson() const
{
  return encodeJson(checked(), cardano_protocol_version_to_cip116_json);
}

void
ProtocolVersion::swap(ProtocolVersion& other) noexcept
{
  std::swap(m_value, other.m_value);
}

ProtocolVersion&
ProtocolVersion::operator=(const ProtocolVersion& other)
{
  if (this != &other)
  {
    ProtocolVersion copy(other);

    swap(copy);
  }

  return *this;
}

ProtocolVersion&
ProtocolVersion::operator=(ProtocolVersion&& other) noexcept
{
  if (this != &other)
  {
    cardano_protocol_version_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }

  return *this;
}

bool
ProtocolVersion::operator==(const ProtocolVersion& other) const
{
  return major() == other.major() && minor() == other.minor();
}

bool
ProtocolVersion::operator!=(const ProtocolVersion& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

ProtocolVersion::ProtocolVersion(cardano_protocol_version_t* value) noexcept
: m_value(value)
{
}

cardano_protocol_version_t*
ProtocolVersion::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from ProtocolVersion");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(ProtocolVersion& lhs, ProtocolVersion& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
