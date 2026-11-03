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
