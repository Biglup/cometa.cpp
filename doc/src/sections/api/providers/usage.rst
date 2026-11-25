Providers usage
===============

Using Blockfrost
----------------

The default transport is available when libcurl is found on Unix or WinHTTP is
enabled on Windows. Supply a project ID for the selected network and pass the
transport to ``BlockfrostProvider``:

.. code-block:: cpp

   #include <Cometa/Providers/BlockfrostProvider.h>
   #include <Cometa/Providers/DefaultHttpTransport.h>

   #include <cstdlib>
   #include <memory>
   #include <stdexcept>

   const char* projectId = std::getenv("BLOCKFROST_API_KEY");

   if (projectId == nullptr)
   {
     throw std::runtime_error("Set BLOCKFROST_API_KEY");
   }

   std::shared_ptr<Cometa::HttpTransport> http =
     std::make_shared<Cometa::DefaultHttpTransport>();

   Cometa::BlockfrostProvider provider(
     Cometa::NetworkMagic::Preprod,
     projectId,
     http);

   Cometa::ProtocolParameters parameters = provider.getParameters();

Link the HTTP target for the chosen link mode. The project ID is sent in
the Blockfrost ``project_id`` header. Set it at runtime and keep it out of
source files and release archives. For a different HTTP stack, implement
``HttpTransport`` and pass it to the same provider.

Koios, Kupmios and Hydra
---------------------------

All three providers require an explicit network. They never infer it from an
endpoint URL, read credentials implicitly, or retry transaction submissions.
Protocol prices and monetary quantities retain their exact values. HTTP failures
remain errors rather than empty UTxO sets. Responses are limited to 8 MiB.

Koios uses its extended UTxO endpoints and Ogmios v6 HTTP service. Address, asset,
and input queries follow offset pages of 100, reject duplicate output identities,
and stop with an error after 10,000 pages. Separate requests are not an atomic
ledger snapshot. Kupmios combines Kupo 2.x with Ogmios v6 over HTTP; both endpoints
must use the selected network. Kupo must index the addresses and assets being
queried and retain history needed for confirmation. Large match sets exceeding
the response limit fail explicitly.

Hydra connects to an existing Conway Head over HTTP. It reads the Head's actual
protocol parameters, including its fee settings, and submits to ``/transaction``
on layer 2. It does not open, commit to, close, or fan out a Head, and never uses
``/cardano-transaction`` to submit to layer 1. A ``202 SubmitTxSubmitted`` response
acknowledges submission without claiming confirmation.

Hydra confirmation polls ``/snapshot`` for the transaction's CBOR in the latest
confirmed snapshot. It validates the transaction ID from those bytes. This API
cannot recover transactions that have already disappeared from the latest
snapshot's transaction list; a timeout means confirmation was not observed.
A committed L1 UTxO is not proof of an L2 transaction. Applications requiring a
complete history should consume and retain Hydra's ``SnapshotConfirmed`` event
stream. Snapshot confirmation is not L1 settlement.

Hydra has no remote evaluation endpoint or staking rewards. These operations
fail explicitly. Use a local script evaluator configured with the Head's ledger
parameters. Datum lookup searches inline datums in the current confirmed UTxO
set and cannot recover spent or hash-only datum bytes.
