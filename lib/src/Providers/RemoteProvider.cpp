/**
 * @file RemoteProvider.cpp
 *
 * @brief Private remote provider implementation.
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

/* INCLUDES ******************************************************************/

#include "RemoteProvider.h"
#include "RemoteCodec.h"

#include <algorithm>
#include <limits>
#include <set>
#include <stdexcept>
#include <system_error>
#include <thread>

/* USINGS ********************************************************************/

using namespace Cometa;

/* INTERNAL IMPLEMENTATION ***************************************************/

namespace
{
/** @brief Reports invalid server data without echoing its body. */
[[noreturn]] void
invalid()
{
  throw std::runtime_error("Invalid provider response");
}

/** @brief Maps explicit chain identity to a hosted endpoint name. @param network Chain. @return Name. */
std::string
networkName(NetworkMagic network)
{
  switch (network)
  {
    case NetworkMagic::Mainnet:
      return "mainnet";
    case NetworkMagic::Preprod:
      return "preprod";
    case NetworkMagic::Preview:
      return "preview";
    default:
      throw std::invalid_argument("Unsupported provider network");
  }
}

/** @brief Checks printable URL characters. @param c Character. @return Whether it is accepted. */
bool
validEndpointCharacter(unsigned char c)
{
  return c > 32 && c < 127 && c != '\\';
}

/** @brief Checks printable credential characters. @param c Character. @return Whether it is accepted. */
bool
validTokenCharacter(unsigned char c)
{
  return c >= 33 && c <= 126;
}

/** @brief Validates an endpoint without logging it. @param endpoint Absolute HTTP URL. */
void
validateEndpoint(const std::string& endpoint)
{
  const std::size_t start = endpoint.find("://");
  if (start == std::string::npos || (endpoint.substr(0, start) != "http" && endpoint.substr(0, start) != "https"))
  {
    throw std::invalid_argument("Invalid provider endpoint");
  }
  const std::size_t end       = endpoint.find_first_of("/?#", start + 3);
  const std::string authority = endpoint.substr(start + 3, end == std::string::npos ? end : end - start - 3);
  if (authority.empty() || authority.find('@') != std::string::npos || endpoint.find('#') != std::string::npos)
  {
    throw std::invalid_argument("Invalid provider endpoint");
  }
  if (endpoint.size() > 8192)
  {
    throw std::invalid_argument("Invalid provider endpoint");
  }
  const std::size_t portStart = authority.front() == '[' ? authority.find(']', 1) : authority.find(':');
  std::string       port;
  if (authority.front() == '[')
  {
    if (portStart == std::string::npos || portStart == 1)
    {
      throw std::invalid_argument("Invalid provider endpoint");
    }
    if (portStart + 1 < authority.size())
    {
      if (authority[portStart + 1] != ':')
      {
        throw std::invalid_argument("Invalid provider endpoint");
      }
      port = authority.substr(portStart + 2);
      if (port.empty())
      {
        throw std::invalid_argument("Invalid provider endpoint");
      }
    }
  }
  else if (portStart != std::string::npos)
  {
    if (portStart == 0)
    {
      throw std::invalid_argument("Invalid provider endpoint");
    }
    port = authority.substr(portStart + 1);
    if (port.empty())
    {
      throw std::invalid_argument("Invalid provider endpoint");
    }
  }
  if (!port.empty())
  {
    if (port.size() > 5 || port.find_first_not_of("0123456789") != std::string::npos || std::stoul(port) > 65535 || std::stoul(port) == 0)
    {
      throw std::invalid_argument("Invalid provider endpoint");
    }
  }
  if (!std::all_of(endpoint.begin(), endpoint.end(), validEndpointCharacter))
  {
    throw std::invalid_argument("Invalid provider endpoint");
  }
}

/** @brief Validates a nonempty header token. @param token Credential. */
void
validateToken(const std::string& token)
{
  if (token.empty() || token.size() > 4096)
  {
    throw std::invalid_argument("Missing provider credential");
  }
  if (!std::all_of(token.begin(), token.end(), validTokenCharacter))
  {
    throw std::invalid_argument("Invalid provider credential");
  }
}

/** @brief Percent-encodes a URL component. @param text Component. @return Encoded text. */
std::string
escaped(const std::string& text)
{
  constexpr char HEX[] = "0123456789ABCDEF";
  std::string    result;
  for (const unsigned char c: text)
  {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~')
    {
      result += static_cast<char>(c);
    }
    else
    {
      result += '%';
      result += HEX[c >> 4];
      result += HEX[c & 15];
    }
  }
  return result;
}

/** @brief Decodes a query name for override comparison. @param text Encoded query name. @return Decoded name. */
std::string
queryName(const std::string& text)
{
  std::string result;
  for (std::size_t i = 0; i < text.size(); ++i)
  {
    if (text[i] == '%')
    {
      if (i + 2 >= text.size())
      {
        throw std::invalid_argument("Invalid endpoint query");
      }
      const std::vector<std::uint8_t> byte = remoteBytes(text.substr(i + 1, 2));
      result                               += static_cast<char>(byte[0]);
      i                                    += 2;
    }
    else
    {
      result += text[i] == '+' ? ' ' : text[i];
    }
  }
  return result;
}

/** @brief Splits encoded query fields without decoding credential values. @param query Query. @return Fields. */
std::vector<std::string>
queryFields(const std::string& query)
{
  std::vector<std::string> fields;
  std::size_t              start = 0;
  while (start < query.size())
  {
    const std::size_t end = query.find('&', start);
    fields.push_back(query.substr(start, end == std::string::npos ? end : end - start));
    if (end == std::string::npos)
    {
      break;
    }
    start = end + 1;
  }
  return fields;
}

/** @brief Joins paths; operation query keys override base parameters. @param base Base URL. @param path Relative path/query. @return URL. */
std::string
joined(const std::string& base, const std::string& path)
{
  if (path.empty())
  {
    return base;
  }
  const std::size_t query  = base.find('?');
  std::string       prefix = base.substr(0, query);
  while (!prefix.empty() && prefix.back() == '/')
  {
    prefix.pop_back();
  }
  std::string           result    = prefix + "/" + path;
  const std::size_t     operation = path.find('?');
  std::set<std::string> keys;
  if (operation != std::string::npos)
  {
    for (const std::string& field: queryFields(path.substr(operation + 1)))
    {
      keys.insert(queryName(field.substr(0, field.find('='))));
    }
  }
  bool hasQuery = operation != std::string::npos;
  if (query != std::string::npos)
  {
    for (const std::string& field: queryFields(base.substr(query + 1)))
    {
      if (keys.count(queryName(field.substr(0, field.find('=')))))
      {
        continue;
      }
      result   += (hasQuery ? "&" : "?") + field;
      hasQuery = true;
    }
  }
  return result;
}

/** @brief Converts request text to bytes. @param text Text. @return Bytes. */
std::vector<std::uint8_t>
bodyBytes(const std::string& text)
{
  return std::vector<std::uint8_t>(text.begin(), text.end());
}

/** @brief Encodes a list of strings. @param values Strings. @return JSON array. */
std::string
strings(const std::vector<std::string>& values)
{
  JsonWriter writer;
  writer.writeStartArray();
  for (const std::string& value: values)
  {
    writer.writeString(value);
  }
  writer.writeEndArray();
  return writer.toString();
}

/** @brief Checks a row's output reference. @param row Row. @param backend Dialect. @return Reference. */
TransactionInput
input(const JsonValue& row, RemoteBackend backend)
{
  const bool          kupo  = backend == RemoteBackend::Kupmios;
  const std::string   hash  = row.at(kupo ? "transaction_id" : "tx_hash").asString();
  const std::uint64_t index = row.at(kupo ? "output_index" : "tx_index")
                                .asUInt();
  if (hash.size() != 64 || index > 65535)
  {
    invalid();
  }
  return TransactionInput(Blake2bHash::fromHex(hash), index);
}

/** @brief Copies only JSON array rows. @param value Array. @return Rows. */
std::vector<JsonValue>
rows(const JsonValue& value)
{
  if (value.type() != JsonType::Array)
  {
    invalid();
  }
  std::vector<JsonValue> result;
  for (std::size_t i = 0; i < value.size(); ++i)
  {
    result.push_back(value.at(i));
  }
  return result;
}

/** @brief Reads native assets and exact lovelace from an extended output. @param row Row. @param kupo Dialect. @return Value. */
Value
amount(const JsonValue& row, bool kupo)
{
  const JsonValue       value = row.at("value");
  Value                 result(remoteQuantity(kupo ? value.at("coins") : value));
  std::set<std::string> seen;
  if (kupo)
  {
    const JsonValue assets = value.at("assets");
    for (std::size_t i = 0; i < assets.size(); ++i)
    {
      std::string       unit = assets.keyAt(i);
      const std::size_t dot  = unit.find('.');
      if (dot != std::string::npos)
      {
        if (dot != 56)
        {
          invalid();
        }
        unit.erase(dot, 1);
      }
      if (dot == std::string::npos && unit.size() != 56)
      {
        invalid();
      }
      const AssetId identifier = AssetId::fromHex(unit);
      if (!seen.insert(identifier.toHex()).second)
      {
        invalid();
      }
      const std::int64_t quantity = remoteQuantity(assets.valueAt(i));
      if (quantity <= 0)
      {
        invalid();
      }
      result.addAsset(identifier, quantity);
    }
  }
  else if (remotePresent(row, "asset_list"))
  {
    for (const JsonValue& asset: rows(row.at("asset_list")))
    {
      const std::string unit       = asset.at("policy_id").asString() + asset.at("asset_name").asString();
      const AssetId     identifier = AssetId::fromHex(unit);
      if (!seen.insert(identifier.toHex()).second)
      {
        invalid();
      }
      const std::int64_t quantity = remoteQuantity(asset.at("quantity"));
      if (quantity <= 0)
      {
        invalid();
      }
      result.addAsset(identifier, quantity);
    }
  }
  return result;
}

/** @brief Gets an asset quantity, treating absence as zero. @param value Output. @param asset Asset. @return Quantity. */
std::int64_t
assetQuantity(const Utxo& value, const AssetId& asset)
{
  return value.output().amount().toAssetMap().get(asset).value_or(0);
}
} // namespace

