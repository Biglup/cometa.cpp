Certificates usage
==================

Certificate types
-----------------

``CertificateType`` identifies every supported certificate variant. Use
``toString(type)`` when a readable type name is needed. Legacy staking and
MIR variants remain represented so historical transactions can be inspected.

Legacy stake registration
-------------------------

``StakeRegistrationCert`` represents the historical stake registration form.
It copies the supplied credential; ``credential()`` returns an independent
value. For current transactions, choose the deposit-bearing registration
certificate when that ledger form is required.

.. code-block:: cpp

   Cometa::StakeRegistrationCert cert(stakeCredential);

   const std::vector<std::uint8_t>     cbor = cert.toCbor();
   const Cometa::StakeRegistrationCert restored =
     Cometa::StakeRegistrationCert::fromCbor(cbor);

Legacy stake deregistration
---------------------------

``StakeDeregistrationCert`` represents the historical stake deregistration
form. It copies its credential and provides CBOR and CIP-116 JSON conversion.

Deposit-bearing registration certificates
-----------------------------------------

``RegistrationCert`` and ``UnregistrationCert`` hold a stake credential and
a deposit amount in lovelace. They copy credentials and return independent
values. The two forms encode with distinct CBOR certificate tags.

.. code-block:: cpp

   Cometa::RegistrationCert   registration(stakeCredential, 2'000'000);
   Cometa::UnregistrationCert unregistration(stakeCredential, 2'000'000);

Stake delegation
----------------

``StakeDelegationCert`` associates a stake credential with a 28-byte pool key
hash. It copies both components and provides CBOR and inspection JSON.

.. code-block:: cpp

   Cometa::StakeDelegationCert delegation(stakeCredential, poolKeyHash);

   const Cometa::StakeDelegationCert restored =
     Cometa::StakeDelegationCert::fromCbor(delegation.toCbor());

Pool registration certificate
-----------------------------

``PoolRegistrationCert`` contains a copied ``PoolParams`` value. The ``params``
getter returns an independent value, which can be changed and supplied to
``setParams``.

.. code-block:: cpp

   Cometa::PoolRegistrationCert registration(params);

   const Cometa::PoolRegistrationCert restored =
     Cometa::PoolRegistrationCert::fromCbor(registration.toCbor());

Pool retirement certificate
---------------------------

``PoolRetirementCert`` stores a 28-byte pool key hash and the retirement epoch.
The hash getter returns an independent value.

.. code-block:: cpp

   Cometa::PoolRetirementCert retirement(poolKeyHash, retirementEpoch);

Combined stake registration and delegation
------------------------------------------

``StakeRegistrationDelegationCert`` registers a stake credential and delegates
to a pool in one certificate. It stores a deposit in lovelace and returns
independent credential and pool hash values.

.. code-block:: cpp

   Cometa::StakeRegistrationDelegationCert cert(
     stakeCredential,
     poolKeyHash,
     2'000'000);

Genesis key delegation
----------------------

``GenesisKeyDelegationCert`` represents the historical delegation of a
genesis key to a delegate key and VRF key. It requires 28-byte genesis and
delegate hashes and a 32-byte VRF hash.

Historical MIR pot transfers
----------------------------

``MirToPotCert`` represents an instantaneous reward transfer between the
reserve and treasury pots. ``MirPotType`` selects the source pot; amounts are
expressed in lovelace. This historical certificate remains available for
inspection of older transactions.

``MirToStakeCredentialsCert`` records a historical distribution from either
pot to stake credentials. Its entries contain a credential and an amount in
lovelace.

``MirCert`` holds either historical form. The ``type()`` discriminator
selects ``asToPot()`` or ``asToStakeCredentials()``.
