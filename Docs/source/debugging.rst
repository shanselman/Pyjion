.. _Debugging:

Debugging
=========

IDE debuggers, like VS Code and PyCharm use a callback system in Python called **tracing**. By default, Pyjion has tracing disabled. To debug Python code with Pyjion, you must enable tracing.

Call tracing
------------

Python's tracing callback API adds overhead to execution. To improve performance, Pyjion will only compile these callbacks when tracing is enabled ahead of time.

Before calling ``sys.settrace()``:

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
    sys.settrace(custom_tracer)
    assert _f() == 10
    sys.settrace(None)

Profiling
---------

Python profilers (like the ``profile`` module) require runtime callbacks in Python execution.

See `Profiling <https://docs.python.org/3/library/profile.html>` for the Python reference.

Call ``sys.setprofile()`` to set a profile function:

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
    sys.setprofile(custom_profiler)
    assert _f() == 10
    sys.setprofile(None)


