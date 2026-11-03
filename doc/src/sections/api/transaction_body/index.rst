Transaction Body
================

The body records inputs, outputs, fees, validity bounds, and optional fields such as
minting, withdrawals, certificates, and governance actions. Dijkstra adds
subtransactions, guards, direct deposits, and account balance requirements. These types
expose the ledger representation; TransactionBuilder handles fee and change
calculations.

.. toctree::
   :maxdepth: 1

   usage
   account_balance_interval
   account_balance_intervals_map
   direct_deposit_map
   guard_set
   required_guards_map
   sub_transaction_body
   transaction_body
   transaction_input
   transaction_input_set
   transaction_output
   transaction_output_list
   value
