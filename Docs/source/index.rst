.. Pyjion documentation master file, created by
   sphinx-quickstart on Thu May 19 09:26:14 2016.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Pyjion
======

Pyjion drop-in JIT compiler for CPython 3.9. Pyjion can make your Python code execute faster without any code changes.

Trying Pyjion
-------------

Checkout the live Pyjion compiler and disassembler sandbox at `trypyjion.com <https://www.trypyjion.com>`_.

Prerequisites
-------------

- Pyjion requires `.NET 6 <https://dotnet.microsoft.com/download>`_ to be installed.
- Pyjion requires `Python 3.9 <https://python.org/downloads>`_.

Installing Pyjion
-----------------

Pyjion is best installed from PyPi within a Python 3.9 virtual environment. There are binary wheels for macOS, Linux and Windows. Make sure you have updated pip before installing.

To install from PyPi, use pip to install from a virtual environment:

.. code-block:: console

   $ python -m pip install pyjion

Using Pyjion
------------

After following the installation steps, pyjion is a python module that you can import a Python 3.9 environment.

After importing pyjion, enable it by calling `pyjion.enable()` which sets a compilation threshold to 0 (the code only needs to be run once to be compiled by the JIT):

.. code-block::

    >>> import pyjion
    >>> pyjion.enable()


Any Python code you define or import after enabling pyjion will be JIT compiled. You don't need to execute functions in any special API, its completely transparent:

.. code-block::

    >>> def half(x):
    ...    return x/2
    >>> half(2)
    1.0

Pyjion will have compiled the `half` function into machine code on-the-fly and stored a cached version of that compiled function inside the function object.
You can see some basic stats by running `pyjion.info(f)`, where `f` is the function object:

.. code-block::

    >>> pyjion.info(half)
    {'failed': False, 'compiled': True, 'run_count': 1}

You can also execute Pyjion against any script or module:

.. code-block:: console

    pyjion my_script.py

Or, for an existing Python module:

.. code-block:: console

    pyjion -m calendar

For a full list of options, see the :ref:`API`.

.. toctree::
    :glob:
    :maxdepth: 3
    :hidden:

    development
    debugging
    disassembler
    graphing
    api
    wsgi
    optimizations
    limitations
