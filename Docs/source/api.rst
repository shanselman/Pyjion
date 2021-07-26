.. _API:

API Reference
=============

Core module
-----------

.. module:: pyjion

.. function:: enable()

   Enable the JIT

.. function:: disable()

   Disable the JIT

.. function:: enable_tracing()

   Enable tracing hooks (e.g. used by debuggers) to be emitted in the compiled CIL. Required for debugging.

.. function:: disable_tracing()

   Disable tracing hooks.

.. function:: enable_profiling()

   Enable profiling hooks (e.g. used by performance profilers) to be emitted in the compiled CIL.

.. function:: disable_profiling()

   Disable profiling hooks.

.. function:: enable_debug()

   Enable compilation of JIT as debuggable methods. I.e., to include debug data (useful for pyjion.dis.dis_native(f, include_offsets=True)

.. function:: disable_debug()

   Disable emitting debuggable JIT methods.

.. function:: enable_pgc()

   Enable compilation profiling (already enabled by default)

.. function:: disable_pgc()

   Disable compilation profiling

.. function:: set_threshold(to: int)

   Set the threshold to JIT compile a function to the number of times it is executed.

.. function:: dump_il(f)

   Return the ECMA CIL bytecode as a bytearray

.. function:: dump_native(f)

   Return the compiled machine-code as a bytearray

.. function:: get_offsets(f: Callable) -> tuple[tuple[int, int, int]]:
   
   Get sequence points for a compiled function (used by the disassembler)

.. function::  enable_graphs() -> None:
   
   Enable graph generation, see :ref:`graphing`

.. function::  disable_graphs() -> None:
   
   Disable graph generation.

.. function::  get_graph(f: Callable) -> str:
   
   Get graph for compiled function, see :ref:`graphing`

.. function::  status() -> Dict[Any, Any]:

   Get Pyjion runtime status.

.. function::  symbols(f: Callable) -> Dict[int, str]:

   Get method symbol table (used by the disassembler).


Disassembly module
------------------

.. automodule:: pyjion.dis
   :members:
   :undoc-members:

WSGI middleware
---------------

.. automodule:: pyjion.wsgi
   :members:
   :undoc-members: