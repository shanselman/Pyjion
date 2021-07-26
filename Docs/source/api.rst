.. _API:

API Reference
=============

Command-line
------------

You can run ``pyjion`` as a standalone command-line program, similar to how you would use ``python`` to run scripts and modules.

.. code-block::

   usage: pyjion [-h] [-m MODULE] [--tracing] [--profiling] [--graph] [--debug] [--no-pgc] [-o OPT_LEVEL] [script]

   Python JIT Compiler

   positional arguments:
   script                script file

   optional arguments:
   -h, --help            show this help message and exit
   -m MODULE             Execute module
   --tracing             Enable tracing (used by debuggers)
   --profiling           Enable profiling
   --graph               Enable instruction graph generation
   --debug               Enable debuggable JIT methods
   --no-pgc              Disable PGC
   -o OPT_LEVEL, --opt-level OPT_LEVEL
                           Optimization level (default 1

Core module
-----------

.. module:: pyjion

.. function:: enable()

   Enable the JIT

.. function:: disable()

   Disable the JIT

.. function:: enable_tracing()

   Enable tracing hooks (e.g. used by debuggers) to be emitted in the compiled CIL. Required for :ref:`Debugging`.

.. function:: disable_tracing()

   Disable tracing hooks.

.. function:: enable_profiling()

   Enable profiling hooks (e.g. used by performance profilers) to be emitted in the compiled CIL. See :ref:`Debugging`

.. function:: disable_profiling()

   Disable profiling hooks.

.. function:: enable_debug()

   Sets debug flags in the .NET JIT Compiler. Used for :ref:`Disassembler` output.

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
   
   Get sequence points for a compiled function (used by the :ref:`Disassembler`)

.. function::  enable_graphs() -> None:
   
   Enable graph generation, see :ref:`Graphing`

.. function::  disable_graphs() -> None:
   
   Disable graph generation.

.. function::  get_graph(f: Callable) -> str:
   
   Get graph for compiled function, see :ref:`Graphing`

.. function::  status() -> Dict[Any, Any]:

   Get Pyjion runtime status.

.. function::  symbols(f: Callable) -> Dict[int, str]:

   Get method symbol table (used by the :ref:`Disassembler`).

Disassembly module
------------------

.. module:: pyjion.dis

.. function:: dis(f, include_offsets=False, print_pc=True)

   Print the ECMA CIL bytecode in a disassembly table.
   Set ``include_offsets=True`` to print the Python opcodes inline with the IL.

.. function:: dis_native(f, include_offsets=False, print_pc=True)

   Print the x86 assembly instructions in a disassembly table (requires distorm3 and rich)
   Set ``include_offsets=True`` to print the Python opcodes inline with the assembly.

WSGI middleware
---------------

.. module:: pyjion.wsgi

.. class:: PyjionWsgiMiddleware(application)

   Provides a WSGI middleware interface that enables the JIT for requests