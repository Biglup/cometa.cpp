Development
===========

.. code-block:: console

   valgrind --leak-check=full --errors-for-leak-kinds=all --error-exitcode=1 build/cometa-tests
   python3 -m pip install -r doc/requirements.txt
   cmake -S doc -B build/docs
   cmake --build build/docs --target doc

Documentation uses Doxygen XML, Breathe, Sphinx, and the Immaterial theme, matching
the cardano-c documentation pipeline. Read the Docs builds use the same configuration.

Implementation records
----------------------

The :doc:`coverage` ledger records the native API mapping. It is a development
record, separate from the public API reference.

.. toctree::
   :maxdepth: 1

   coverage
