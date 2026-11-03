Address usage
=============

Constructing and inspecting addresses
-------------------------------------

Build an address from credentials or parse its bytes or text. Credentials passed
to a constructor and returned by getters are independent values.

.. code-block:: cpp

   const Cometa::BaseAddress paymentAddress(
     Cometa::NetworkId::Testnet,
     paymentCredential,
     stakeCredential);

   const std::string text = paymentAddress.toString();

   const Cometa::Address                    parsed = Cometa::Address::fromString(text);
   const std::optional<Cometa::BaseAddress> base   = parsed.asBase();

``Address`` accepts Bech32 Shelley addresses and Base58 Byron addresses.
``asBase``, ``asEnterprise``, ``asPointer``, ``asReward``, and ``asByron`` return
``std::nullopt`` for a different address format; allocation failures still throw.
The specialized ``fromAddress`` factories report a wrong format as ``Cometa::Error``.

Shelley byte input must be canonical and contain exactly one address. Text input
checks the network/type prefix and rejects mixed case. Uppercase Bech32 input is
accepted and formatted as lowercase. Network IDs preserve their four-bit value.

``PointerAddress`` currently accepts slot, transaction index, and certificate index
values up to ``UINT32_MAX``. Larger values are rejected rather than truncated.
Byron attributes use an owned encrypted derivation path of at most 64 bytes and
an optional nonnegative network magic; absence denotes mainnet.
