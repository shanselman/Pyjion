Disassembler
============

You can see the machine code for the compiled function by disassembling it in the Python REPL.
Pyjion has essentially compiled your small Python function into a small, standalone application.

Prerequisites
-------------

The X86_64 disassembler requires ``rich`` and ``distorm3``. You can install the Prerequisites using the ``pyjion[dis]`` package:

.. code-block:: 

   > pip install pyjion[dis]

CIL Disassembler
----------------

Pyjion's compiler uses ECMA 335 CIL as the intermediary language. You can see the IL for a compiled function using the ``pyjion.dis.dis(f)`` function:

.. code-block:: 

    >>> import pyjion
    >>> import pyjion.dis
    >>> def half(x):
        return x / 2
    >>> pyjion.enable()
    >>> half(4)
    2.0
    >>> pyjion.disable()
    IL_0000: ldarg.1
    IL_0001: ldc.i4 104
    IL_0006: conv.i
    IL_0007: add
    IL_0008: stloc.0
    IL_0009: ldarg.2
    ...

For a reference of the CIL, see `ECMA335 reference <https://github.com/tonybaloney/ecma-335/tree/master/docs>`_.

The ``dis()`` function also has an extra setting to print the sequence-points of the Python opcodes (see Python's ``dis`` module):

.. code-block:: 

    >>> import pyjion
    >>> import pyjion.dis
    >>> def half(x):
        return x / 2
    >>> pyjion.enable()
    >>> half(4)
    2.0
    >>> pyjion.disable()
    >>> pyjion.dis.dis(half, include_offsets=True)

    IL_002b: conv.i
    IL_002c: add
    IL_002d: stind.i
    // 2 STORE_NAME - 0 (a)
    IL_002e: ldloc.0
    IL_002f: ldc.i4.2
    IL_0030: conv.u4
    IL_0031: stind.i4
    IL_0032: ldarg.1
    IL_0033: ldc.i8 140106856465584
    IL_003c: conv.i
    IL_003d: call METHOD_STORENAME_TOKEN

X86_64 Disassembler
-------------------

The disassembler uses a project called distorm3. I recommend disabling Pyjion before running the disassembler, because it will JIT-compile distorm3 (which is massive).

To run the X86_64 disassembler:

.. code-block::

    >>> import pyjion
    >>> import pyjion.dis
    >>> def half(x):
           return x / 2
    >>> pyjion.enable()
    >>> half(4)
     2.0
    >>> pyjion.disable()
    >>> pyjion.dis.dis_native(half)
    PUSH RBP
    SUB RSP, 0x290
    LEA RBP, [RSP+0x290]
    XOR EAX, EAX
    MOV [RBP-0x288], RAX
    XORPS XMM8, XMM8
    MOVAPS [RBP-0x280], XMM8
    MOV RAX, 0xfffffffffffffdc0
    MOVAPS [RBP+RAX-0x30], XMM8
    MOVAPS [RBP+RAX-0x20], XMM8
    MOVAPS [RBP+RAX-0x10], XMM8
    ADD RAX, 0x30
    JNZ 0x7f6d0c013fcf
    MOV [RBP-0x30], RAX
    MOV [RBP-0x8], RDI
    MOV [RBP-0x10], RSI
    MOV [RBP-0x18], RDX
    MOV [RBP-0x20], RCX
    MOV [RBP-0x28], R8
    MOV EDI, 0x68
    ...

The ``dis_native()`` function has an optional flag to print the offsets of the Python bytecodes as comments:

.. code-block:: 

    >>> import pyjion
    >>> import pyjion.dis
    >>> def half(x):
        return x / 2
    >>> pyjion.enable()
    >>> half(4)
    2.0
    >>> pyjion.disable()
    >>> pyjion.dis.dis_native(half, include_offsets=True)

    MOVSXD RDI, EDI
    MOV [RBP-0x58], RDI
    MOV EDI, 0x1
    MOVSXD RDI, EDI
    MOV RSI, 0x7f6d2b7b7950
    ADD [RSI], RDI
    MOV [RBP-0xc8], RSI
    ; 2 STORE_NAME - 0 (a)
    MOV RDI, [RBP-0x30]
    MOV DWORD [RDI], 0x2
    MOV RDI, [RBP-0xc8]
    MOV RSI, [RBP-0x10]
    MOV RDX, 0x7f6d2b6904b0
    MOV RAX, 0x7f6d20ff1350
    CALL QWORD [RAX] ; METHOD_STORENAME_TOKEN
    MOV [RBP-0xcc], EAX
    CMP DWORD [RBP-0xcc], 0x0
    JZ 0x7f6d0c0140a4
    MOV RDI, [RBP-0x10]
    MOV RAX, 0x7f6d20ff0350
    CALL QWORD [RAX] ; METHOD_EH_TRACE
    NOP
    JMP 0x7f6d0c01496f
    MOV EDI, 0x1
    MOVSXD RDI, EDI
    MOV RSI, 0x7f6d286f4330
    ADD [RSI], RDI

External disassemblers
----------------------

If you prefer to use an external disassembler, like Hopper, you can use the ``pyjion.dump_native()`` function to fetch the bytearray and write it to disk.

.. code-block:: python

    >>> import pyjion
    >>> def half(x):
        return x / 2
    >>> pyjion.enable()
    >>> half(4)
    2.0
    >>> pyjion.disable()
    >>> raw, length, position = pyjion.dump_native(x)
    >>> with open('dump.raw', 'wb') as out:
    ...   out.write(raw)

In your disassembler, open the ``dump.raw`` file as a raw file. The ``position`` variable shown in this example is the base address of the JIT in memory:

.. image:: _static/hopper.png
    :width: 50%
    :align: center
