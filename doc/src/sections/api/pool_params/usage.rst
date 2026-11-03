Pool Parameters usage
=====================

Pool primitives
---------------

``Ipv4`` and ``Ipv6`` accept network-order bytes and provide CBOR, text, and byte
conversions. IPv4 text contains four decimal octets. IPv6 text currently requires
eight groups of four hexadecimal digits; compressed forms such as ``::1`` are
not accepted. Address equality compares bytes, regardless of hexadecimal case.

.. code-block:: cpp

   const Cometa::Ipv4 address = Cometa::Ipv4::fromString("192.0.2.1");

   const std::vector<std::uint8_t> bytes = address.toBytes();

   const Cometa::Blake2bHash hash = Cometa::Blake2bHash::fromHex(std::string(64, '1'));
   Cometa::PoolMetadata      metadata("https://example.com/pool.json", hash);

   metadata.setUrl("https://example.com/new.json");

``PoolMetadata`` stores a URL of at most 128 bytes and a 32-byte content hash.
An empty URL is allowed; embedded NUL characters are rejected by constructors and
setters. Copies and returned hashes are independent values.

``BlsKey`` stores a 96-byte public key and a 48-byte proof of possession. Construct
it from two byte vectors, or use ``fromHex`` with 192 and 96 hexadecimal characters.
It supports CBOR and CIP-116 JSON. These operations validate the encoding and
lengths; they do not verify the proof cryptographically.

Pool relays
-----------

``SingleHostAddrRelay`` holds optional IPv4/IPv6 addresses and a port.
``SingleHostNameRelay`` holds a DNS name and optional port; ``MultiHostNameRelay``
holds a DNS name for SRV lookup. DNS names contain 1 to 64 bytes without embedded
NULs. These values describe relay configuration; they do not perform DNS lookup.

.. code-block:: cpp

   Cometa::SingleHostAddrRelay ipRelay(3001, Cometa::Ipv4::fromString("192.0.2.1"));

   ipRelay.setIpv6(Cometa::Ipv6::fromString("0000:0000:0000:0000:0000:0000:0000:0001"));
   ipRelay.setIpv4(std::nullopt);

   Cometa::Relays relays {
     Cometa::Relay(ipRelay),
     Cometa::Relay(Cometa::SingleHostNameRelay("relay.example.com", 3001)),
     Cometa::Relay(Cometa::MultiHostNameRelay("_cardano._tcp.example.com"))
   };

   const std::vector<std::uint8_t> cbor = relays.toCbor();

``Relay`` holds one of the three variants. Inspect ``type`` before calling
``asSingleHostAddr``, ``asSingleHostName``, or ``asMultiHostName``; a mismatched
accessor throws ``Error``. Returned variants and addresses are independent values.

Optional setters accept ``std::nullopt`` to remove a field. Port zero is distinct
from an absent port. ``Relays`` preserves insertion order and duplicates;
``pushBack`` copies its argument, and ``at`` returns an independent relay.

Pool owners
-----------

``PoolOwners`` stores unique 28-byte pool owner key hashes in sorted order.
Inserting an existing hash returns ``false``. The collection copies inserted
values and returns independent values from ``at`` and ``values``.

.. code-block:: cpp

   Cometa::PoolOwners owners;

   owners.insert(Cometa::Blake2bHash::fromHex(std::string(56, '1')));

   const std::vector<std::uint8_t> cbor     = owners.toCbor();
   const Cometa::PoolOwners        restored = Cometa::PoolOwners::fromCbor(cbor);

The CBOR encoder applies set tag 258. The decoder also accepts an untagged
array, rejects duplicate hashes, and rejects malformed lengths.

Pool parameters
---------------

``PoolParams`` holds the operator and VRF hashes, pledge, cost, margin, reward
address, owner hashes, relay list, and optional metadata or BLS key. It copies
all supplied values, and each component getter returns an independent value.

.. code-block:: cpp

   Cometa::PoolParams params(
     operatorHash,
     vrfHash,
     10'000'000,
     340'000,
     Cometa::UnitInterval(1, 20),
     rewardAddress,
     owners,
     relays);

   params.setMetadata(Cometa::PoolMetadata(
     "https://example.com/pool.json",
     metadataHash));

   const std::vector<std::uint8_t> cbor     = params.toCbor();
   const Cometa::PoolParams        restored = Cometa::PoolParams::fromCbor(cbor);

CBOR contains the pool parameter fields as a sequence, with no outer array.
``setBlsKey(std::nullopt)`` and ``setMetadata(std::nullopt)`` remove optional fields.
