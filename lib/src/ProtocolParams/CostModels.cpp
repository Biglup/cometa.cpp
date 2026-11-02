/**
 * @file CostModels.cpp
 *
 * @brief Plutus cost models indexed by language.
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

#include <Cometa/ProtocolParams/CostModels.h>

#include "../CardanoInterop.h"

#include <cardano/buffer.h>
#include <cardano/protocol_params/costmdls.h>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CostModels::CostModels()
: m_value(nullptr)
{
  cardano_costmdls_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_costmdls_new(&value);
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> owner(value);

  checkCardanoError(status, "CostModels");
  m_value = owner.release();
}

CostModels::CostModels(const CostModels& other)
: m_value(nullptr)
{
  cardano_costmdls_t*                                       value  = nullptr;
  const cardano_error_t                                     status = cardano_costmdls_deep_clone(other.checked(), &value);
  CardanoObject<cardano_costmdls_t, cardano_costmdls_unref> owner(value);

  checkCardanoError(status, "CostModels copy");
  m_value = owner.release();
}

CostModels::CostModels(CostModels&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

CostModels::~CostModels()
{
  cardano_costmdls_unref(&m_value);
}

CostModels
CostModels::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return CostModels(decodeCbor<cardano_costmdls_t, cardano_costmdls_unref>(cbor, cardano_costmdls_from_cbor));
}

CostModels
CostModels::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

void
CostModels::insert(const CostModel& model)
{
  const CostModel copy(model);
  checkCardanoError(cardano_costmdls_insert(checked(), copy.checked()), "CostModels insert");
}

bool
CostModels::contains(PlutusLanguageVersion language) const
{
  return cardano_costmdls_has(checked(), static_cast<cardano_plutus_language_version_t>(language));
}

std::optional<CostModel>
CostModels::get(std::uint8_t languageId) const
{
  cardano_cost_model_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_costmdls_get_by_language_id(checked(), languageId, &value);
  CardanoObject<cardano_cost_model_t, cardano_cost_model_unref> owner(value);

  checkCardanoError(status, "CostModels get");

  if (!value)
  {
    return std::nullopt;
  }

  const CostModel borrowed(owner.release());
  return CostModel(borrowed);
}

std::optional<CostModel>
CostModels::get(PlutusLanguageVersion language) const
{
  return get(static_cast<std::uint8_t>(language));
}

std::vector<std::uint8_t>
CostModels::languageViews() const
{
  cardano_buffer_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_costmdls_get_language_views_encoding(checked(), &value);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);

  checkCardanoError(status, "CostModels languageViews");

  const std::size_t length = cardano_buffer_get_size(owner.get());

  if (length == 0)
  {
    return {};
  }

  const std::uint8_t* bytes = cardano_buffer_get_data(owner.get());
  return std::vector<std::uint8_t>(bytes, bytes + length);
}

std::vector<std::uint8_t>
CostModels::toCbor() const
{
  return encodeCbor(checked(), cardano_costmdls_to_cbor);
}

std::string
CostModels::toJson() const
{
  return encodeJson(checked(), cardano_costmdls_to_cip116_json);
}

void
CostModels::swap(CostModels& other) noexcept
{
  std::swap(m_value, other.m_value);
}

CostModels&
CostModels::operator=(const CostModels& other)
{
  if (this != &other)
  {
    CostModels copy(other);
    swap(copy);
  }
  return *this;
}

CostModels&
CostModels::operator=(CostModels&& other) noexcept
{
  if (this != &other)
  {
    cardano_costmdls_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
CostModels::operator==(const CostModels& other) const
{
  return toCbor() == other.toCbor();
}

bool
CostModels::operator!=(const CostModels& other) const
{
  return !(*this == other);
}

/* PRIVATE FUNCTIONS *********************************************************/

CostModels::CostModels(cardano_costmdls_t* value) noexcept
: m_value(value)
{
}

cardano_costmdls_t*
CostModels::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CostModels");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CostModels& lhs, CostModels& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
