import pyjion
import pyjion.dis


def test_import(capsys):
    def _f():
        print("foo foo")
        return 2

    assert _f() == 2
    info = pyjion.info(_f)
    assert info['compiled']
    captured = capsys.readouterr()
    pyjion.dis.dis(_f)
    assert "ldarg.1" in captured.out
    assert "METHOD_LOADGLOBAL_HASH" in captured.out
