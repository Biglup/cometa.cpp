/**
 * @file ProviderFixtures.h
 *
 * @brief Shared response fixtures and request recorder for provider tests.
 *
 * @author angel.castillo
 * @date   Nov 03, 2026
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
#include <Cometa/Cometa.h>
#include <fstream>
#include <gtest/gtest.h>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace ProviderFixtures
{
/** @brief Injectable request recorder with deterministic responses. */
class Transport final : public Cometa::HttpTransport
{
  public:

    /** @brief Queued responses. */
    std::vector<Cometa::HttpResponse> responses;
    /** @brief Observed requests. */
    mutable std::vector<Cometa::HttpRequest> requests;

    /** @copydoc Cometa::HttpTransport::execute */
    Cometa::HttpResponse
    execute(const Cometa::HttpRequest& request) const override
    {
      if (requests.size() >= responses.size())
      {
        throw std::runtime_error("Unexpected fixture request");
      }
      const Cometa::HttpResponse response = responses.at(requests.size());
      requests.push_back(request);
      return response;
    }
};

/** @brief Reads a public provider fixture. @param name File basename. @return Raw JSON. */
inline std::string
fixture(const std::string& name)
{
  std::ifstream input(std::string(COMETA_PROVIDER_FIXTURE_DIR) + "/" + name + ".json", std::ios::binary);
  if (!input)
  {
    throw std::runtime_error("Missing provider fixture");
  }
  return std::string(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
}

/** @brief Creates a response. @param body Raw body. @param status HTTP status. @return Response. */
inline Cometa::HttpResponse
reply(const std::string& body, long status = 200)
{
  return { status, std::vector<std::uint8_t>(body.begin(), body.end()) };
}

/** @brief Encloses an RPC result. @param value Raw JSON. @return Envelope. */
inline std::string
rpc(const std::string& value)
{
  return "{\"jsonrpc\":\"2.0\",\"id\":\"cometa\",\"result\":" + value + "}";
}

/** @brief Returns a minimal valid transaction encoding. @return Transaction. */
inline Cometa::Transaction
transaction()
{
  return Cometa::Transaction::fromCbor("84a3008001800200a0f5f6");
}

/** @brief Reads a test reward account. @return Account. */
inline Cometa::RewardAddress
reward()
{
  return Cometa::RewardAddress::fromBech32("stake_test1uppy2gm2hqzkwc80em4mlat73j4jyqvzhclrvsu72g9xg4q2yweet");
}
} // namespace ProviderFixtures