/* FUNCTIONS IMPLEMENTATION **************************************************/

namespace Cometa
{
RemoteProvider::RemoteProvider(RemoteBackend backend, NetworkMagic network, std::shared_ptr<HttpTransport> transport, std::string queryEndpoint, std::string rpcEndpoint, const std::string& queryToken, const std::string& rpcToken)
: m_backend(backend)
, m_network(network)
, m_transport(std::move(transport))
, m_queryEndpoint(std::move(queryEndpoint))
, m_rpcEndpoint(std::move(rpcEndpoint))
{
  const std::string chain = networkName(network);
  if (!m_transport)
  {
    throw std::invalid_argument("HTTP transport is required");
  }
  if (m_queryEndpoint.empty())
  {
    if (backend == RemoteBackend::Koios)
    {
      m_queryEndpoint = "https://" + (chain == "mainnet" ? std::string("api") : chain) + ".koios.rest/api/v1";
    }
  }
  validateEndpoint(m_queryEndpoint);
  if (!queryToken.empty())
  {
    validateToken(queryToken);
    m_queryHeaders.emplace_back("Authorization", "Bearer " + queryToken);
  }
  if (backend == RemoteBackend::Kupmios)
  {
    validateEndpoint(m_rpcEndpoint);
    if (!rpcToken.empty())
    {
      validateToken(rpcToken);
      m_rpcHeaders.emplace_back("Authorization", "Bearer " + rpcToken);
    }
  }
}

std::string
RemoteProvider::name() const
{
  return m_backend == RemoteBackend::Hydra ? "Hydra" : m_backend == RemoteBackend::Koios ? "Koios"
                                                                                         : "Kupmios";
}

NetworkMagic
RemoteProvider::networkMagic() const
{
  return m_network;
}

HttpResponse
RemoteProvider::request(const std::string& path, const std::optional<std::string>& body, const std::string& type, bool rpc, std::chrono::milliseconds timeout) const
{
  const bool  separate = rpc && m_backend == RemoteBackend::Kupmios;
  HttpRequest request;
  request.method  = body.has_value() ? "POST" : "GET";
  request.url     = joined(separate ? m_rpcEndpoint : m_queryEndpoint, path);
  request.headers = separate ? m_rpcHeaders : m_queryHeaders;
  request.headers.emplace_back("Accept", "application/json");
  request.timeout = std::max(std::chrono::milliseconds(1), std::min(timeout, std::chrono::milliseconds(30000)));
  if (body)
  {
    request.body = bodyBytes(*body);
    request.headers.emplace_back("Content-Type", type);
  }
  try
  {
    const HttpResponse response = m_transport->execute(request);
    if (response.body.size() > 8 * 1024 * 1024)
    {
      throw std::runtime_error("Response too large");
    }
    return response;
  }
  catch (const std::system_error& error)
  {
    if (error.code() == std::errc::timed_out)
    {
      throw;
    }
    throw std::runtime_error("Provider transport failed");
  }
  catch (const std::exception&)
  {
    throw std::runtime_error("Provider transport failed");
  }
}

JsonValue
RemoteProvider::rpc(const std::string& method, const std::string& params) const
{
  const std::string body     = remoteObject({ { "jsonrpc", remoteString("2.0") }, { "id", remoteString("cometa") }, { "method", remoteString(method) }, { "params", params } });
  const JsonValue   response = remoteJson(request(m_backend == RemoteBackend::Koios ? "ogmios" : "", body, "application/json", true));
  if (response.at("jsonrpc").asString() != "2.0" || response.at("id").asString() != "cometa")
  {
    invalid();
  }
  if (response.contains("method") && response.at("method").asString() != method)
  {
    invalid();
  }
  if (response.contains("error"))
  {
    throw std::runtime_error("Ogmios rejected the request");
  }
  return response.at("result");
}

ProtocolParameters
RemoteProvider::getParameters() const
{
  if (m_backend == RemoteBackend::Hydra)
  {
    return remoteHydraParameters(remoteJson(request("protocol-parameters")));
  }
  return remoteParameters(rpc("queryLedgerState/protocolParameters"));
}

std::vector<JsonValue>
RemoteProvider::pages(const std::string& path, const std::optional<std::string>& body) const
{
  if (m_backend == RemoteBackend::Kupmios)
  {
    return rows(remoteJson(request(path, body)));
  }
  std::vector<JsonValue> result;
  for (std::size_t page = 0; page < 10000; ++page)
  {
    const std::string            query   = path + (path.find('?') == std::string::npos ? "?" : "&") + "limit=100&offset=" + std::to_string(page * 100) + "&order=tx_hash.asc,tx_index.asc";
    const std::vector<JsonValue> current = rows(remoteJson(request(query, body)));
    if (current.size() > 100)
    {
      invalid();
    }
    result.insert(result.end(), current.begin(), current.end());
    if (current.size() < 100)
    {
      return result;
    }
  }
  throw std::runtime_error("Provider pagination limit exceeded");
}

Utxo
RemoteProvider::output(const JsonValue& row) const
{
  const TransactionInput reference = input(row, m_backend);
  const bool             kupo      = m_backend == RemoteBackend::Kupmios;
  TransactionOutput      output(Address::fromString(row.at("address").asString()), amount(row, kupo));
  if (remotePresent(row, "datum_hash"))
  {
    const Blake2bHash hash        = Blake2bHash::fromHex(row.at("datum_hash").asString());
    const bool        inlineDatum = kupo ? row.at("datum_type").asString() == "inline" : remotePresent(row, "inline_datum");
    if (inlineDatum)
    {
      output.setDatum(Datum(kupo ? resolveDatum(hash) : remoteDatum(row.at("inline_datum").at("bytes").asString(), hash)));
    }
    else
    {
      if (kupo && row.at("datum_type").asString() != "hash")
      {
        invalid();
      }
      output.setDatum(Datum(hash));
    }
  }
  if (kupo && remotePresent(row, "script_hash"))
  {
    const std::string hash   = row.at("script_hash").asString();
    const JsonValue   script = remoteJson(request("scripts/" + escaped(hash)));
    output.setScriptRef(remoteScript(script.at("language").asString(), script.at("script").asString(), hash));
  }
  else if (!kupo && remotePresent(row, "reference_script"))
  {
    const JsonValue   script = row.at("reference_script");
    const std::string hash   = script.at("hash").asString();
    if (script.at("type").asString() == "timelock" && !remotePresent(script, "bytes"))
    {
      const Script native(NativeScript::fromJson(script.at("value").toJson()));
      if (native.hash().toHex() != hash)
      {
        invalid();
      }
      output.setScriptRef(native);
    }
    else
    {
      output.setScriptRef(remoteScript(script.at("type").asString(), script.at("bytes").asString(), hash));
    }
  }
  return Utxo(reference, output);
}

UtxoList
RemoteProvider::outputs(const std::string& path, const std::optional<std::string>& body) const
{
  if (m_backend == RemoteBackend::Hydra)
  {
    return remoteHydraOutputs(remoteJson(request("snapshot/utxo")));
  }
  UtxoList              result;
  std::set<std::string> seen;
  for (const JsonValue& row: pages(path, body))
  {
    const Utxo value = output(row);
    if (!seen.insert(remoteInputKey(value.input())).second)
    {
      invalid();
    }
    if (m_backend == RemoteBackend::Koios && row.at("is_spent").asBool())
    {
      continue;
    }
    if (m_backend == RemoteBackend::Kupmios && row.at("spent_at").type() != JsonType::Null)
    {
      invalid();
    }
    result.push_back(value);
  }
  return result;
}

UtxoList
RemoteProvider::getUnspentOutputs(const Address& address) const
{
  const std::string text = address.toString();
  UtxoList          result;
  if (m_backend == RemoteBackend::Hydra)
  {
    for (const Utxo& value: outputs("snapshot/utxo"))
    {
      if (value.output().address() != address)
      {
        continue;
      }
      result.push_back(value);
    }
    return result;
  }
  if (m_backend == RemoteBackend::Koios)
  {
    result = outputs("address_utxos", remoteObject({ { "_addresses", strings({ text }) }, { "_extended", "true" } }));
  }
  else
  {
    result = outputs("matches/" + escaped(text) + "?unspent");
  }
  for (const Utxo& value: result)
  {
    if (value.output().address() != address)
    {
      invalid();
    }
  }
  return result;
}

UtxoList
RemoteProvider::getUnspentOutputsWithAsset(const Address& address, const AssetId& asset) const
{
  UtxoList result;
  for (const Utxo& value: getUnspentOutputs(address))
  {
    if (assetQuantity(value, asset) <= 0)
    {
      continue;
    }
    result.push_back(value);
  }
  return result;
}

Utxo
RemoteProvider::getUnspentOutputByNft(const AssetId& asset) const
{
  if (asset.isLovelace())
  {
    throw std::invalid_argument("NFT lookup requires a native asset");
  }
  const std::string unit = asset.toHex();
  UtxoList          result;
  if (m_backend == RemoteBackend::Hydra)
  {
    for (const Utxo& value: outputs("snapshot/utxo"))
    {
      if (assetQuantity(value, asset) <= 0)
      {
        continue;
      }
      result.push_back(value);
    }
  }
  else if (m_backend == RemoteBackend::Koios)
  {
    result = outputs("asset_utxos", remoteObject({ { "_asset_list", "[" + strings({ unit.substr(0, 56), unit.substr(56) }) + "]" }, { "_extended", "true" } }));
  }
  else
  {
    result = outputs("matches/*?unspent&policy_id=" + unit.substr(0, 56) + "&asset_name=" + unit.substr(56));
  }
  if (result.size() != 1 || assetQuantity(result.at(0), asset) != 1)
  {
    throw std::runtime_error("Expected exactly one NFT output");
  }
  return result.at(0);
}

UtxoList
RemoteProvider::resolveUnspentOutputs(const TransactionInputSet& inputs) const
{
  std::vector<std::string> requested;
  std::set<std::string>    identities;
  for (const TransactionInput& reference: inputs)
  {
    const std::string key = remoteInputKey(reference);
    if (identities.insert(key).second)
    {
      requested.push_back(key);
    }
  }
  std::map<std::string, Utxo> found;
  if (m_backend == RemoteBackend::Hydra)
  {
    for (const Utxo& value: outputs("snapshot/utxo"))
    {
      if (identities.count(remoteInputKey(value.input())))
      {
        found.emplace(remoteInputKey(value.input()), value);
      }
    }
  }
  else if (m_backend == RemoteBackend::Kupmios)
  {
    for (const TransactionInput& reference: inputs)
    {
      for (const Utxo& value: outputs("matches/" + std::to_string(reference.index()) + "@" + reference.id().toHex() + "?unspent"))
      {
        if (!found.emplace(remoteInputKey(value.input()), value).second)
        {
          invalid();
        }
      }
    }
  }
  else
  {
    for (std::size_t offset = 0; offset < requested.size(); offset += 100)
    {
      const std::vector<std::string> batch(requested.begin() + offset, requested.begin() + std::min(offset + 100, requested.size()));
      const std::string              body = remoteObject({ { "_utxo_refs", strings(batch) }, { "_extended", "true" } });
      for (const Utxo& value: outputs("utxo_info", body))
      {
        if (!found.emplace(remoteInputKey(value.input()), value).second)
        {
          invalid();
        }
      }
    }
  }
  for (const std::pair<const std::string, Utxo>& entry: found)
  {
    if (identities.count(entry.first) == 0)
    {
      invalid();
    }
  }
  UtxoList result;
  for (const std::string& key: requested)
  {
    const std::map<std::string, Utxo>::const_iterator value = found.find(key);
    if (value != found.end())
    {
      result.push_back(value->second);
    }
  }
  return result;
}

std::uint64_t
RemoteProvider::getRewardsAvailable(const RewardAddress& address) const
{
  const std::string text = address.toString();
  if (m_backend == RemoteBackend::Hydra)
  {
    throw std::logic_error("Hydra Heads do not support staking rewards");
  }
  if (m_backend == RemoteBackend::Koios)
  {
    const std::vector<JsonValue> found = rows(remoteJson(request("account_info", remoteObject({ { "_stake_addresses", strings({ text }) } }))));
    if (found.empty())
    {
      return 0;
    }
    if (found.size() != 1 || found[0].at("stake_address").asString() != text)
    {
      invalid();
    }
    return found[0].at("rewards_available").asUInt();
  }
  const Credential             owner  = address.credential();
  const bool                   script = owner.type() == CredentialType::ScriptHash;
  const std::vector<JsonValue> found  = rows(rpc("queryLedgerState/rewardAccountSummaries", remoteObject({ { script ? "scripts" : "keys", strings({ owner.hash().toHex() }) } })));
  if (found.empty())
  {
    return 0;
  }
  if (found.size() != 1 || found[0].at("credential").asString() != owner.hash().toHex() || found[0].at("from").asString() != (script ? "script" : "verificationKey"))
  {
    invalid();
  }
  return found[0].at("rewards").at("ada").at("lovelace").asUInt();
}

PlutusData
RemoteProvider::resolveDatum(const Blake2bHash& hash) const
{
  if (hash.toBytes().size() != 32)
  {
    throw std::invalid_argument("Datum hash must contain 32 bytes");
  }
  if (m_backend == RemoteBackend::Hydra)
  {
    for (const Utxo& value: outputs("snapshot/utxo"))
    {
      const std::optional<Datum> datum = value.output().datum();
      if (datum && datum->inlineData() && Blake2bHash::compute(datum->inlineData()->toCbor()) == hash)
      {
        return *datum->inlineData();
      }
    }
    throw std::runtime_error("Datum is not available in the current Hydra snapshot");
  }
  if (m_backend == RemoteBackend::Kupmios)
  {
    return remoteDatum(remoteJson(request("datums/" + hash.toHex())).at("datum").asString(), hash);
  }
  const std::vector<JsonValue> found = rows(remoteJson(request("datum_info", remoteObject({ { "_datum_hashes", strings({ hash.toHex() }) } }))));
  if (found.size() != 1 || found[0].at("datum_hash").asString() != hash.toHex())
  {
    invalid();
  }
  return remoteDatum(found[0].at("bytes").asString(), hash);
}

bool
RemoteProvider::confirmed(const Blake2bHash& id, std::chrono::milliseconds timeout) const
{
  if (m_backend == RemoteBackend::Hydra)
  {
    const HttpResponse response = request("snapshot", std::nullopt, "application/json", false, timeout);
    if (response.status == 404)
    {
      return false;
    }
    const JsonValue data = remoteJson(response);
    if (data.at("tag").asString() == "InitialSnapshot")
    {
      return false;
    }
    if (data.at("tag").asString() != "ConfirmedSnapshot")
    {
      invalid();
    }
    const JsonValue snapshot = data.at("snapshot");
    snapshot.at("number").asUInt();
    bool found = false;
    for (const JsonValue& value: rows(snapshot.at("confirmed")))
    {
      const Blake2bHash actual = Transaction::fromCbor(value.at("cborHex").asString()).id();
      if (value.contains("txId") && value.at("txId").asString() != actual.toHex())
      {
        invalid();
      }
      if (actual == id)
      {
        found = true;
      }
    }
    return found;
  }
  if (m_backend == RemoteBackend::Koios)
  {
    const std::vector<JsonValue> found = rows(remoteJson(request("tx_status", remoteObject({ { "_tx_hashes", strings({ id.toHex() }) } }), "application/json", false, timeout)));
    if (found.empty())
    {
      return false;
    }
    if (found.size() != 1 || found[0].at("tx_hash").asString() != id.toHex())
    {
      invalid();
    }
    return remotePresent(found[0], "num_confirmations") && found[0].at("num_confirmations").asUInt() > 0;
  }
  const std::vector<JsonValue> found = rows(remoteJson(request("matches/*@" + id.toHex(), std::nullopt, "application/json", false, timeout)));
  for (const JsonValue& row: found)
  {
    if (row.at("transaction_id").asString() != id.toHex() || remoteBytes(row.at("created_at").at("header_hash").asString()).size() != 32)
    {
      invalid();
    }
    row.at("created_at").at("slot_no").asUInt();
  }
  return !found.empty();
}

bool
RemoteProvider::confirmTransaction(const Blake2bHash& id, std::chrono::milliseconds timeout) const
{
  if (timeout.count() < 0 || id.toBytes().size() != 32)
  {
    throw std::invalid_argument("Invalid confirmation request");
  }
  const std::chrono::steady_clock::time_point start    = std::chrono::steady_clock::now();
  const std::chrono::milliseconds             maximum  = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::time_point::max() - start);
  const std::chrono::steady_clock::time_point deadline = timeout >= maximum ? std::chrono::steady_clock::time_point::max() : start + timeout;
  while (std::chrono::steady_clock::now() < deadline)
  {
    try
    {
      const std::chrono::milliseconds remaining = std::chrono::duration_cast<std::chrono::milliseconds>(deadline - std::chrono::steady_clock::now());
      if (confirmed(id, std::max(std::chrono::milliseconds(1), remaining)))
      {
        return std::chrono::steady_clock::now() < deadline;
      }
    }
    catch (const std::system_error& error)
    {
      if (error.code() == std::errc::timed_out && std::chrono::steady_clock::now() >= deadline)
      {
        return false;
      }
      throw;
    }
    std::this_thread::sleep_until(std::min(deadline, std::chrono::steady_clock::now() + std::chrono::milliseconds(1000)));
  }
  return false;
}

