import contextlib
import io

import pyjion
import pyjion.dis


def test_import():
    def _f():
        print("foo foo")
        return 2

    assert _f() == 2
    info = pyjion.info(_f)
    assert info['compiled']
    f = io.StringIO()
    with contextlib.redirect_stdout(f):
        pyjion.dis.dis(_f)
    assert "ldarg.1" in f.getvalue()
    assert "METHOD_LOADGLOBAL_HASH" in f.getvalue()
