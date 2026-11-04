Transaction Builder
===================

TransactionBuilder assembles and balances transactions from available UTxOs, outputs,
scripts, and current protocol parameters. It computes fees and change and evaluates
Plutus scripts. Coin selection and evaluation can be replaced through C++ interfaces.
SubTransactionBuilder builds Dijkstra intents for inclusion in a batch.

.. toctree::
   :maxdepth: 1

   usage
   fee
   script_data_hash
   implicit_coin
   transaction_balance_request
   transaction_balancing
   coin_selection_request
   coin_selection_result
   coin_selector
   deferred_redeemer
   large_first_coin_selector
   native_transaction_evaluator
   provider_transaction_evaluator
   random_improve_coin_selector
   selection_strategy
   sub_transaction_builder
   transaction_builder
   transaction_evaluator
