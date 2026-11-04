/**
 * @file CostModel.cpp
 *
 * @brief Plutus operation costs.
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

#include <Cometa/ProtocolParams/CostModel.h>

#include "../CardanoInterop.h"

#include <cardano/protocol_params/cost_model.h>

#include <utility>

/* CLASS IMPLEMENTATION ******************************************************/

namespace Cometa
{
CostModel::CostModel(PlutusLanguageVersion language, const std::vector<std::int64_t>& costs)
: m_value(nullptr)
{
  cardano_cost_model_t* value  = nullptr;
  const cardano_error_t status = cardano_cost_model_new(
    static_cast<cardano_plutus_language_version_t>(language),
    costs.data(),
    costs.size(),
    &value);
  CardanoObject<cardano_cost_model_t, cardano_cost_model_unref> owner(value);

  checkCardanoError(status, "CostModel");
  m_value = owner.release();
}

CostModel::CostModel(const CostModel& other)
: m_value(nullptr)
{
  cardano_cost_model_t*                                         value  = nullptr;
  const cardano_error_t                                         status = cardano_cost_model_deep_clone(other.checked(), &value);
  CardanoObject<cardano_cost_model_t, cardano_cost_model_unref> owner(value);

  checkCardanoError(status, "CostModel copy");
  m_value = owner.release();
}

CostModel::CostModel(CostModel&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
{
}

CostModel::~CostModel()
{
  cardano_cost_model_unref(&m_value);
}

CostModel
CostModel::fromCbor(const std::vector<std::uint8_t>& cbor)
{
  return CostModel(decodeCbor<cardano_cost_model_t, cardano_cost_model_unref>(cbor, cardano_cost_model_from_cbor));
}

CostModel
CostModel::fromCbor(std::string_view cborHex)
{
  return fromCbor(decodeHex(cborHex));
}

std::uint8_t
CostModel::languageId() const
{
  std::uint8_t id = 0;
  checkCardanoError(cardano_cost_model_get_language_id(checked(), &id), "CostModel languageId");
  return id;
}

std::optional<PlutusLanguageVersion>
CostModel::language() const
{
  const std::uint8_t id = languageId();

  if (id > static_cast<std::uint8_t>(PlutusLanguageVersion::V4))
  {
    return std::nullopt;
  }

  cardano_plutus_language_version_t language = CARDANO_PLUTUS_LANGUAGE_VERSION_V1;
  checkCardanoError(cardano_cost_model_get_language(checked(), &language), "CostModel language");
  return static_cast<PlutusLanguageVersion>(language);
}

std::size_t
CostModel::size() const
{
  return cardano_cost_model_get_costs_size(checked());
}

std::int64_t
CostModel::cost(std::size_t operation) const
{
  if (operation >= size())
  {
    throw std::out_of_range("CostModel operation is out of range");
  }

  std::int64_t value = 0;
  checkCardanoError(cardano_cost_model_get_cost(checked(), operation, &value), "CostModel cost");
  return value;
}

std::vector<std::int64_t>
CostModel::costs() const
{
  const std::size_t   count = size();
  const std::int64_t* data  = cardano_cost_model_get_costs(checked());

  if (count == 0)
  {
    return {};
  }

  if (!data)
  {
    throw std::logic_error("CostModel has no cost data");
  }

  return std::vector<std::int64_t>(data, data + count);
}

void
CostModel::setCost(std::size_t operation, std::int64_t value)
{
  if (operation >= size())
  {
    throw std::out_of_range("CostModel operation is out of range");
  }

  checkCardanoError(cardano_cost_model_set_cost(checked(), operation, value), "CostModel setCost");
}

std::vector<std::uint8_t>
CostModel::toCbor() const
{
  return encodeCbor(checked(), cardano_cost_model_to_cbor);
}

std::string
CostModel::toJson() const
{
  return encodeJson(checked(), cardano_cost_model_to_cip116_json);
}

void
CostModel::swap(CostModel& other) noexcept
{
  std::swap(m_value, other.m_value);
}

CostModel&
CostModel::operator=(const CostModel& other)
{
  if (this != &other)
  {
    CostModel copy(other);
    swap(copy);
  }
  return *this;
}

CostModel&
CostModel::operator=(CostModel&& other) noexcept
{
  if (this != &other)
  {
    cardano_cost_model_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
  }
  return *this;
}

bool
CostModel::operator==(const CostModel& other) const
{
  return toCbor() == other.toCbor();
}

bool
CostModel::operator!=(const CostModel& other) const
{
  return !(*this == other);
}

CostModel
CostModel::adopt(cardano_cost_model_t* value)
{
  if (!value)
  {
    throw std::invalid_argument("Cannot adopt a null CostModel");
  }

  return CostModel(value);
}

cardano_cost_model_t*
CostModel::nativeHandle() noexcept
{
  return m_value;
}

const cardano_cost_model_t*
CostModel::nativeHandle() const noexcept
{
  return m_value;
}

cardano_cost_model_t*
CostModel::release() noexcept
{
  return std::exchange(m_value, nullptr);
}

/* PRIVATE FUNCTIONS *********************************************************/

CostModel::CostModel(cardano_cost_model_t* value) noexcept
: m_value(value)
{
}

cardano_cost_model_t*
CostModel::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from CostModel");
  }
  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(CostModel& lhs, CostModel& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
