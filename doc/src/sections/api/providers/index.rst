Providers
=========

Provider supplies protocol parameters, UTxOs, rewards, transaction submission, and
optional remote evaluation. BlockfrostProvider implements this interface. Its
HttpTransport dependency can use the supplied platform transport or an application’s
HTTP client.

.. toctree::
   :maxdepth: 1

   usage
   blockfrost_provider
   curl_http_transport
   default_http_transport
   http_transport
   provider
   win_http_transport
