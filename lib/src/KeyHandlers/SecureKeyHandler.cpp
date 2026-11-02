/**
 * @file SecureKeyHandler.cpp
 *
 * @brief C++ signing callbacks and secure handler operations.
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

#include <Cometa/KeyHandlers/SecureKeyHandler.h>

#include "../CardanoInterop.h"

#include <cardano/buffer.h>
#include <cardano/crypto/bip32_public_key.h>
#include <cardano/crypto/ed25519_public_key.h>
#include <cardano/key_handlers/account_derivation_path.h>
#include <cardano/key_handlers/derivation_path.h>
#include <cardano/key_handlers/secure_key_handler.h>
#include <cardano/key_handlers/secure_key_handler_impl.h>
#include <cardano/object.h>
#include <cardano/transaction/sub_transaction.h>
#include <cardano/transaction/transaction.h>
#include <cardano/witness_set/vkey_witness_set.h>

#include <cstdio>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <utility>
#include <variant>

/* STATIC FUNCTIONS **********************************************************/

namespace Cometa
{
namespace
{
/** @brief Keeps C++ signing operations alive while the native handler exists. */
struct CallbackContext : cardano_object_t
{
    /** @brief The operations for the selected key scheme. */
    std::variant<Bip32KeyCallbacks, Ed25519KeyCallbacks> callbacks;

    /** @brief Stores BIP32 signing operations. @param value The operations. */
    explicit CallbackContext(Bip32KeyCallbacks value)
    : cardano_object_t { 1, &CallbackContext::deallocate, {} }
    , callbacks(std::move(value))
    {
    }

    /** @brief Stores Ed25519 signing operations. @param value The operations. */
    explicit CallbackContext(Ed25519KeyCallbacks value)
    : cardano_object_t { 1, &CallbackContext::deallocate, {} }
    , callbacks(std::move(value))
    {
    }

