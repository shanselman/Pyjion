.. _Limitations:

Known Limitations
=================

Exception Handling
------------------

Pyjion does not currently support ``try..except`` blocks. 

Initial implementations were buggy, due to CPython being tightly coupled to fields inside frame objects. Ceval's frame evaluation loop keeps two stacks, a value stack and an exception block handler stack.
Have both Pyjion and CPython evaluate frames causes a number of issues.

Some work has been done to support this.

With statements
---------------

Pyjion does not currently support ``with`` blocks. 

This is on the roadmap (and related to try..except).

Async/Await and coroutines
--------------------------

Pyjion does not currently support ``async..await`` (``YIELD_FROM``) statements.

This is because the Python types used to wrap coroutines are private to libpython.
