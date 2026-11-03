Transaction
===========

A Transaction combines a body, witnesses, auxiliary data, and a validity flag. Construct
transactions with TransactionBuilder, inspect or serialize them as values, then apply
signatures. Changing the body after signing changes the transaction ID and invalidates
those signatures. Dijkstra SubTransaction values represent independently signed intents
included in a top-level transaction.

.. toctree::
   :maxdepth: 1

   usage
   sub_transaction
   sub_transaction_set
   transaction