    /** @brief Releases the callback state. @param value The state. */
    static void
    deallocate(void* value)
    {
      delete static_cast<CallbackContext*>(static_cast<cardano_object_t*>(value));
    }
};

/** @brief Retrieves the operations for a native callback.
 * @param implementation The active handler.
 * @return The callback state.
 */
CallbackContext&
context(cardano_secure_key_handler_impl_t* implementation)
{
  return *static_cast<CallbackContext*>(implementation->context);
}

/** @brief Records an exception message for a native callback.
 * @param implementation The active handler.
 * @param message The error message.
 * @return A native error code.
 */
cardano_error_t
callbackError(cardano_secure_key_handler_impl_t* implementation, const char* message)
{
  std::snprintf(implementation->error_message, sizeof(implementation->error_message), "%s", message);
  return CARDANO_ERROR_GENERIC;
}

/** @brief Copies a native transaction into a C++ value.
 * @param value The transaction.
 * @return The copied value.
 */
Transaction
copyTransaction(const cardano_transaction_t* value)
{
  return Transaction::fromCbor(encodeCbor(value, cardano_transaction_to_cbor));
}

/** @brief Copies a native sub-transaction into a C++ value.
 * @param value The sub-transaction.
 * @return The copied value.
 */
SubTransaction
copySubTransaction(const cardano_sub_transaction_t* value)
{
  return SubTransaction::fromCbor(encodeCbor(value, cardano_sub_transaction_to_cbor));
}

/** @brief Converts native derivation paths to C++ values.
 * @param paths The native paths.
 * @param count The number of paths.
 * @return The converted paths.
 */
std::vector<DerivationPath>
copyPaths(const cardano_derivation_path_t* paths, std::size_t count)
{
  std::vector<DerivationPath> copied;
  copied.reserve(count);

  for (std::size_t index = 0; index < count; ++index)
  {
    if (paths[index].purpose > std::numeric_limits<std::uint32_t>::max() || paths[index].coin_type > std::numeric_limits<std::uint32_t>::max() || paths[index].account > std::numeric_limits<std::uint32_t>::max() || paths[index].role > std::numeric_limits<std::uint32_t>::max() || paths[index].index > std::numeric_limits<std::uint32_t>::max())
    {
      throw std::out_of_range("Derivation path segment exceeds 32 bits");
    }

    copied.push_back({ static_cast<std::uint32_t>(paths[index].purpose),
                       static_cast<std::uint32_t>(paths[index].coin_type),
                       static_cast<std::uint32_t>(paths[index].account),
                       static_cast<DerivationRole>(paths[index].role),
                       static_cast<std::uint32_t>(paths[index].index) });
  }

  return copied;
}

/** @brief Copies C++ witnesses into a native result.
 * @param witnesses The witnesses to copy.
 * @param output Receives the native witnesses.
 * @return The native status.
 */
cardano_error_t
copyWitnesses(const VkeyWitnessSet& witnesses, cardano_vkey_witness_set_t** output)
{
  const std::vector<std::uint8_t> cbor = witnesses.toCbor();
  *output                              = decodeCbor<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref>(cbor, cardano_vkey_witness_set_from_cbor);
  return CARDANO_SUCCESS;
}

/** @brief Invokes the BIP32 transaction signing operation.
 * @param implementation The active handler.
 * @param transaction The transaction to sign.
 * @param paths The derivation paths.
 * @param count The number of paths.
 * @param output Receives the witnesses.
 * @return The native status.
 */
cardano_error_t
bip32SignTransaction(
  cardano_secure_key_handler_impl_t* implementation,
  cardano_transaction_t*             transaction,
  const cardano_derivation_path_t*   paths,
  std::size_t                        count,
  cardano_vkey_witness_set_t**       output)
{
  try
  {
    const Bip32KeyCallbacks& callbacks = std::get<Bip32KeyCallbacks>(context(implementation).callbacks);
    return copyWitnesses(callbacks.signTransaction(copyTransaction(transaction), copyPaths(paths, count)), output);
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "BIP32 transaction signer failed");
  }
}

/** @brief Invokes the BIP32 sub-transaction signing operation.
 * @param implementation The active handler.
 * @param transaction The sub-transaction to sign.
 * @param paths The derivation paths.
 * @param count The number of paths.
 * @param output Receives the witnesses.
 * @return The native status.
 */
cardano_error_t
bip32SignSubTransaction(
  cardano_secure_key_handler_impl_t* implementation,
  cardano_sub_transaction_t*         transaction,
  const cardano_derivation_path_t*   paths,
  std::size_t                        count,
  cardano_vkey_witness_set_t**       output)
{
  try
  {
    const Bip32KeyCallbacks& callbacks = std::get<Bip32KeyCallbacks>(context(implementation).callbacks);
    return copyWitnesses(callbacks.signSubTransaction(copySubTransaction(transaction), copyPaths(paths, count)), output);
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "BIP32 sub-transaction signer failed");
  }
}

/** @brief Invokes the BIP32 account public-key operation.
 * @param implementation The active handler.
 * @param path The account derivation path.
 * @param output Receives the public key.
 * @return The native status.
 */
cardano_error_t
bip32AccountPublicKey(
  cardano_secure_key_handler_impl_t* implementation,
  cardano_account_derivation_path_t  path,
  cardano_bip32_public_key_t**       output)
{
  try
  {
    if (path.purpose > std::numeric_limits<std::uint32_t>::max() || path.coin_type > std::numeric_limits<std::uint32_t>::max() || path.account > std::numeric_limits<std::uint32_t>::max())
    {
      throw std::out_of_range("Account path segment exceeds 32 bits");
    }

    const Bip32KeyCallbacks&        callbacks = std::get<Bip32KeyCallbacks>(context(implementation).callbacks);
    const Bip32PublicKey            key       = callbacks.accountPublicKey({ static_cast<std::uint32_t>(path.purpose),
                                                                             static_cast<std::uint32_t>(path.coin_type),
                                                                             static_cast<std::uint32_t>(path.account) });
    const std::vector<std::uint8_t> bytes     = key.toBytes();
    return cardano_bip32_public_key_from_bytes(bytes.data(), bytes.size(), output);
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "BIP32 public key lookup failed");
  }
}

