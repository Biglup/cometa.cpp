Cometa.cpp
==========

.. image:: _static/logo.png
   :width: 300
   :align: center
   :alt: Cometa.cpp

Documentation for version |version|.

Overview
--------

Cometa.cpp is a C++17 library for Cardano, built on cardano-c. It provides domain
values, transaction builders, signing, and network providers for applications on
Linux, macOS, and Windows.

Main features
~~~~~~~~~~~~~

* Shelley and Byron address parsing and generation.
* Ed25519 signatures, BIP32 derivation, and BIP39 mnemonics.
* Transaction serialization, inspection, building, signing, and submission.
* Native assets, staking, and Conway governance.
* Native Plutus evaluation and script parameter application.
* Dijkstra subtransactions, guards, and direct reward-account deposits.
* Injectable coin selection, transaction evaluation, signing, and HTTP transport.

C++ interface
~~~~~~~~~~~~~

Values manage their resources automatically and support independent copies.
Builders are move-only. APIs use standard strings, containers, optional values,
and exceptions. Applications do not manage C reference counts or output buffers.
See :doc:`design` for ownership and callback behavior.

Cross-platform integration
~~~~~~~~~~~~~~~~~~~~~~~~~~

Packages provide static and shared libraries with CMake targets. The optional
HTTP transport uses WinHTTP on Windows and libcurl on Linux and macOS. Start with
:doc:`getting_started` for installation and linking.

Examples and contributions
~~~~~~~~~~~~~~~~~~~~~~~~~~

The `examples directory <https://github.com/Biglup/cometa.cpp/tree/main/examples>`_
contains complete transaction flows. See the
`contributing guide <https://github.com/Biglup/cometa.cpp/blob/main/CONTRIBUTING.md>`_
for development conventions.

License
~~~~~~~

Cometa.cpp is licensed under the
`Apache License 2.0 <https://github.com/Biglup/cometa.cpp/blob/main/LICENSE>`_.

.. toctree::
   :hidden:
   :caption: Getting Started

   getting_started
   design
   error_handling
   collections

.. toctree::
   :hidden:
   :caption: API Reference
   :maxdepth: 2

   sections/api/address/index
   sections/api/assets/index
   sections/api/auxiliary_data/index
   sections/api/certs/index
   sections/api/common/index
   sections/api/cryptography/index
   sections/api/cbor/index
   sections/api/encoding/index
   sections/api/json/index
   sections/api/key_handlers/index
   sections/api/plutus_data/index
   sections/api/pool_params/index
   sections/api/proposal_procedures/index
   sections/api/protocol_params/index
   sections/api/providers/index
   sections/api/scripts/index
   sections/api/transaction/index
   sections/api/transaction_body/index
   sections/api/transaction_builder/index
   sections/api/voting_procedures/index
   sections/api/witness_set/index
   sections/api/message_signing/index
   sections/api/error
   sections/api/slot_config
