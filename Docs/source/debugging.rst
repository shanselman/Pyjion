.. _Debugging:

Debugging
=========

IDE debuggers, like VS Code and PyCharm use a callback system in Python called **tracing**. By default, Pyjion has tracing disabled. To debug Python code with Pyjion, you must enable tracing.

Call tracing
------------

Python's tracing callback API adds overhead to execution. To improve performance, Pyjion will only compile these callbacks when tracing is enabled ahead of time.

Before calling ``sys.settrace()`` you must enable the tracing hooks by calling ``pyjion.enable_tracing()``:

.. code-block:: python

    import sys
    import pyjion

    def custom_tracer(frame, event, args):
        ...

    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    pyjion.enable()
    pyjion.enable_tracing()
    sys.settrace(custom_tracer)
    assert _f() == 10
    sys.settrace(None)

You can disable tracing again by calling ``pyjion.disable_tracing()``

Profiling
---------

Python profilers (like the ``profile`` module) require runtime callbacks in Python execution. Python's profiling callback API adds overhead to execution. To improve performance, Pyjion will only compile these callbacks when profiling is enabled ahead of time.

See `Profiling <https://docs.python.org/3/library/profile.html>` for the Python reference.

Before calling ``sys.setprofile()`` you must enable the profiling hooks by calling ``pyjion.enable_profiling()``:

.. code-block:: python

    import sys
    import pyjion

    def custom_profiler(frame, event, args):
        ...

    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    pyjion.enable()
    pyjion.enable_profiling()
    sys.setprofile(custom_profiler)
    assert _f() == 10
    sys.setprofile(None)

You can disable profiling again by calling ``pyjion.disable_profiling()``

Command Line
------------

If you're running scripts and modules using the ``-m pyjion`` command, you can use the ``--tracing`` and ``-profiling`` flags:

Enabling tracing:

.. code-block::

   > pyjion --tracing my_script.py

Enabling profiling:

.. code-block::

   > pyjion --profiling my_script.py