/** @brief Invokes the Ed25519 transaction signing operation.
 * @param implementation The active handler.
 * @param transaction The transaction to sign.
 * @param output Receives the witnesses.
 * @return The native status.
 */
cardano_error_t
ed25519SignTransaction(
  cardano_secure_key_handler_impl_t* implementation,
  cardano_transaction_t*             transaction,
  cardano_vkey_witness_set_t**       output)
{
  try
  {
    const Ed25519KeyCallbacks& callbacks = std::get<Ed25519KeyCallbacks>(context(implementation).callbacks);
    return copyWitnesses(callbacks.signTransaction(copyTransaction(transaction)), output);
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "Ed25519 transaction signer failed");
  }
}

/** @brief Invokes the Ed25519 sub-transaction signing operation.
 * @param implementation The active handler.
 * @param transaction The sub-transaction to sign.
 * @param output Receives the witnesses.
 * @return The native status.
 */
cardano_error_t
ed25519SignSubTransaction(
  cardano_secure_key_handler_impl_t* implementation,
  cardano_sub_transaction_t*         transaction,
  cardano_vkey_witness_set_t**       output)
{
  try
  {
    const Ed25519KeyCallbacks& callbacks = std::get<Ed25519KeyCallbacks>(context(implementation).callbacks);
    return copyWitnesses(callbacks.signSubTransaction(copySubTransaction(transaction)), output);
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "Ed25519 sub-transaction signer failed");
  }
}

/** @brief Invokes the Ed25519 public-key operation.
 * @param implementation The active handler.
 * @param output Receives the public key.
 * @return The native status.
 */
cardano_error_t
ed25519PublicKey(cardano_secure_key_handler_impl_t* implementation, cardano_ed25519_public_key_t** output)
{
  try
  {
    const Ed25519KeyCallbacks&      callbacks = std::get<Ed25519KeyCallbacks>(context(implementation).callbacks);
    const Ed25519PublicKey          key       = callbacks.publicKey();
    const std::vector<std::uint8_t> bytes     = key.toBytes();
    return cardano_ed25519_public_key_from_bytes(bytes.data(), bytes.size(), output);
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "Ed25519 public key lookup failed");
  }
}

/** @brief Invokes optional handler serialization.
 * @param implementation The active handler.
 * @param output Receives the serialized bytes.
 * @return The native status.
 */
cardano_error_t
serializeHandler(cardano_secure_key_handler_impl_t* implementation, cardano_buffer_t** output)
{
  try
  {
    const CallbackContext&          state = context(implementation);
    const std::vector<std::uint8_t> bytes = std::holds_alternative<Bip32KeyCallbacks>(state.callbacks)
      ? std::get<Bip32KeyCallbacks>(state.callbacks).serialize()
      : std::get<Ed25519KeyCallbacks>(state.callbacks).serialize();
    *output                               = cardano_buffer_new_from(data(bytes), bytes.size());
    return *output ? CARDANO_SUCCESS : CARDANO_ERROR_MEMORY_ALLOCATION_FAILED;
  }
  catch (const std::exception& error)
  {
    return callbackError(implementation, error.what());
  }
  catch (...)
  {
    return callbackError(implementation, "Secure key handler serialization failed");
  }
}

/** @brief Creates a native handler with C++ callback state.
 * @param name The handler name.
 * @param state The callback state.
 * @param type The key scheme.
 * @return The new handler.
 */