Blake2bHash
RemoteProvider::submitTransaction(const Transaction& transaction) const
{
  const Blake2bHash expected = transaction.id();
  std::string       result;
  if (m_backend == RemoteBackend::Hydra)
  {
    const HttpResponse response = request("transaction", remoteObject({ { "type", remoteString("Tx ConwayEra") }, { "description", remoteString("") }, { "cborHex", remoteString(remoteHex(transaction.toCbor())) } }));
    const JsonValue    value    = remoteJson(response);
    const std::string  tag      = value.at("tag").asString();
    if (response.status == 200 && tag == "SubmitTxConfirmed")
    {
      value.at("snapshotNumber").asUInt();
    }
    else if (response.status != 202 || tag != "SubmitTxSubmitted")
    {
      invalid();
    }
    return expected;
  }
  else
  {
    result = rpc("submitTransaction", remoteObject({ { "transaction", remoteObject({ { "cbor", remoteString(remoteHex(transaction.toCbor())) } }) } })).at("transaction").at("id").asString();
  }
  if (result != expected.toHex())
  {
    invalid();
  }
  return expected;
}

RedeemerList
RemoteProvider::evaluateTransaction(const Transaction& transaction, const UtxoList& additionalUtxos) const
{
  if (m_backend == RemoteBackend::Hydra)
  {
    throw std::logic_error("Hydra has no remote evaluation endpoint; configure a local evaluator");
  }
  JsonWriter writer;
  writer.writeStartArray();
  std::set<std::string> seen;
  for (const Utxo& value: additionalUtxos)
  {
    if (!seen.insert(remoteInputKey(value.input())).second)
    {
      throw std::invalid_argument("Duplicate additional input");
    }
    writer.writeRaw(remoteOgmiosOutput(value));
  }
  writer.writeEndArray();
  const std::string cbor = remoteString(remoteHex(transaction.toCbor()));
  return remoteBudgets(transaction, rpc("evaluateTransaction", remoteObject({ { "transaction", remoteObject({ { "cbor", cbor } }) }, { "additionalUtxo", writer.toString() } })));
}
} // namespace Cometa
