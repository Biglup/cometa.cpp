Error Handling
==============

Operation failures throw ``Cometa::Error``, derived from ``std::system_error``.
Use ``ErrorCode`` to identify the failure:

.. code-block:: cpp

   try
   {
     Cometa::AssetName asset = Cometa::AssetName::fromCbor(bytes);
   }
   catch (const Cometa::Error& error)
   {
     if (error.code() == Cometa::ErrorCode::Decoding)
     {
       // Handle invalid CBOR.
     }
   }

Invalid arguments throw ``std::invalid_argument``. Allocation failures reported
as such throw ``std::bad_alloc``. CBOR parsing rejects trailing bytes, and
``Blake2bHash::compute`` requires nonempty input.

Copy assignment leaves the destination unchanged when copying fails. Destruction,
moves, and swaps do not throw.