cardano_secure_key_handler_t*
createHandler(std::string_view name, CallbackContext* state, SecureKeyHandlerType type)
{
  if (name.empty() || name.size() >= 256 || name.find('\0') != std::string_view::npos)
  {
    delete state;
    throw std::invalid_argument("Secure key handler name must contain 1 to 255 characters without NUL");
  }

  cardano_secure_key_handler_impl_t implementation {};
  std::memcpy(implementation.name, name.data(), name.size());
  implementation.context = state;
  implementation.type    = static_cast<cardano_secure_key_handler_type_t>(type);

  if (type == SecureKeyHandlerType::Bip32)
  {
    implementation.bip32_sign_transaction                = &bip32SignTransaction;
    implementation.bip32_sign_sub_transaction            = &bip32SignSubTransaction;
    implementation.bip32_get_extended_account_public_key = &bip32AccountPublicKey;
    implementation.serialize                             = std::get<Bip32KeyCallbacks>(state->callbacks).serialize ? &serializeHandler : nullptr;
  }
  else
  {
    implementation.ed25519_sign_transaction     = &ed25519SignTransaction;
    implementation.ed25519_sign_sub_transaction = &ed25519SignSubTransaction;
    implementation.ed25519_get_public_key       = &ed25519PublicKey;
    implementation.serialize                    = std::get<Ed25519KeyCallbacks>(state->callbacks).serialize ? &serializeHandler : nullptr;
  }

  cardano_secure_key_handler_t* value  = nullptr;
  const cardano_error_t         status = cardano_secure_key_handler_new(implementation, &value);
  if (status != CARDANO_SUCCESS)
  {
    delete state;
    checkCardanoError(status, "SecureKeyHandler");
  }

  return value;
}

/** @brief Converts C++ derivation paths for native signing.
 * @param paths The C++ paths.
 * @return The native paths.
 */
std::vector<cardano_derivation_path_t>
nativePaths(const std::vector<DerivationPath>& paths)
{
  std::vector<cardano_derivation_path_t> native;
  native.resize(paths.size());

  for (std::size_t index = 0; index < paths.size(); ++index)
  {
    const DerivationPath& path = paths[index];
    native[index]              = { path.purpose, path.coinType, path.account, static_cast<std::uint64_t>(path.role), path.index };
  }

  return native;
}

/** @brief Copies native witnesses into a C++ value.
 * @param value The native witnesses.
 * @return The copied value.
 */
VkeyWitnessSet
copyWitnesses(cardano_vkey_witness_set_t* value)
{
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(value);
  return VkeyWitnessSet::fromCbor(encodeCbor(owner.get(), cardano_vkey_witness_set_to_cbor));
}
} // namespace

/* CLASS IMPLEMENTATION ******************************************************/

SecureKeyHandler
SecureKeyHandler::bip32(std::string_view name, Bip32KeyCallbacks callbacks)
{
  if (!callbacks.signTransaction || !callbacks.signSubTransaction || !callbacks.accountPublicKey)
  {
    throw std::invalid_argument("BIP32 handler requires transaction, sub-transaction, and account-public-key callbacks");
  }

  CallbackContext* state = new CallbackContext(std::move(callbacks));
  return SecureKeyHandler(createHandler(name, state, SecureKeyHandlerType::Bip32), SecureKeyHandlerType::Bip32);
}

SecureKeyHandler
SecureKeyHandler::ed25519(std::string_view name, Ed25519KeyCallbacks callbacks)
{
  if (!callbacks.signTransaction || !callbacks.signSubTransaction || !callbacks.publicKey)
  {
    throw std::invalid_argument("Ed25519 handler requires transaction, sub-transaction, and public-key callbacks");
  }

  CallbackContext* state = new CallbackContext(std::move(callbacks));
  return SecureKeyHandler(createHandler(name, state, SecureKeyHandlerType::Ed25519), SecureKeyHandlerType::Ed25519);
}

SecureKeyHandler::SecureKeyHandler(const SecureKeyHandler& other)
: m_value(other.checked())
, m_type(other.m_type)
{
  cardano_secure_key_handler_ref(m_value);
}

SecureKeyHandler::SecureKeyHandler(SecureKeyHandler&& other) noexcept
: m_value(std::exchange(other.m_value, nullptr))
, m_type(other.m_type)
{
}

SecureKeyHandler::~SecureKeyHandler()
{
  cardano_secure_key_handler_unref(&m_value);
}

std::string
SecureKeyHandler::name() const
{
  return cardano_secure_key_handler_get_name(checked());
}

std::string
SecureKeyHandler::lastError() const
{
  return cardano_secure_key_handler_get_last_error(checked());
}

SecureKeyHandlerType
SecureKeyHandler::type() const
{
  checked();
  return m_type;
}

