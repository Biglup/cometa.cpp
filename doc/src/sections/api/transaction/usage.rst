Transaction usage
=================

Signed transactions
-------------------

``SubTransaction`` pairs a sub-transaction body with witnesses and optional auxiliary data. ``applyVkeyWitnesses`` merges signatures into its witness set, and ``id()`` returns the body hash used for signing.

``Transaction`` pairs a top-level body with witnesses and optional auxiliary data. It exposes the validity flag, required signer discovery, and ledger-ordered indexes for redeemers and transaction components. ``applyVkeyWitnesses`` merges signatures into its witness set.

Subtransaction collections
--------------------------

``SubTransactionSet`` keeps sub-transactions in insertion order and rejects duplicate body hashes. Use ``findById`` to look up an intent by its hash.
