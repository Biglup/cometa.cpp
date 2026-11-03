Common usage
============

Hash and credential sets
------------------------

``Blake2bHashSet`` and ``CredentialSet`` keep elements sorted and unique.
``insert`` returns false for a duplicate; ``at`` and ``values`` return independent
copies, so changing a returned credential does not alter membership.

.. code-block:: cpp

   Cometa::CredentialSet credentials { paymentCredential };
   const bool inserted = credentials.insert(stakeCredential);
   const std::vector<Cometa::Credential> snapshot = credentials.values();
   const std::vector<std::uint8_t> cbor = credentials.toCbor();

Parsing accepts tagged or untagged arrays and rejects duplicate elements.
Encoding always writes a sorted definite-length array with CBOR set tag 258.
An untagged, unsorted, or indefinite-length input therefore need not round-trip
byte for byte. Other tags, including oversized values that could narrow to 258,
are rejected. ``toJson`` writes the elements as a sorted JSON array.

Delegated representatives
-------------------------

``DRep`` represents a key or script credential, abstention, or no confidence.
Key and script representatives also support bech32 identifiers.

``VoteDelegationCert`` assigns a stake credential's voting rights to a DRep,
including the predefined abstain and no-confidence choices.

``StakeVoteDelegationCert`` combines pool delegation and DRep delegation for
one stake credential.

``VoteRegistrationDelegationCert`` registers a stake credential and delegates
its voting rights in one certificate. It records the registration deposit in
lovelace.

``StakeVoteRegistrationDelegationCert`` combines stake registration, pool
delegation, and DRep delegation with one registration deposit.

``UnregisterDRepCert`` unregisters a DRep credential and records the returned
deposit in lovelace.

``RegisterDRepCert`` registers a DRep credential with a deposit and an optional
metadata anchor. The anchor can be replaced or removed.

``UpdateDRepCert`` records a DRep credential and its optional metadata anchor.

``AuthCommitteeHotCert`` authorizes a committee hot credential with a cold
credential.

``ResignCommitteeColdCert`` records a committee cold credential's resignation
and optional anchor.

``Certificate`` holds any ledger certificate and exposes its
``CertificateType`` and independent typed values.

``CertificateSet`` keeps certificates in insertion order and accepts any
``Certificate`` variant. Accessors return independent values::

   Cometa::CertificateSet certificates;
   certificates.add(Cometa::Certificate(registration));
   const Cometa::Certificate first = certificates.at(0);