VkeyWitnessSet
SecureKeyHandler::signTransaction(const Transaction& transaction, const std::vector<DerivationPath>& paths) const
{
  if (m_type != SecureKeyHandlerType::Bip32)
  {
    throw std::logic_error("BIP32 paths require a BIP32 handler");
  }

  if (paths.empty())
  {
    throw std::invalid_argument("At least one BIP32 key path is required");
  }

  const std::vector<std::uint8_t>                                 bytes = transaction.toCbor();
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> native(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(bytes, cardano_transaction_from_cbor));
  const std::vector<cardano_derivation_path_t>                              nativePath = nativePaths(paths);
  cardano_vkey_witness_set_t*                                               output     = nullptr;
  const cardano_error_t                                                     status     = cardano_secure_key_handler_bip32_sign_transaction(checked(), native.get(), nativePath.data(), nativePath.size(), &output);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(output);
  checkCardanoError(status, "SecureKeyHandler signTransaction");
  return copyWitnesses(owner.release());
}

VkeyWitnessSet
SecureKeyHandler::signSubTransaction(const SubTransaction& transaction, const std::vector<DerivationPath>& paths) const
{
  if (m_type != SecureKeyHandlerType::Bip32)
  {
    throw std::logic_error("BIP32 paths require a BIP32 handler");
  }

  if (paths.empty())
  {
    throw std::invalid_argument("At least one BIP32 key path is required");
  }

  const std::vector<std::uint8_t>                                         bytes = transaction.toCbor();
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> native(
    decodeCbor<cardano_sub_transaction_t, cardano_sub_transaction_unref>(bytes, cardano_sub_transaction_from_cbor));
  const std::vector<cardano_derivation_path_t>                              nativePath = nativePaths(paths);
  cardano_vkey_witness_set_t*                                               output     = nullptr;
  const cardano_error_t                                                     status     = cardano_secure_key_handler_bip32_sign_sub_transaction(checked(), native.get(), nativePath.data(), nativePath.size(), &output);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(output);
  checkCardanoError(status, "SecureKeyHandler signSubTransaction");
  return copyWitnesses(owner.release());
}

VkeyWitnessSet
SecureKeyHandler::signTransaction(const Transaction& transaction) const
{
  if (m_type != SecureKeyHandlerType::Ed25519)
  {
    throw std::logic_error("Ed25519 signing requires an Ed25519 handler");
  }

  const std::vector<std::uint8_t>                                 bytes = transaction.toCbor();
  CardanoObject<cardano_transaction_t, cardano_transaction_unref> native(
    decodeCbor<cardano_transaction_t, cardano_transaction_unref>(bytes, cardano_transaction_from_cbor));
  cardano_vkey_witness_set_t*                                               output = nullptr;
  const cardano_error_t                                                     status = cardano_secure_key_handler_ed25519_sign_transaction(checked(), native.get(), &output);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(output);
  checkCardanoError(status, "SecureKeyHandler signTransaction");
  return copyWitnesses(owner.release());
}

VkeyWitnessSet
SecureKeyHandler::signSubTransaction(const SubTransaction& transaction) const
{
  if (m_type != SecureKeyHandlerType::Ed25519)
  {
    throw std::logic_error("Ed25519 signing requires an Ed25519 handler");
  }

  const std::vector<std::uint8_t>                                         bytes = transaction.toCbor();
  CardanoObject<cardano_sub_transaction_t, cardano_sub_transaction_unref> native(
    decodeCbor<cardano_sub_transaction_t, cardano_sub_transaction_unref>(bytes, cardano_sub_transaction_from_cbor));
  cardano_vkey_witness_set_t*                                               output = nullptr;
  const cardano_error_t                                                     status = cardano_secure_key_handler_ed25519_sign_sub_transaction(checked(), native.get(), &output);
  CardanoObject<cardano_vkey_witness_set_t, cardano_vkey_witness_set_unref> owner(output);
  checkCardanoError(status, "SecureKeyHandler signSubTransaction");
  return copyWitnesses(owner.release());
}

