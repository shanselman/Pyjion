import pyjion.dis
import pyjion
import io
import contextlib
import dis
import sys
import pytest


@pytest.mark.tracing
def test_traces_in_code():
    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    assert _f() == 10
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_TRACE_FRAME_ENTRY" in f.getvalue()
    assert "METHOD_TRACE_LINE" in f.getvalue()
    assert "METHOD_TRACE_FRAME_EXIT" in f.getvalue()


@pytest.mark.tracing
def test_custom_tracer():
    def custom_trace(frame, event, args):
        frame.f_trace_opcodes = True
        if event == 'opcode':
            out = io.StringIO()
            dis.disco(frame.f_code, frame.f_lasti, file=out)
            lines = out.getvalue().split('\\n')
            [print(f"{l}") for l in lines]
            out.close()
        elif event == 'call':
            print(f"Calling {frame.f_code}")
        elif event == 'return':
            print(f"Returning {args}")
        elif event == 'line':
            print(f"Changing line to {frame.f_lineno}")
        else:
            print(f"{frame} ({event} - {args})")
        return custom_trace

    def test_f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        sys.settrace(custom_trace)
        assert test_f() == 10
        sys.settrace(None)

    assert "Calling <code object test_f" in f.getvalue()
    assert "Changing line to " in f.getvalue()
    assert "Returning " in f.getvalue()
    assert "STORE_FAST" in f.getvalue()


@pytest.mark.tracing
def test_eh_tracer():
    def custom_trace(frame, event, args):
        frame.f_trace_opcodes = True
        if event == 'exception':
            print("Hit exception")
            print(*args)
        return custom_trace

    def test_f():
        a = 1 / 0
        return a

    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        sys.settrace(custom_trace)
        with pytest.raises(ZeroDivisionError):
            test_f()
        sys.settrace(None)

    assert "Hit exception" in f.getvalue()


@pytest.mark.profile
def test_profile_hooks_in_code():
    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    assert _f() == 10
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_PROFILE_FRAME_ENTRY" in f.getvalue()
    assert "METHOD_PROFILE_FRAME_EXIT" in f.getvalue()


@pytest.mark.profile
def test_custom_tracer():
    def custom_trace(frame, event, args):
        frame.f_trace_opcodes = True
        if event == 'opcode':
            with io.StringIO() as out:
                dis.disco(frame.f_code, frame.f_lasti, file=out)
                lines = out.getvalue().split('\\n')
                [print(f"{l}") for l in lines]
        elif event == 'call':
            print(f"Calling {frame.f_code}")
        elif event == 'return':
            print(f"Returning {args}")
        elif event == 'line':
            print(f"Changing line to {frame.f_lineno}")
        else:
            print(f"{frame} ({event} - {args})")
        return custom_trace

    def _f():
        a = 1
        b = 2
        c = 3
        d = 4
        return a + b + c + d

    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        sys.setprofile(custom_trace)
        assert _f() == 10
        sys.setprofile(None)

    assert "Calling <code object test_f" in f.getvalue()
    assert "Returning " in f.getvalue()
