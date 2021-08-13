.. _Limitations:

Known Limitations
=================

With statements
---------------

Pyjion does not currently support ``with`` blocks. 

This is on the roadmap (and related to try..except).

Async/Await and coroutines
--------------------------

Pyjion does not currently support ``async..await`` (``YIELD_FROM``) statements.

This is because the Python types used to wrap coroutines are private to libpython.