Bip32PublicKey
SecureKeyHandler::accountPublicKey(const AccountDerivationPath& path) const
{
  if (m_type != SecureKeyHandlerType::Bip32)
  {
    throw std::logic_error("Account derivation requires a BIP32 handler");
  }

  const cardano_account_derivation_path_t                                   nativePath { path.purpose, path.coinType, path.account };
  cardano_bip32_public_key_t*                                               value  = nullptr;
  const cardano_error_t                                                     status = cardano_secure_key_handler_bip32_get_extended_account_public_key(checked(), nativePath, &value);
  CardanoObject<cardano_bip32_public_key_t, cardano_bip32_public_key_unref> owner(value);
  checkCardanoError(status, "SecureKeyHandler accountPublicKey");

  std::vector<std::uint8_t> bytes(cardano_bip32_public_key_get_bytes_size(owner.get()));
  checkCardanoError(cardano_bip32_public_key_to_bytes(owner.get(), bytes.data(), bytes.size()), "SecureKeyHandler accountPublicKey");
  return Bip32PublicKey(bytes);
}

Ed25519PublicKey
SecureKeyHandler::publicKey() const
{
  if (m_type != SecureKeyHandlerType::Ed25519)
  {
    throw std::logic_error("Ed25519 public key requires an Ed25519 handler");
  }

  cardano_ed25519_public_key_t*                                                 value  = nullptr;
  const cardano_error_t                                                         status = cardano_secure_key_handler_ed25519_get_public_key(checked(), &value);
  CardanoObject<cardano_ed25519_public_key_t, cardano_ed25519_public_key_unref> owner(value);
  checkCardanoError(status, "SecureKeyHandler publicKey");

  std::vector<std::uint8_t> bytes(cardano_ed25519_public_key_get_bytes_size(owner.get()));
  checkCardanoError(cardano_ed25519_public_key_to_bytes(owner.get(), bytes.data(), bytes.size()), "SecureKeyHandler publicKey");
  return Ed25519PublicKey(bytes);
}

std::vector<std::uint8_t>
SecureKeyHandler::serialize() const
{
  cardano_buffer_t*                                     value  = nullptr;
  const cardano_error_t                                 status = cardano_secure_key_handler_serialize(checked(), &value);
  CardanoObject<cardano_buffer_t, cardano_buffer_unref> owner(value);
  checkCardanoError(status, "SecureKeyHandler serialize");

  const std::uint8_t* bytes = cardano_buffer_get_data(owner.get());
  if (cardano_buffer_get_size(owner.get()) == 0U)
  {
    return {};
  }

  return { bytes, bytes + cardano_buffer_get_size(owner.get()) };
}

void
SecureKeyHandler::swap(SecureKeyHandler& other) noexcept
{
  std::swap(m_value, other.m_value);
  std::swap(m_type, other.m_type);
}

SecureKeyHandler&
SecureKeyHandler::operator=(const SecureKeyHandler& other)
{
  if (this != &other)
  {
    SecureKeyHandler copy(other);
    swap(copy);
  }

  return *this;
}

SecureKeyHandler&
SecureKeyHandler::operator=(SecureKeyHandler&& other) noexcept
{
  if (this != &other)
  {
    cardano_secure_key_handler_unref(&m_value);
    m_value = std::exchange(other.m_value, nullptr);
    m_type  = other.m_type;
  }

  return *this;
}

/* PRIVATE FUNCTIONS *********************************************************/

SecureKeyHandler::SecureKeyHandler(cardano_secure_key_handler_t* value, SecureKeyHandlerType type) noexcept
: m_value(value)
, m_type(type)
{
}

cardano_secure_key_handler_t*
SecureKeyHandler::checked() const
{
  if (!m_value)
  {
    throw std::logic_error("Cannot observe a moved-from SecureKeyHandler");
  }

  return m_value;
}

/* FUNCTIONS IMPLEMENTATION **************************************************/

void
swap(SecureKeyHandler& lhs, SecureKeyHandler& rhs) noexcept
{
  lhs.swap(rhs);
}
} // namespace Cometa
